// Morse telegraph relay firmware


const int LEFT_IN     = 4;
const int LEFT_OUT    = 5;

const int RIGHT_IN = 37;
const int RIGHT_OUT = 38;

const int LED_RED      = 19;

const int ACTIVE = HIGH;
const int IDLE   = LOW;

enum State { ST_IDLE, ST_FORWARD_LR, ST_FORWARD_RL, ST_COLLISION };
State state = ST_IDLE;

unsigned long collisionUntil = 0;
const unsigned long COLLISION_LATCH_MS = 500;

void busRelease(int pin) { pinMode(pin, INPUT); }
void busDrive(int pin)   { pinMode(pin, OUTPUT); digitalWrite(pin, ACTIVE); }
bool busActive(int pin)  { return digitalRead(pin) == ACTIVE; }

void setup() {
  Serial.begin(115200);
  busRelease(LEFT_BUS);
  busRelease(RIGHT_BUS);
  pinMode(LED_GREEN_LR, OUTPUT);
  pinMode(LED_GREEN_RL, OUTPUT);
  pinMode(LED_RED,      OUTPUT);
  Serial.println("Relay ready");
}

void loop() {
  bool l, r;

  switch (state) {
    case ST_IDLE:
      l = busActive(LEFT_BUS);
      r = busActive(RIGHT_BUS);
      if (l && r) {
        state = ST_COLLISION;
        collisionUntil = millis() + COLLISION_LATCH_MS;
        digitalWrite(LED_RED, HIGH);
        Serial.println("COLLISION");
      } else if (l) {
        state = ST_FORWARD_LR;
        busDrive(RIGHT_BUS);
        digitalWrite(LED_GREEN_LR, HIGH);
      } else if (r) {
        state = ST_FORWARD_RL;
        busDrive(LEFT_BUS);
        digitalWrite(LED_GREEN_RL, HIGH);
      }
      break;

    case ST_FORWARD_LR:
      l = busActive(LEFT_BUS);
      // Briefly release RIGHT to sense if the far side is also keying.
      busRelease(RIGHT_BUS);
      delayMicroseconds(20);
      r = busActive(RIGHT_BUS);
      if (l && r) {
        state = ST_COLLISION;
        collisionUntil = millis() + COLLISION_LATCH_MS;
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN_LR, LOW);
        Serial.println("COLLISION (LR)");
      } else if (l) {
        busDrive(RIGHT_BUS);  // keep mirroring
      } else {
        busRelease(RIGHT_BUS);
        digitalWrite(LED_GREEN_LR, LOW);
        state = ST_IDLE;
      }
      break;

    case ST_FORWARD_RL:
      r = busActive(RIGHT_BUS);
      busRelease(LEFT_BUS);
      delayMicroseconds(20);
      l = busActive(LEFT_BUS);
      if (r && l) {
        state = ST_COLLISION;
        collisionUntil = millis() + COLLISION_LATCH_MS;
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN_RL, LOW);
        Serial.println("COLLISION (RL)");
      } else if (r) {
        busDrive(LEFT_BUS);
      } else {
        busRelease(LEFT_BUS);
        digitalWrite(LED_GREEN_RL, LOW);
        state = ST_IDLE;
      }
      break;

    case ST_COLLISION:
      busRelease(LEFT_BUS);
      busRelease(RIGHT_BUS);
      if (millis() >= collisionUntil &&
          !busActive(LEFT_BUS) && !busActive(RIGHT_BUS)) {
        digitalWrite(LED_RED, LOW);
        state = ST_IDLE;
        Serial.println("cleared");
      }
      break;
  }
}