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
    const int prev;
    const int tau;
    const int STEP;
}pulse;

pulse setupRotation(int dir, int step, int tau);
void Rotation(pulse *state);

pulse m1, m2;

void setup() {
    m1 = setupRotation(33,25,1000);
    m2 = setupRotation(27,14,1000);
}

void loop() {
    Rotation(m1);
    Rotation(m2);
}

pulse setupRotation(int dir, int step, int tau, bool direction){
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    if(direction)digitalWrite(DIR_PIN, HIGH);
    else digitalWrite(DIR_PIN, LOW);
    pulse motor;
    motor.tau = tau;
    motor.prev = micros();
    motor.STEP = step;
    motor.state = ON;
    digitalWrite(step,HIGH);
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
