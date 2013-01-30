#ifndef ENCODER_MONITOR_H
#define ENCODER_MONITOR_H


void beeper_init();
void beeper_play_tone(uint16_t sound_freq, uint16_t duration);

void beeper_handle_timer_int();

#endif