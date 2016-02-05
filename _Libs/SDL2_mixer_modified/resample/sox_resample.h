# ifndef RESAMPLE_H
# define RESAMPLE_H

struct SoxResampler {
    int rate_in;
    int rate_out;
    int channels;
    double factor;
    //struct rs_data *soxr;
};

struct SoxResampler *SoxResamplerINIT(int rate_in, int rate_out, unsigned int channels, int buffSize);

int SoxResamplerProcess(struct SoxResampler * res, char *input, int in_len, char *output[], int *outlen);

void SoxResamplerFree(struct SoxResampler * res);


# endif
