/*// Pines conectados al motor 1
const int EN_PIN1   = 32; // ENABLE
const int STEP_PIN1 = 33; // STEP
const int DIR_PIN1  = 25; // DIR

// Pines conectados al motor 2
const int EN_PIN2   = 26; // ENABLE
const int STEP_PIN2 = 27; // STEP
const int DIR_PIN2  = 14; // DIR*/

typedef struct{
    unsigned long prev;
    unsigned long tau;
    int STEP, EN, DIR;
    bool dir; // direction flag
  // true = forward, false = backward
}pulse;

typedef enum{SCAN, ROTATE, ROTATE_ON, UP, UP_ON, HOME, HOME_ON} MOTOR_STATES;
typedef struct{
    MOTOR_STATES state;
    pulse m1, m2, m3;
    int step_r, step_l;
    int num_r, num_l;
    int MAX, HOME;
    int count_r, count_l, count; // counts for right and left steps
}MOTOR;

pulse setupRotation(int dir, int step, int en, unsigned long tau, bool direction);
bool Rotation(pulse *state);

MOTOR setupMotor(int step_r, int step_l, int num_r, int num_l, int MAX, int HOME);
void MotorControl(MOTOR *motor);

MOTOR machine;

void setup() {
    machine = setupMotor(20, 200, 80, 200, 34, 35); 
    machine.m1 = setupRotation(17, 16, 15, 2, false);
    machine.m2 = setupRotation(19, 18, 5, 2, false);
    pinMode(2, OUTPUT);
    pinMode(0, OUTPUT);
    pinMode(4, OUTPUT);
    digitalWrite(2, HIGH);
    digitalWrite(0, HIGH);
    digitalWrite(4, LOW);
}

void loop() {
    MotorControl(&machine);
}

MOTOR setupMotor(int step_r, int step_l, int num_r, int num_l, int MAX, int HOME){
    MOTOR motor;
    motor.state = SCAN;
    motor.step_r = step_r;
    motor.step_l = step_l;
    motor.num_r = num_r;
    motor.num_l = num_l;
    motor.count_r = 0;
    motor.count_l = 0;
    motor.count = 0;
    motor.MAX = MAX;
    motor.HOME = HOME;
    pinMode(MAX, INPUT_PULLDOWN);
    pinMode(HOME, INPUT_PULLDOWN);
    return motor;
}

void MotorControl(MOTOR *motor){
    switch(motor->state) {
        case SCAN:
            delay(50);
            if(digitalRead(motor->MAX)== HIGH){
                digitalWrite(motor->m2.DIR, motor->m2.dir ? LOW : HIGH);
                motor->state = HOME; // Move to home position
                motor->count_r = 0; // Reset counts
                motor->count_l = 0;
                motor->count = 0;

            } else if(motor->count_r < motor->num_r) {
                motor->state = ROTATE;
                motor->count = 0;
                //digitalWrite(motor->m1.EN, LOW); 
            } else if(motor->count_l < motor->num_l) {
                motor->state = UP; 
                motor->count = 0; 
                //digitalWrite(motor->m2.EN, LOW); 
            } 
            break;
        case ROTATE:
            if(motor->count < motor->step_r){
                motor->state = ROTATE_ON; // Start rotating
                motor->m1.prev = millis();
            } else {
                motor->state = SCAN; // Reset to scan after rotation
                motor->count_r++; // Increment right count
                motor->count = 0; // Reset count
                //digitalWrite(motor->m1.EN, HIGH); // Disable motor
            }
            break;
        case ROTATE_ON:
            if(Rotation(&motor->m1)){
                motor->state = ROTATE;
                motor->count++;
            }
            break;
        case UP:
            if(motor->count < motor->step_l){
                motor->state = UP_ON; // Start rotating
                motor->m2.prev = millis();
            } else {
                motor->state = SCAN; // Reset to scan after rotation
                motor->count_l++; // Increment left count
                motor->count_r = 0; // Reset right count
                motor->count = 0; // Reset count
                //digitalWrite(motor->m2.EN, HIGH); // Disable motor
            }
            break;
        case UP_ON:
            if(Rotation(&motor->m2)){
                motor->state = UP;
                motor->count++;
            } 
            break;
        case HOME:
            if(digitalRead(motor->HOME) != HIGH){
                motor->state = HOME_ON; // Start rotating
                motor->m2.prev = millis();
            } else {
                motor->state = SCAN;
                digitalWrite(motor->m2.DIR, motor->m2.dir ? HIGH : LOW);
            }
            break;
        case HOME_ON:
            if(Rotation(&motor->m2)){
                motor->state = HOME;
            } 
            break;
    }
}

pulse setupRotation(int dir, int step, int en, unsigned long tau, bool direction){
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    pinMode(en, OUTPUT);
    digitalWrite(dir, direction ? HIGH : LOW);
    digitalWrite(en, LOW);
    pulse motor;
    motor.tau = tau;
    motor.prev = millis();
    motor.STEP = step;
    motor.EN = en;
    motor.DIR = dir;
    motor.dir = direction; // Set direction flag
    return motor;
}

bool Rotation(pulse *motor){
    if((millis()-motor->prev) < motor->tau / 2){
        digitalWrite(motor->STEP, HIGH);
    } else if((millis()-motor->prev) < motor->tau){
        digitalWrite(motor->STEP, LOW);
    } else {
        motor->prev += motor->tau;  // mejor que reiniciar con `millis()`
        return true;
    }
    return false;
}