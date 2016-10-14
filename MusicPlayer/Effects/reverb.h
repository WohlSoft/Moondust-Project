#ifndef REVERB_H
#define REVERB_H

extern void reverbEffect(int chan, void *stream, int len, void *udata);
extern void reverbEffectDone(int chan, void *udata);

#endif // REVERB_H
