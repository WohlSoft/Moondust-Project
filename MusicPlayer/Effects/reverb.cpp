#include <vector>
#include <deque>
#include <cmath>
#ifdef USE_SDL_MIXER_X
#include <SDL2/SDL_mixer_ext.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#include "reverb.h"

struct Reverb /* This reverb implementation is based on Freeverb impl. in Sox */
{
    float feedback, hf_damping, gain;
    struct FilterArray
    {
        struct Filter
        {
            std::vector<float> Ptr;  size_t pos;  float Store;
            void Create(size_t size) { Ptr.resize(size); pos = 0; Store = 0.f; }
            float Update(float a, float b)
            {
                Ptr[pos] = a;
                if(!pos) pos = Ptr.size()-1; else --pos;
                return b;
            }
            float ProcessComb(float input, const float feedback, const float hf_damping)
            {
                Store = Ptr[pos] + (Store - Ptr[pos]) * hf_damping;
                return Update(input + feedback * Store, Ptr[pos]);
            }
            float ProcessAllPass(float input)
            {
                return Update(input + Ptr[pos] * .5f, Ptr[pos]-input);
            }
        } comb[8], allpass[4];
        void Create(double rate, double scale, double offset)
        {
            /* Filter delay lengths in samples (44100Hz sample-rate) */
            static const int comb_lengths[8] = {1116,1188,1277,1356,1422,1491,1557,1617};
            static const int allpass_lengths[4] = {225,341,441,556};
            double r = rate * (1 / 44100.0); // Compensate for actual sample-rate
            const int stereo_adjust = 12;
            for(size_t i=0; i<8; ++i, offset=-offset)
                comb[i].Create( scale * r * (comb_lengths[i] + stereo_adjust * offset) + .5 );
            for(size_t i=0; i<4; ++i, offset=-offset)
                allpass[i].Create( r * (allpass_lengths[i] + stereo_adjust * offset) + .5 );
        }
        void Process(size_t length,
            const std::deque<float>& input, std::vector<float>& output,
            const float feedback, const float hf_damping, const float gain)
        {
            for(size_t a=0; a<length; ++a)
            {
                float out = 0, in = input[a];
                for(size_t i=8; i-- > 0; ) out += comb[i].ProcessComb(in, feedback, hf_damping);
                for(size_t i=4; i-- > 0; ) out += allpass[i].ProcessAllPass(out);
                output[a] = out * gain;
            }
        }
    } chan[2];
    std::vector<float> out[2];
    std::deque<float> input_fifo;
    void Create(double sample_rate_Hz,
        double wet_gain_dB,
        double room_scale, double reverberance, double fhf_damping, /* 0..1 */
        double pre_delay_s, double stereo_depth,
        size_t buffer_size)
    {
        size_t delay = pre_delay_s  * sample_rate_Hz + .5;
        double scale = room_scale * .9 + .1;
        double depth = stereo_depth;
        double a =  -1 /  std::log(1 - /**/.3 /**/);          // Set minimum feedback
        double b = 100 / (std::log(1 - /**/.98/**/) * a + 1); // Set maximum feedback
        feedback = 1 - std::exp((reverberance*100.0 - b) / (a * b));
        hf_damping = fhf_damping * .3 + .2;
        gain = std::exp(wet_gain_dB * (std::log(10.0) * 0.05)) * .015;
        input_fifo.insert(input_fifo.end(), delay, 0.f);
        for(size_t i = 0; i <= std::ceil(depth); ++i)
        {
            chan[i].Create(sample_rate_Hz, scale, i * depth);
            out[i].resize(buffer_size);
        }
    }
    void Process(size_t length)
    {
        for(size_t i=0; i<2; ++i)
            if(!out[i].empty())
                chan[i].Process(length,
                    input_fifo,
                    out[i], feedback, hf_damping, gain);
        input_fifo.erase(input_fifo.begin(), input_fifo.begin() + length);
    }
};

struct MyReverbData
{
    bool        wetonly;
    unsigned    amplitude_display_counter = 0;
    float       prev_avg_flt[2];

    Reverb chan[2];
    MyReverbData() :
        wetonly(false),
        amplitude_display_counter(0),
        prev_avg_flt{0,0}
    {

        for(size_t i=0; i<2; ++i)
            chan[i].Create(44100,
                4.0,  // wet_gain_dB  (-10..10)
                .7,   // room_scale   (0..1)
                .8,   // reverberance (0..1)
                .3,   // hf_damping   (0..1)
                .000, // pre_delay_s  (0.. 0.5)
                1,   // stereo_depth (0..1)
                16384);
    }
};

static MyReverbData* reverb_data = NULL;

void reverbEffect(int, void *stream, int len, void *)
{
    if(!reverb_data)
        reverb_data = new MyReverbData();

    int count = len/4;
    short* samples = (short*)stream;
    if(count % 2 == 1)
    {
        // An uneven number of samples? To avoid complicating matters,
        // just ignore the odd sample.
        count   -= 1;
    }

    if(!count)
        return;

    // Attempt to filter out the DC component. However, avoid doing
    // sudden changes to the offset, for it can be audible.
    double average[2] = {0, 0};
    for(unsigned w=0; w<2; ++w)
        for(int p = 0; p < count; ++p)
          average[w] += samples[p*2+w];

    float *prev_avg_flt = reverb_data->prev_avg_flt;

    float average_flt[2] =
    {
        prev_avg_flt[0] = (prev_avg_flt[0] + average[0]*0.04/double(count)) / 1.04,
        prev_avg_flt[1] = (prev_avg_flt[1] + average[1]*0.04/double(count)) / 1.04
    };

    if(!reverb_data->amplitude_display_counter--)
    {
        reverb_data->amplitude_display_counter = (44100 / count) / 24;
        double amp[2]={0,0};
        for(int w=0; w<2; ++w)
        {
            average[w] /= double(count);
            for(int p = 0; p < count; ++p)
                amp[w] += std::fabs(samples[p*2+w] - average[w]);
            amp[w] /= double(count);
            // Turn into logarithmic scale
            const double dB = std::log(amp[w]<1 ? 1 : amp[w]) * 4.328085123;
            const double maxdB = 3*16; // = 3 * log2(65536)
            amp[w] = dB/maxdB;
        }
        //UI.IllustrateVolumes(amp[0], amp[1]);
    }

    Reverb *chan = reverb_data->chan;

    // Convert input to float format
    std::vector<float> dry[2];
    for(int w=0; w<2; ++w)
    {
        dry[w].resize(count);
        float a = average_flt[w];
        for(int p = 0; p < count; ++p)
        {
            int s = samples[p*2+w];
            dry[w][p] = (s - a) * double(0.3/32768.0);
        }
        // ^  Note: ftree-vectorize causes an error in this loop on g++-4.4.5
        chan[w].input_fifo.insert(
        chan[w].input_fifo.end(),
            dry[w].begin(), dry[w].end());
    }

    // Reverbify it
    for(unsigned w=0; w<2; ++w)
        reverb_data->chan[w].Process(count);

    for(int p = 0; p < count; ++p)
        for(int w=0; w<2; ++w)
        {
            float out = ((1 - reverb_data->wetonly) * dry[w][p] +
                         .5 * (chan[0].out[w][p] + chan[1].out[w][p])) * 32768.0f
                            + average_flt[w];
              samples[p*2+w] = short(out<-32768.f ? -32768 : out>32767.f ?  32767 : out);
        }
}

void reverbEffectDone(int, void *)
{
    if(reverb_data)
        delete reverb_data;
    reverb_data = NULL;
}
