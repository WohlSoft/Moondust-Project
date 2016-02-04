#include "SDL_mixer_ext.h"
#include "sox_resample.h"

struct SoxResampler *SoxResamplerINIT(int rate_in, int rate_out, unsigned int channels, int buffSize)
{
    struct SoxResampler *res = (struct SoxResampler*)malloc(sizeof(struct SoxResampler));
    if(!res)
    {
        Mix_SetError("SoxResampler: Out of memory!");
        return NULL;
    }
    res->rate_in=rate_in;
    res->rate_out=rate_out;
    res->channels=channels;
    res->soxr = resample_init(rate_in, rate_out, buffSize);
    res->factor = ((double)rate_out/(double)rate_in)+.5;

    return res;
}

int SoxResamplerProcess(struct SoxResampler * res, char *input, int in_len, char *output, int *outlen)
{
    size_t s_in_len = in_len/2;
    size_t s_out_len = (size_t)((double)(s_in_len)*res->factor);
    short left[s_in_len];
    short right[s_in_len];
    short left_n[s_out_len];
    short right_n[s_out_len];

    int i=0;
    short *l=left;short *r=right;
    for(i=0; i<in_len; i+=2)
    {
        *l++ = input[i];
        *r++ = input[i+1];
    }

    int len = resample(res->soxr, left, s_in_len, left_n, s_out_len, 0);
        len = resample(res->soxr, right, s_in_len, right_n, s_out_len, 0);

    l=left_n;
    r=right_n;
    for(i=0; i<len; ++i)
    {
        output[i*2]=*l++;
        output[i*2]=*r++;
    }
    *outlen=len*2;

    return 0;
}

void SoxResamplerFree(struct SoxResampler *res)
{
    if(res)
    {
        if(res->soxr) resample_close(res->soxr);
        free(res);
    }
}
