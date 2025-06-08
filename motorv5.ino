// motorv5.ino
// Control de motores paso y buzzer
// Autor: Juan Pablo Arenas
//Motor1: Pin 17 (DIR), Pin 16 (STEP), Pin 15 (EN)
//Motor2: Pin 19 (DIR), Pin 18 (STEP), Pin 5 (EN)
//Buzzer: Pin 21
//PULSO
typedef struct{
    unsigned long prev;
    unsigned long tau;
    int STEP, EN, DIR;
    bool dir; // direction flag
  // true = forward, false = backward
}pulse;

pulse setupRotation(int dir, int step, int en, unsigned long tau, bool direction);
bool Rotation(pulse *state);

//MOTOR
typedef enum{SCAN, ROTATE, ROTATE_ON, UP, UP_ON, HOME, HOME_ON} MOTOR_STATES;
typedef struct{
    MOTOR_STATES state;
    pulse m1, m2, m3;
    int step_r, step_l;
    int num_r, num_l;
    int MAX, HOME;
    int count_r, count_l, count; // counts for right and left steps
}MOTOR;

MOTOR setupMotor(int step_r, int step_l, int num_r, int num_l, int MAX, int HOME);
void MotorControl(MOTOR *motor);

MOTOR machine;

//MUSICA
typedef enum{ REPRODUCIENDO, FINALIZADO, ESPERANDO_REPETICION } Estado_Musica;
typedef struct {
    Estado_Musica estadoActual;
    int buzzerPin;
    int totalNotas;
    unsigned long tiempoNotaAnterior;
    unsigned long tiempoFinalizado;
    int notaActual;
    bool notaEnCurso;
    unsigned long tiempoActual;
    int melodia[84] = {
      660, 660, 0, 660, 0, 510, 660, 0, 770, 0, 380, 0,
      510, 0, 380, 0, 320, 0, 440, 0, 480, 0, 450, 430,
      0, 380, 660, 0, 760, 0, 860, 0, 700, 770,
      0, 660, 0, 520, 580, 480, 0, 510, 0, 380, 0, 320, 0,
      440, 0, 480, 0, 450, 430, 0, 380, 660, 0,
      760, 0, 860, 0, 700, 770, 0, 660, 0, 520,
      580, 0, 660, 0, 870, 0, 760, 0, 660, 0,
      520, 580, 0, 480
    };
    int duraciones[84] = {
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150, 150, 150, 225, 225,
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 225, 225, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150
    };
} Musica;

Musica setupMusica(int pin);
void Musica_void(Musica *musica);
Musica musica;


void setup() {
    machine = setupMotor(20, 200, 80, 200, 34, 35); 
    machine.m1 = setupRotation(17, 16, 15, 2, false);
    machine.m2 = setupRotation(19, 18, 5, 2, false);
    
    musica = setupMusica(21);
    pinMode(musica.buzzerPin, OUTPUT);
}

void loop() {
    MotorControl(&machine);
    Musica_void(&musica);
}

MOTOR setupMotor(int step_r, int step_l, int num_r, int num_l, int MAX, int HOME){
    pinMode(2, OUTPUT);
    pinMode(0, OUTPUT);
    pinMode(4, OUTPUT);
    digitalWrite(2, HIGH);
    digitalWrite(0, HIGH);
    digitalWrite(4, LOW);
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
            //delay(50);
            if(digitalRead(motor->MAX)== HIGH){
                digitalWrite(motor->m2.DIR, motor->m2.dir ? LOW : HIGH);
                motor->state = HOME; // Move to home position
                motor->count_r = 0; // Reset counts
                motor->count_l = 0;
                motor->count = 0;

            } else if(motor->count_r < motor->num_r) {
                motor->state = ROTATE;
                motor->count = 0;
            } else if(motor->count_l < motor->num_l) {
                motor->state = UP; 
                motor->count = 0; 
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

//MUSICA
void Musica_void(Musica *musica){
  musica->tiempoActual = millis();

  switch (musica->estadoActual) {

    case REPRODUCIENDO:
      if (musica->notaActual < musica->totalNotas) {
        if (!musica->notaEnCurso) {
          if (musica->melodia[musica->notaActual] != 0) {
            tone(musica->buzzerPin, musica->melodia[musica->notaActual]*8);
          } else {
            noTone(musica->buzzerPin);
          }
          musica->tiempoNotaAnterior = musica->tiempoActual;
          musica->notaEnCurso = true;
        } else if (musica->tiempoActual - musica->tiempoNotaAnterior >= musica->duraciones[musica->notaActual]) {
          noTone(musica->buzzerPin);
          musica->notaActual++;
          musica->tiempoNotaAnterior = musica->tiempoActual;
          musica->notaEnCurso = false;
        }
      } else {
        musica->estadoActual = FINALIZADO;
        musica->tiempoFinalizado = musica->tiempoActual;
      }
      break;

    case FINALIZADO:
      musica->estadoActual = ESPERANDO_REPETICION;
      break;

    case ESPERANDO_REPETICION:
      if (musica->tiempoActual - musica->tiempoFinalizado >= 2000) {
        musica->estadoActual = REPRODUCIENDO;
        musica->notaActual = 0;
        musica->tiempoNotaAnterior = musica->tiempoActual;
        musica->notaEnCurso = false;
      }
      break;
  }
}

Musica setupMusica(int pin){
    Musica m;
    m.estadoActual = REPRODUCIENDO;
    m.buzzerPin = pin;    
    m.tiempoNotaAnterior = 0;
    m.tiempoFinalizado = 0;
    m.notaActual = 0;
    m.notaEnCurso = false; 
    m.tiempoActual = 0;
    m.totalNotas = sizeof(m.melodia) / sizeof(m.melodia[0]);
    return m;
}