typedef struct{
    unsigned long prev;
    unsigned long tau;
    int STEP, EN, DIR;
}pulse;

pulse setupRotation(int dir, int step, int en, unsigned long tau, bool direction);
bool Rotation(pulse *state);

void setup() {
    m1 = setupRotation(4, 5, 23, 5, true);
    m2 = setupRotation(15, 2, 22, 5, false);
}

bool a = false, b = false;

void loop() {
    if(millis() % 1000 < 500 && !a) {
        a = Rotation(&m1);
    } else if(millis() % 1000 >= 500) {
        a = false; // Reinicia bandera cuando salimos del ciclo
    }

    if(millis() % 1000 >= 500 && !b) {
        b = Rotation(&m2);
    } else if(millis() % 1000 < 500) {
        b = false; // Reinicia bandera cuando salimos del ciclo
    }
}


pulse setupRotation(int dir, int step, int en, unsigned long tau, bool direction){
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    digitalWrite(dir, direction ? HIGH : LOW);
    digitalWrite(en, HIGH);
    pulse motor;
    motor.tau = tau;
    motor.prev = millis();
    motor.STEP = step;
    motor.EN = en;
    motor.DIR = dir;
    return motor;
}

bool Rotation(pulse *motor){
    digitalWrite(motor->EN, LOW); // Enable the motor
    if((millis()-motor->prev) < motor->tau / 2){
        digitalWrite(motor->STEP, HIGH);
    } else if((millis()-motor->prev) < motor->tau){
        digitalWrite(motor->STEP, LOW);
    } else {
        digitalWrite(motor->EN, HIGH); // Disable the motor
        motor->prev += motor->tau;  // mejor que reiniciar con `millis()`
        return true;
    }
    digitalWrite(motor->EN, HIGH); // Disable the motor
    return false;
}