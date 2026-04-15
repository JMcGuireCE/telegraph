// Relay firmware — MCU bridges two isolated open-drain bus segments

const int LEFT_IN = 4;   // sense left bus (HIGH = idle, LOW = active)
const int LEFT_OUT = 16;  // drive left N-MOS gate (HIGH = pull bus LOW)
const int RIGHT_IN = 17;  // sense right bus
const int RIGHT_OUT = 18;  // drive right N-MOS gate
const int LED_GREEN_LR = 25;
const int LED_GREEN_RL = 26;
const int LED_RED = 27;

enum State { IDLE, FORWARD_LR, FORWARD_RL, COLLISION };
State state = IDLE;

unsigned long collisionUntil = 0;
const unsigned long LATCH_MS = 500;

void setup() {
  Serial.begin(115200);
  pinMode(LEFT_IN, INPUT);
  pinMode(RIGHT_IN, INPUT);
  pinMode(LEFT_OUT, OUTPUT);
  pinMode(RIGHT_OUT, OUTPUT);
  pinMode(LED_GREEN_LR, OUTPUT);
  pinMode(LED_GREEN_RL, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LEFT_OUT,  LOW);   // don't drive either bus
  digitalWrite(RIGHT_OUT, LOW);
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
        digitalWrite(LEFT_OUT,  LOW);
        digitalWrite(RIGHT_OUT, LOW);
        digitalWrite(LED_RED, HIGH);

      } else if (l) {
        state = FORWARD_LR;
        digitalWrite(RIGHT_OUT, HIGH);    // mirror left onto right
        digitalWrite(LED_GREEN_LR, HIGH);

      } else if (r) {
        state = FORWARD_RL;
        digitalWrite(LEFT_OUT, HIGH);     // mirror right onto left
        digitalWrite(LED_GREEN_RL, HIGH);
      }
      break;

    case FORWARD_LR:
      if (r) {
        // far side started keying too
        state = COLLISION;
        collisionUntil = millis() + LATCH_MS;
        digitalWrite(RIGHT_OUT, LOW);
        digitalWrite(LED_GREEN_LR, LOW);
        digitalWrite(LED_RED, HIGH);

      } else if (l) {
        digitalWrite(RIGHT_OUT, HIGH);    // keep mirroring

      } else {
        digitalWrite(RIGHT_OUT, LOW);     // key released, stop
        digitalWrite(LED_GREEN_LR, LOW);
        state = IDLE;
      }
      break;

    case FORWARD_RL:
      if (l) {
        state = COLLISION;
        collisionUntil = millis() + LATCH_MS;
        digitalWrite(LEFT_OUT, LOW);
        digitalWrite(LED_GREEN_RL, LOW);
        digitalWrite(LED_RED, HIGH);

      } else if (r) {
        digitalWrite(LEFT_OUT, HIGH);
        
      } else {
        digitalWrite(LEFT_OUT, LOW);
        digitalWrite(LED_GREEN_RL, LOW);
        state = IDLE;
      }
      break;

    case COLLISION:
      digitalWrite(LEFT_OUT,  LOW);
      digitalWrite(RIGHT_OUT, LOW);
      if (millis() >= collisionUntil && !l && !r) {
        digitalWrite(LED_RED, LOW);
        state = IDLE;
      }
      break;
  }
}