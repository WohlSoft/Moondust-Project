#include "SDL_mixer_ext.h"
#include "sox_resample.h"

struct SoxResampler *ResamplerINIT(int rate_in, int rate_out, unsigned int channels)
{
    struct SoxResampler *res = (struct SoxResampler*)malloc(sizeof(struct SoxResampler));
    if(!res)
    {
        Mix_SetError("Resampler: Out of memory!");
        return NULL;
    }
    res->rate_in=rate_in;
    res->rate_out=rate_out;
    res->channels=channels;
    res->soxr = soxr_create(
        (double)rate_in, (double)rate_out, channels,  /* Input rate, output rate, # of channels. */
        &res->error,                                  /* To report any error during creation. */
        NULL, NULL, NULL);                            /* Use configuration defaults.*/
    return res;
}

