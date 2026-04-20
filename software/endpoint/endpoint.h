// Copyright (c) ELCT201-002 Group 2, 2026

#ifndef ENDPOINT_H
#define ENDPOINT_H


#define TX_PIN      18
#define RX_PIN      5
#define BUTTON_PIN  16
#define SPEAKER_PIN 17

#define LEDC_PRECISION 8
#define LEDC_SAMPLE_RATE 1000
#define LEDC_DUTY 128

// Whether to play audio through the speaker
void speakerEnable(bool enable); 

#endif  // ENDPOINT_H
