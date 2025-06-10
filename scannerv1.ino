// motorv5.ino
// Control de motores paso, scanner y buzzer
// Autor: Juan Pablo Arenas a partir de sensorv4.ino de Helver Gamboa
//Motor1: Pin 23 (DIR), Pin 22 (STEP), Pin 19 (EN)
//Motor2: Pin 18 (DIR), Pin 5 (STEP), Pin 0 (EN)
//Buzzer: Pin 21
//MAX: Pin 35
//MIN: Pin 32
//Boton1: Pin 2
//Boton2: Pin 15
//Interruptor: Pin 33

#include <BluetoothSerial.h>

BluetoothSerial BT;

#define SKY 100.0 // Maximum distance threshold (cm) for sensor out-of-range detection
#define ancho 80

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

//SCANNER
typedef struct {
    int sensorPin;
    double Bits;
    int noSamples;
    double sumOfSamples;
    int analog;
    double Voltaje;
    float distancia;
    int contador;
    int k;
    double timer;
    bool scanning; // true if scanning
} Sensor;
Sensor setupSensor(int pin, double bits, int noSamples);
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

//MOTOR
typedef enum{WELLCOME, SCAN, ROTATE, ROTATE_ON, UP, UP_ON, HOME, HOME_ON, SUBIR, SUBIR_ON, BAJAR, BAJAR_ON,
MIDIENDO, ESPERANDO, PROCESANDO} MOTOR_STATES;
typedef struct{
    MOTOR_STATES state;
    pulse m1, m2;
    int step_r, step_l;
    int num_r, num_l;
    int MAX, HOME;
    int B1, B2;
    int INTERRUPTOR;
    int count_r, count_l, count; // counts for right and left steps
    Sensor s; // Integrate sensor into motor structure
}MOTOR;
MOTOR setupMotor(int step_r, int step_l, int num_r, int num_l, int MAX, int HOME, int B1, int B2, int I);
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
    Serial.begin(115200);
    BT.begin("JP"); // Bluetooth device name
    Serial.println("Bluetooth started, waiting for connection...");
    machine = setupMotor(40, 400, 40, 200, 35, 32, 2, 15, 33); 
    machine.m1 = setupRotation(18, 5, 0, 2, false);
    machine.m2 = setupRotation(23, 22, 19, 2, false);
    machine.s = setupSensor(34, 4095, 50); // Sensor setup
    
    musica = setupMusica(21);
}

void loop() {
    MotorControl(&machine);
    Musica_void(&musica);
}

MOTOR setupMotor(int step_r, int step_l, int num_r, int num_l, int MAX, int HOME, int B1, int B2, int I){
    pinMode(4, OUTPUT);//MS1
    pinMode(16, OUTPUT);//MS2
    pinMode(17, OUTPUT);//MS3
    digitalWrite(4, HIGH);
    digitalWrite(16, HIGH);
    digitalWrite(17, LOW);
    // 1 = 1/8 microstepping
    MOTOR motor;
    motor.state = WELLCOME;
    motor.step_r = step_r;
    motor.step_l = step_l;
    motor.num_r = num_r;
    motor.num_l = num_l;
    motor.count_r = 0;
    motor.count_l = 0;
    motor.count = 0;
    motor.MAX = MAX;
    motor.HOME = HOME;
    motor.B1 = B1;
    motor.B2 = B2;
    motor.INTERRUPTOR = I;
    pinMode(I, INPUT_PULLDOWN);
    pinMode(B1, INPUT_PULLDOWN);
    pinMode(B2, INPUT_PULLDOWN);
    pinMode(MAX, INPUT_PULLDOWN);
    pinMode(HOME, INPUT_PULLDOWN);
    return motor;
}

void MotorControl(MOTOR *motor){
    switch(motor->state) {
        case WELLCOME:
            if(digitalRead(motor->INTERRUPTOR) == HIGH){
                motor->state = SCAN; // Transition to SCAN state
                motor->s.scanning = true; // Enable scanning
            } else if(digitalRead(motor->B1) == HIGH){
                motor->state = SUBIR; // Transition to UP state
            } else if (digitalRead(motor->B2) == HIGH){
                motor->state = BAJAR; // Transition to DOWN state
                digitalWrite(motor->m2.DIR, motor->m2.dir ? LOW : HIGH);
            } 
            break;
        case SCAN:
            if(motor->s.scanning) {
                motor->state = ESPERANDO;
                motor->s.timer = millis();
                motor->s.scanning = false; 
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
                motor->s.sumOfSamples = 0; // Reset sensor sum for next scan
                motor->s.scanning = true; // Re-enable scanning for next cycle
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
            } else if(motor->s.contador <= 240) {
                motor->state = SCAN; // Reset to scan after rotation
                motor->count_l++; // Increment left count
                motor->count_r = 0; // Reset right count
                motor->count = 0; // Reset count
                motor->s.sumOfSamples = 0;
                motor->s.scanning = true;// Re-enable scanning for next cycle
            } else{
                motor->state = HOME; // Reset to scan after rotation
                motor->count_r = 0; // Reset right count
                motor->count_l = 0; // Reset left count
                motor->count = 0; // Reset count
                motor->s.sumOfSamples = 0;
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
                motor->s.scanning = true; 
            }
            break;
        case HOME_ON:
            if(Rotation(&motor->m2)){
                motor->state = HOME;
            } 
            break;
        case ESPERANDO:
            if(digitalRead(motor->MAX)== HIGH){
                digitalWrite(motor->m2.DIR, motor->m2.dir ? LOW : HIGH);
                motor->state = HOME; // Move to home position
                motor->count_r = 0; // Reset counts
                motor->count_l = 0;
                motor->count = 0;
                BT.print("Baja en esperando");

            } else if(digitalRead(motor->INTERRUPTOR) == HIGH){
                motor->state = WELLCOME; 
                motor->count_r = 0;
                motor->count_l = 0;
                motor->count = 0;
            } else if( millis() - motor->s.timer > 50 ){
                motor->state = MIDIENDO;
            }
            break;
        case MIDIENDO:
            if(motor->s.k < motor->s.noSamples){
                int reading = analogRead(motor->s.sensorPin);
                if (reading >= 0 && reading <= motor->s.Bits) { // Validate reading
                    motor->s.sumOfSamples += reading;
                    motor->s.k++;
                }
                motor->state = ESPERANDO;
                motor->s.timer = millis();
            } else{
                motor->s.k = 0;
                motor->state = PROCESANDO;
            }
            break;
        case PROCESANDO:
            motor->s.analog = motor->s.sumOfSamples / motor->s.noSamples; // Calcula el promedio
            motor->s.Voltaje = mapDouble((double)motor->s.analog, 0.0, motor->s.Bits, 0.0, 5.0); // Convierte a voltaje

            if (motor->s.Voltaje >= 2.9) {  // 6
                motor->s.distancia = 40.82545 * exp(-0.65439 * motor->s.Voltaje) + 0.3;  // Modelo 5
            } else if (motor->s.Voltaje >= 2.7) {  // 7
                motor->s.distancia = 21.83057 * pow(motor->s.Voltaje, -1.03006) - 0.50;  // Modelo 4
            } else if (motor->s.Voltaje >= 2.4) {  // 8
                motor->s.distancia = 21.83057 * pow(motor->s.Voltaje, -1.03006) - 0.34;  // Modelo 4
            } else if (motor->s.Voltaje >= 2.2) {  // 9
                motor->s.distancia = 40.82545 * exp(-0.65439 * motor->s.Voltaje) + 0.06;  // Modelo 5
            } else if (motor->s.Voltaje >= 2.0) {  // 10
                motor->s.distancia = 21.83057 * pow(motor->s.Voltaje, -1.03006) - 0.09;   // Modelo 4
            } else if (motor->s.Voltaje >= 1.8) {  // 11
                motor->s.distancia = 21.12816 * pow(motor->s.Voltaje, -0.98062) - 0.17 ;   // Modelo 3
            } else if (motor->s.Voltaje >= 1.72) {  // 12
                motor->s.distancia = 21.12816 * pow(motor->s.Voltaje, -0.98062) - 0.10;    // Modelo 3
            } else if (motor->s.Voltaje >= 1.59) {  // 13
                motor->s.distancia = 21.12816 * pow(motor->s.Voltaje, -0.98062) - 0.20;  // Modelo 3
            } else if (motor->s.Voltaje >= 1.49) {  // 14
                motor->s.distancia = 21.12816 * pow(motor->s.Voltaje, -0.98062);  // Modelo 3
            } else if (motor->s.Voltaje >= 1.39) {  // 15
                motor->s.distancia = 21.12816 * pow(motor->s.Voltaje, -0.98062) + 0.12; // Modelo 3
            } else if (motor->s.Voltaje >= 1.29) {  // 16
                motor->s.distancia = 20.4026 * pow(motor->s.Voltaje, -0.90119) - 0.11;  // Modelo 2
            } else if (motor->s.Voltaje >= 1.20) {  // 17
                motor->s.distancia = 20.4026 * pow(motor->s.Voltaje, -0.90119) ;  // Modelo 2
            } else if (motor->s.Voltaje >= 1.09) {  // 18 y 19
                motor->s.distancia = 21.12816 * pow(motor->s.Voltaje, -0.98062) + 0.15;  // Modelo 3
            } else {  // 20++
                motor->s.distancia = 21.12816 * pow(motor->s.Voltaje, -0.98062);  // Modelo 3
            }

            BT.print("Voltaje: ");
            BT.print(motor->s.Voltaje);
            BT.print(" V | ");
            BT.print("Distancia: ");
            BT.print(motor->s.distancia);
            BT.println(" cm");

            if(motor->s.distancia > 30){
                motor->s.distancia = SKY;
                motor->s.contador++;
            } else{
                motor->s.contador = 0;
            }
            motor->state = SCAN;
            motor->s.timer = millis();
            break;
        case SUBIR:
            if(digitalRead(motor->B1) == HIGH){
                motor->state = SUBIR_ON; // Start rotating
                motor->m2.prev = millis();
            } else {
                motor->state = WELLCOME;
                motor->s.scanning = true; 
            }
            break;
        case SUBIR_ON:
            if(Rotation(&motor->m2)){
                motor->state = SUBIR;
            } 
            break;
        case BAJAR:
            if(digitalRead(motor->B2) == HIGH){
                motor->state = BAJAR_ON; // Start rotating
                motor->m2.prev = millis();
            } else {
                motor->state = WELLCOME;
                digitalWrite(motor->m2.DIR, motor->m2.dir ? HIGH : LOW);
            }
            break;
        case BAJAR_ON:
            if(Rotation(&motor->m2)){
                motor->state = BAJAR;
            } 
            break;
    }
}

//PWM
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
        //motor->prev += motor->tau;  // mejor que reiniciar con `millis()`
        return true;
    }
    return false;
}

//SENSOR
Sensor setupSensor(int pin, double bits, int noSamples) {
    pinMode(pin, INPUT);
    Sensor s;
    s.sensorPin = pin;
    s.Bits = bits;
    s.noSamples = noSamples;
    s.sumOfSamples = 0;
    s.distancia = 0;
    s.contador = 0;
    s.timer = 0;
    s.k = 0;
    s.scanning = true; // Initialize scanning state
    return s;
}

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
    pinMode(pin, OUTPUT);
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