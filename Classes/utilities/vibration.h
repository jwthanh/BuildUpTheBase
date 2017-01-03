#ifndef VIBRATION_H
#define VIBRATION_H

void do_vibrate(int milliseconds);

#ifdef __ANDROID__
void vibrate(int milliseconds);
#else
#endif

#endif
