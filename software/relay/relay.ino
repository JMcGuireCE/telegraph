// Relay firmware — MCU bridges two isolated open-drain bus segments

const int LEFT_IN = 13;   // sense left bus (HIGH = idle, LOW = active)
const int LEFT_OUT = 12;  // drive left N-MOS gate (HIGH = pull bus LOW)
const int RIGHT_IN = 14;  // sense right bus
const int RIGHT_OUT = 27;  // drive right N-MOS gate
const int LED_GREEN_LR = 4;
const int LED_GREEN_RL = 2;
const int LED_RED =15;

enum State { IDLE, FORWARD_LR, FORWARD_RL, COLLISION };
State state = IDLE;

unsigned long collisionUntil = 0;
const unsigned long LATCH_MS = 50;

void setup() {
  Serial.begin(115200);
  pinMode(LEFT_IN, INPUT);
  pinMode(RIGHT_IN, INPUT);
  pinMode(LEFT_OUT, OUTPUT);
  pinMode(RIGHT_OUT, OUTPUT);
  pinMode(LED_GREEN_LR, OUTPUT);
  pinMode(LED_GREEN_RL, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LEFT_OUT,  HIGH);   // don't drive either bus
  digitalWrite(RIGHT_OUT, HIGH);
  Serial.println("Relay ready");
}

void loop() {
  bool l = (digitalRead(LEFT_IN)  == LOW);  // LOW = someone keying
  bool r = (digitalRead(RIGHT_IN) == LOW);

  switch (state) {

    case IDLE:
      if (l && r) {
        state = COLLISION;
        collisionUntil = millis() + LATCH_MS;
        digitalWrite(LEFT_OUT,  HIGH);
        digitalWrite(RIGHT_OUT, HIGH);
        digitalWrite(LED_RED, HIGH);

      } else if (l) {
        state = FORWARD_LR;
        digitalWrite(RIGHT_OUT, LOW);    // mirror left onto right
        digitalWrite(LED_GREEN_LR, HIGH);

      } else if (r) {
        state = FORWARD_RL;
        digitalWrite(LEFT_OUT, LOW);     // mirror right onto left
        digitalWrite(LED_GREEN_RL, HIGH);
      }
      break;

    case FORWARD_LR:
      if (r) {
        // far side started keying too
        state = COLLISION;
        collisionUntil = millis() + LATCH_MS;
        digitalWrite(RIGHT_OUT, HIGH);
        digitalWrite(LED_GREEN_LR, LOW);
        digitalWrite(LED_RED, HIGH);

      } else if (l) {
        digitalWrite(RIGHT_OUT, LOW);    // keep mirroring

      } else {
        digitalWrite(RIGHT_OUT, HIGH);     // key released, stop
        digitalWrite(LED_GREEN_LR, LOW);
        state = IDLE;
      }
      break;

    case FORWARD_RL:
    if (r) {
        digitalWrite(LEFT_OUT, LOW);
        
      } else {
        digitalWrite(LEFT_OUT, HIGH);
        digitalWrite(LED_GREEN_RL, LOW);
        state = IDLE;
      }
      break;

    case COLLISION:
      digitalWrite(LEFT_OUT,  HIGH);
      digitalWrite(RIGHT_OUT, HIGH);
      digitalWrite(LED_RED, HIGH);
      if (millis() >= collisionUntil && !l && !r) {
        digitalWrite(LED_RED, LOW);
        state = IDLE;
      }
      break;
  }
}