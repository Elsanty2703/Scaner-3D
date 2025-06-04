/*// Pines conectados al motor 1
const int EN_PIN1   = 32; // ENABLE
const int STEP_PIN1 = 33; // STEP
const int DIR_PIN1  = 25; // DIR

// Pines conectados al motor 2
const int EN_PIN2   = 26; // ENABLE
const int STEP_PIN2 = 27; // STEP
const int DIR_PIN2  = 14; // DIR*/

typedef enum{ON, OFF}MOTOR_STATES;
typedef struct{
    MOTOR_STATES state;
    const int prev;
    const int tau;
    const int STEP;
}motor;

MOTOR_STATES setupRotation(int EN, int dir, int step, int tau);
void Rotation(MOTOR_STATES *state);

MOTOR_STATES m1, m2;

void setup() {
    m1 = setupRotation(32,33,25,1000);
    m2 = setupRotation(26,27,14,1000);
}

void loop() {
    Rotation(m1);
    Rotation(m2);
}

MOTOR_STATES setupRotation(int en, int dir, int step, int tau){
  pinMode(en, OUTPUT);
  pinMode(step, OUTPUT);
  pinMode(dir, OUTPUT);
  digitalWrite(DIR_PIN, HIGH);
  MOTOR_STATES state;
  state.tau = tau;
  state.prev = micros();
  state.STEP = step;
  digitalWrite(step,HIGH);
}

void Rotation(MOTOR_STATES *motor){

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
