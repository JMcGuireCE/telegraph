// ============================================================
//  Morse Code Open-Drain Relay — Nucleo-G474RE (Arduino/STM32duino)
//
//  Flow:
//    IC2 signal → detected on IC2_IN → relayed via IC1_OUT
//    IC1 signal → detected on IC1_IN → relayed via IC2_OUT
// ============================================================

// --- Pin Definitions (Arduino header labels) -----------------
const int IC1_IN  = A0;   // PA0 — reads  IC1 bus
const int IC1_OUT = A5;   // PA1 — drives IC1 bus LOW
const int IC2_IN  = A2;   // PA4 — reads  IC2 bus
const int IC2_OUT = A3;   // PB0 — drives IC2 bus LOW

// --- Bail timeout (ms) ---------------------------------------
const unsigned long BAIL_MS = 700;

// --- Open-drain helpers --------------------------------------
inline void busAssert (int pin) { digitalWrite(pin, LOW);  }
inline void busRelease(int pin) { digitalWrite(pin, HIGH); }

// --- Monitor and relay ---------------------------------------
void checkAndRelay(int inPin, int outPin, const char *label) {
  if (digitalRead(inPin) == HIGH) return;

  unsigned long t0 = millis();

  while (digitalRead(inPin) == LOW) {
    if (millis() - t0 > BAIL_MS) {
      Serial.print("[BAIL] ");
      Serial.print(label);
      Serial.println(" — pulse too long, ignoring.");
      return;
    }
  }

  unsigned long duration = millis() - t0;

  Serial.print("[RELAY] ");
  Serial.print(label);
  Serial.print(" — ");
  Serial.print(duration);
  Serial.println(" ms");

  busAssert(outPin);
  delay(duration);
  busRelease(outPin);
}

// --- Setup ---------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(IC1_IN,  INPUT_PULLUP);
  pinMode(IC2_IN,  INPUT_PULLUP);
  pinMode(IC1_OUT, OUTPUT);
  pinMode(IC2_OUT, OUTPUT);
  busRelease(IC1_OUT);
  busRelease(IC2_OUT);

  Serial.println("\r\n============================================");
  Serial.println(" Morse Relay ready — Nucleo-G474RE / Arduino");
  Serial.println("  IC1_IN=A0  IC1_OUT=A1");
  Serial.println("  IC2_IN=A2  IC2_OUT=A3");
  Serial.println("============================================\r\n");
}

void loop() {
  bool ic1Low = (digitalRead(IC1_IN) == LOW);
  bool ic2Low = (digitalRead(IC2_IN) == LOW);

  if (ic1Low) checkAndRelay(IC1_IN, IC2_OUT, "IC1→IC2");
  if (ic2Low) checkAndRelay(IC2_IN, IC1_OUT, "IC2→IC1");
}
