typedef enum {ROTATE, STOP, SCAN, UP, HOME} MOTOR_STATES;
typedef struct {
    MOTOR_STATES state;
    unsigned long prev;
    unsigned long tau;
    int STEP_PIN;
    int DIR_PIN;
    bool direction; // true for clockwise, false for counter-clockwise
    const int steps; // Number of steps to move
    int currentStep; // Current step position
    bool scanned;
    bool stopped;
    bool home; // Flag to indicate if the motor is at home position
} motor;

motor setupMotor(int stepPin, int dirPin, int enPin, unsigned long tau, bool direction, int steps);
void motorControl(motor *m);

motor m1, m2;
void setup() {
    m1 = setupMotor(25, 33, 32, 5, true, 10); // Motor 1
    m2 = setupMotor(14, 27, 26, 5, false, 200); // Motor 2
}

void loop() {
    motorControl(&m1);
    motorControl(&m2);
}

motor setupMotor(int stepPin, int dirPin, int enPin, unsigned long tau, bool direction, int steps) {
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(enPin, OUTPUT);
    digitalWrite(dirPin, direction ? HIGH : LOW);
    digitalWrite(enPin, HIGH);
    motor m;
    m.tau = tau;
    m.prev = millis();
    m.STEP_PIN = stepPin;
    m.DIR_PIN = dirPin;
    m.direction = direction;
    m.steps = steps;
    m.currentStep = 0;
    m.scanned = false;
    m.stopped = false;
    m.home = false;
    m.state = SCAN; // Initial state is SCAN
    return m;
}

void motorControl(motor *m) {
    switch (m->state) {
        case SCAN:
            if ((m->currentStep < m->steps)&&(m->scanned)&& !m->stopped) {
                m->state = ROTATE;
                digitalWrite(m->STEP_PIN, HIGH);
                m->prev = millis();
            } else if((m->currentStep >= m->steps) && (m->scanned)&& !m->stopped) {
                m->state = UP;
                m->prev = millis();
            } else if (m->stopped) {
                m->state = HOME;
            }
            break;

        case ROTATE:
            if ((millis() - m->prev) > m->tau/2) {
                digitalWrite(m->STEP_PIN, HIGH);
                delayMicroseconds(100); // Pulse width
                digitalWrite(m->STEP_PIN, LOW);
                m->prev = millis();
                m->currentStep++;
            }
            if (m->currentStep >= m->steps) {
                m->state = STOP;
            }
            break;

        case STOP:
            // Motor is stopped, can implement logic to wait or reset
            break;

        case UP:
            // Implement UP logic if needed
            break;

        case HOME:
            // Implement HOME logic if needed
            break;
    }
}