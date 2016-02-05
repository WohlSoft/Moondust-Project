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
//    res->soxr = resample_init(rate_in, rate_out, buffSize);
//    if(!res->soxr)
//    {
//        free(res);
//        return NULL;
//    }
    res->factor = ((double)rate_out/(double)rate_in)+.5;

    return res;
}

int SoxResamplerProcess(struct SoxResampler * res, char *input, int in_len, char *output[], int *outlen)
{
    if(in_len==0)
    {
        *outlen = 0;
        return 0;
    }
    size_t s_in_len = in_len/2;
    size_t s_out_len = (size_t)((double)(s_in_len)*res->factor);
    short left[s_in_len];
    short right[s_in_len];
    short left_n[s_out_len];
    short right_n[s_out_len];

    int i=0;
    double p, q, r;
    short *lt=left;short *rt=right;
    for(i=0; i<in_len; i+=2)
    {
        *lt++ = input[i];
        *rt++ = input[i+1];
    }

    //Resample left
    for(p=0.0, q=0.0, r=0.0; q<=s_in_len; q+=1.0 )
    {
        int oldSam = (int)p;
        left_n[(int)q] = left[oldSam];
        r+= 1.0/res->factor;
        if(r>=1.0)
        {
            p = roundf(q/res->factor);
            r=0.0;
        }
    }

    //Resample right
    for(p=0.0, q=0.0, r=0.0; q<=s_in_len; q+=1.0 )
    {
        int oldSam = (int)p;
        right_n[(int)q] = right[oldSam];
        r+= 1.0/res->factor;
        if(r>=1.0)
        {
            p = roundf(q/res->factor);
            r=0.0;
        }
    }

    lt=left_n;
    rt=right_n;
    for(i=0; i<s_out_len; ++i)
    {
        output[i*2]=*lt++;
        output[i*2+1]=*rt++;
    }
    *outlen = s_out_len*2;
    return 0;
}


void SoxResamplerFree(struct SoxResampler *res)
{
    if(res)
    {
        //if(res->soxr) resample_close(res->soxr);
        free(res);
    }
}
