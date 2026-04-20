// ============================================================
//  Morse Code Open-Drain Relay — ESP32
//  Schematic: IC1 (left) <——One-Wire——> IC2 (right)
//
//  Each side has a dedicated INPUT and OUTPUT pin:
//    IC1_IN  — reads  the IC1 bus (what IC1 is sending)
//    IC1_OUT — drives the IC1 bus LOW (ESP32 → IC1)
//    IC2_IN  — reads  the IC2 bus (what IC2 is sending)
//    IC2_OUT — drives the IC2 bus LOW (ESP32 → IC2)
//
//  Flow:
//    Signal from IC2 → detected on IC2_IN → relayed via IC1_OUT
//    Signal from IC1 → detected on IC1_IN → relayed via IC2_OUT
//
//  Echo prevention: IC1_OUT and IC1_IN are separate physical pins,
//  so driving IC1_OUT LOW does NOT affect IC1_IN. No lock flags needed.
//
//  Standard Morse timing @ 12 WPM (1 unit = 100 ms):
//    DOT  : 50  – 200 ms
//    DASH : 200 – 600 ms
// ============================================================

// --- Pin Definitions -------------------------------------------
const int IC1_IN  = 35;   // GPIO reads  IC1 bus  (INPUT)
const int IC1_OUT = 13;   // GPIO drives IC1 bus  (OUTPUT, open-drain)
const int IC2_IN  = 19;  // GPIO reads  IC2 bus  (INPUT)
const int IC2_OUT = 4;  // GPIO drives IC2 bus  (OUTPUT, open-drain)

// --- Morse Timing (ms) -----------------------------------------
const unsigned long UNIT     = 100;
const unsigned long DOT_MIN  =  50;
const unsigned long DOT_MAX  = 200;
const unsigned long DASH_MIN = 200;
const unsigned long DASH_MAX = 600;

// ---------------------------------------------------------------
// Open-drain output helpers
// IC1_OUT / IC2_OUT are always OUTPUT mode;
// write HIGH to release (FET off), LOW to assert (FET on).
// ---------------------------------------------------------------
inline void busAssert(int outPin)  { digitalWrite(outPin, LOW);  }
inline void busRelease(int outPin) { digitalWrite(outPin, HIGH); }

// ---------------------------------------------------------------
// Relay a pulse of 'duration' ms onto 'outPin'
// ---------------------------------------------------------------
void relayPulse(int outPin, unsigned long duration) {
  busAssert(outPin);
  delay(duration);
  busRelease(outPin);
}

// ---------------------------------------------------------------
// Monitor 'inPin'; if a valid Morse pulse is detected,
// relay it to 'outPin'.
//
// Because inPin and outPin are completely separate GPIO pins,
// driving outPin LOW has zero effect on inPin — echo loops are
// impossible by hardware design.
// ---------------------------------------------------------------
void checkAndRelay(int inPin, int outPin) {
  // Bus is normally HIGH. LOW = someone asserting the line.
  if (digitalRead(inPin) == HIGH) return;

  // Measure pulse duration
  unsigned long t0 = millis();
  while (digitalRead(inPin) == LOW) {
    if (millis() - t0 > DASH_MAX + 100) return;  // too long, bail
  }
  unsigned long duration = millis() - t0;

  // Classify and relay
  if (duration >= DOT_MIN && duration < DOT_MAX) {
    relayPulse(outPin, UNIT);       // DOT  → 1 unit
  } else if (duration >= DASH_MIN && duration <= DASH_MAX) {
    relayPulse(outPin, UNIT * 3);   // DASH → 3 units
  }
  // else: noise, ignore
}

// ---------------------------------------------------------------
// Setup
// ---------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // Input pins — read the bus
  pinMode(IC1_IN, INPUT);
  pinMode(IC2_IN, INPUT);

  // Output pins — open-drain: start released (HIGH)
  pinMode(IC1_OUT, OUTPUT);
  pinMode(IC2_OUT, OUTPUT);
  busRelease(IC1_OUT);
  busRelease(IC2_OUT);

  Serial.println("Morse Relay ready (4-pin open-drain).");
  Serial.println("  IC1_IN=" + String(IC1_IN) + "  IC1_OUT=" + String(IC1_OUT));
  Serial.println("  IC2_IN=" + String(IC2_IN) + "  IC2_OUT=" + String(IC2_OUT));
}

// ---------------------------------------------------------------
// Loop — poll both input sides, relay to opposite output
// ---------------------------------------------------------------
void loop() {
  // IC2 sent something → relay to IC1
  checkAndRelay(IC2_IN, IC1_OUT);

  // IC1 sent something → relay to IC2
  checkAndRelay(IC1_IN, IC2_OUT);
}
