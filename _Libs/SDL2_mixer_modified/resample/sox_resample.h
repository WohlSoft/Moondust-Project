# ifndef RESAMPLE_H
# define RESAMPLE_H

#include "soxr/soxr.h"

struct SoxResampler {
    int rate_in;
    int rate_out;
    int channels;
    soxr_t soxr;
    soxr_error_t error;
};


# endif
