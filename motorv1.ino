/*// Pines conectados al motor 1
const int EN_PIN1   = 32; // ENABLE
const int STEP_PIN1 = 33; // STEP
const int DIR_PIN1  = 25; // DIR

// Pines conectados al motor 2
const int EN_PIN2   = 26; // ENABLE
const int STEP_PIN2 = 27; // STEP
const int DIR_PIN2  = 14; // DIR*/

typedef enum{ON, OFF}PULSE_STATES;
typedef struct{
    PULSE_STATES state;
    int prev;
    int tau;
    int STEP;
}pulse;

pulse setupRotation(int dir, int step, int tau, bool direction);
void Rotation(pulse *state);

pulse m1, m2;

void setup() {
    m1 = setupRotation(33,25,1000, true);
    m2 = setupRotation(27,14,1000, false);
}

void loop() {
    Rotation(&m1);
    Rotation(&m2);
}

pulse setupRotation(int dir, int step, int tau, bool direction){
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    if(direction)digitalWrite(dir, HIGH);
    else digitalWrite(dir, LOW);
    pulse motor;
    motor.tau = tau;
    motor.prev = micros();
    motor.STEP = step;
    motor.state = ON;
    digitalWrite(step,HIGH);
    return motor;
}

void Rotation(pulse *motor){

    switch(motor->state){
        case ON:
            if((micros()-motor->prev)>motor->tau/2){
                motor->state = OFF;
                motor->prev = micros();
                digitalWrite(motor->STEP,LOW);
            }
            break;
        case OFF:
            if((micros()-motor->prev)>motor->tau/2){
                motor->state = ON;
                motor->prev = micros();
                digitalWrite(motor->STEP,HIGH);
            }
            break;
        
    }
}
// This code is for controlling two stepper motors using pulse width modulation.
// It sets up the motors with specified parameters and alternates their states
// to create rotation. The motors are controlled by setting the direction and
// generating pulses at specified intervals. The code uses a simple state machine