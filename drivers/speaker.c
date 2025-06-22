#include "speaker.h"

#include "../lib/std.h"
int32_t beeping = 0;

// // 播放声音
void beep(uint16_t freq, int32_t durat)
{
    uint16_t counter = 1193182 / freq;
    outb(PIT_CTRL_PORT, 0b10110110);
    outb(PIT_CHAN2_PORT, (uint8_t)counter);
    outb(PIT_CHAN2_PORT, (uint8_t)(counter >> 8));
    if (!beeping)
    {
        outb(SPEAKER_PORT, in(SPEAKER_PORT) | 0b11);
        beeping = 1;

        sleep(durat);

        outb(SPEAKER_PORT, in(SPEAKER_PORT) & 0xfc);
        beeping = 0;
    }
}

inline void speaker_play(uint16_t frequency)
{
    beep(frequency, BEEP_MS);
}

// // 停止声音
// void speaker_stop();
