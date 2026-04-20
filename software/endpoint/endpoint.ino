// Copyright (c) ELCT201-002 Group 2, 2026

#include "./endpoint.h"
#include "driver/ledc.h"

bool button = false;
bool rx = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Telegraph Endpoint");
  Serial.println("ELCT 201 Section 2 Group 2");
  
  pinMode(RX_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  pinMode(TX_PIN, OUTPUT);

  Serial.print("Initializing LEDC...");
  ledcAttach(SPEAKER_PIN, LEDC_SAMPLE_RATE, LEDC_PRECISION);
  Serial.println("OK");

  Serial.println("Setup Complete.");
}


void loop() {
  button = digitalRead(BUTTON_PIN);
  rx = digitalRead(RX_PIN);

  digitalWrite(TX_PIN, button);
  speakerEnable(button || rx);
  Serial.print("Button...");
  Serial.println(button);
  Serial.print("RX...");
  Serial.println(rx);
}


void speakerEnable(bool enable) {
  if (enable)
    ledcWrite(SPEAKER_PIN, LEDC_DUTY);
  else
    ledcWrite(SPEAKER_PIN, 0);
}
