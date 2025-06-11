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

#include <math.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


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
typedef struct 
{
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
#define NOTE_MI6   2638*2
#define NOTE_SOL6  3136*2
#define NOTE_LA6   3520*2
#define NOTE_LAS6  3730*2
#define NOTE_SI6   3952*2
#define NOTE_DO7   4186*2
#define NOTE_RE7   4698*2
#define NOTE_MI7   5274*2
#define NOTE_FA7   5588*2
#define NOTE_SOL7  6272*2
#define NOTE_LA7   7040*2
typedef enum { REPRODUCIENDO, FINALIZADO, ESPERANDO_REPETICION } Estado_Musica;
const int melodia[] = {
  NOTE_MI7, NOTE_MI7, 0, NOTE_MI7,
  0, NOTE_DO7, NOTE_MI7, 0,
  NOTE_SOL7, 0, 0,  0,
  NOTE_SOL6, 0, 0, 0,

  NOTE_DO7, 0, 0, NOTE_SOL6,
  0, 0, NOTE_MI6, 0,
  0, NOTE_LA6, 0, NOTE_SI6,
  0, NOTE_LAS6, NOTE_LA6, 0,

  NOTE_SOL6, NOTE_MI7, NOTE_SOL7,
  NOTE_LA7, 0, NOTE_FA7, NOTE_SOL7,
  0, NOTE_MI7, 0, NOTE_DO7,
  NOTE_RE7, NOTE_SI6, 0, 0,

  NOTE_DO7, 0, 0, NOTE_SOL6,
  0, 0, NOTE_MI6, 0,
  0, NOTE_LA6, 0, NOTE_SI6,
  0, NOTE_LAS6, NOTE_LA6, 0,

  NOTE_SOL6, NOTE_MI7, NOTE_SOL7,
  NOTE_LA7, 0, NOTE_FA7, NOTE_SOL7,
  0, NOTE_MI7, 0, NOTE_DO7,
  NOTE_RE7, NOTE_SI6, 0, 0
};
const int duraciones[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
typedef struct {
    Estado_Musica estadoActual;
    int buzzerPin;
    int totalNotas;
    unsigned long tiempoNotaAnterior;
    unsigned long tiempoFinalizado;
    int notaActual;
    bool notaEnCurso;
    unsigned long tiempoActual;
    const int* melodia;
    const int* duraciones;
} Musica;
Musica setupMusica(int pin);
void Musica_void(Musica *musica);
Musica musica;

#define OLED_MOSI   14
#define OLED_CLK    12
#define OLED_DC     26
#define OLED_RESET  27
#define OLED_CS     25

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Estados de la máquina
typedef enum{ INICIO, BIENVENIDA,ESCANEANDO,FINALIZA,CANCELADO } Estado_LCD;
typedef struct {
    Estado_LCD estadoActual = INICIO;

    int BOTON_PIN;

    unsigned long tiempoAnterior;
    unsigned long intervaloCarga;
    unsigned long tiempoInicio;
    unsigned long tiempoPresionado;
    unsigned long tiempoBotonAnterior;
    unsigned long tiempoCancelacion;
    unsigned long tiempoFinalizacion;

    bool esperandoAntiRebote;

    int carga;

    int numTriangulos;
    int numPuntos;
    float resolucion; 
} LCD;
bool pulsed = false;

LCD setupLCD(int nTri, int nPoints, int resol);
void LCD_void(LCD *lcd);
LCD lcd;

void mostrarPantallaInicio();
void mostrarCancelacion();
void mostrarPantallaBienvenida();
void dibujarEscaneo(LCD *lcd);
void actualizarCarga(LCD *lcd);
void mostrarResultados(LCD *lcd);


void setup() {
    Serial.begin(115200);
    machine = setupMotor(20, 200, 80, 200, 35, 32, 2, 15, 33); 
    machine.m1 = setupRotation(18, 5, 0, 2, false);
    machine.m2 = setupRotation(23, 22, 19, 2, false);
    machine.s = setupSensor(34, 4095, 10); // Sensor setup
    musica = setupMusica(21);
    LCD lcd = setupLCD(1245, 3250, 0.5 );
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();
  lcd.tiempoInicio = millis();
}

void loop() {
    MotorControl(&machine);
    Musica_void(&musica);
    LCD_void(&lcd);
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
                pulsed = true;
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
                pulsed = false;
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
                digitalWrite(motor->m2.DIR, motor->m2.dir ? LOW : HIGH);
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

            } else if(digitalRead(motor->INTERRUPTOR) == HIGH){
                motor->state = WELLCOME; 
                pulsed = true;
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
            if(motor->s.analog >= 3920){
                motor->s.distancia = 0;
            } else if(motor->s.analog >=1600){
                motor->s.distancia = (0.00000078*pow(motor->s.analog,2))-(0.0078*motor->s.analog)+25.9;
            } else{
                motor->s.distancia = 50;
            }

            Serial.println(motor->s.distancia);

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
void Musica_void(Musica *musica) {

  switch (musica->estadoActual) {
    case REPRODUCIENDO:
      if (musica->notaActual < musica->totalNotas) {
        if (!musica->notaEnCurso) {
          if (musica->melodia[musica->notaActual] != 0) {
            tone(musica->buzzerPin, musica->melodia[musica->notaActual]/2);
          } else {
            noTone(musica->buzzerPin);
          }
          musica->tiempoNotaAnterior = millis();
          musica->notaEnCurso = true;
        } else if (millis() - musica->tiempoNotaAnterior >= musica->duraciones[musica->notaActual]*9) {
          noTone(musica->buzzerPin);
          musica->notaActual++;
          musica->tiempoNotaAnterior = millis();
          musica->notaEnCurso = false;
        }
      } else {
        musica->estadoActual = FINALIZADO;
        musica->tiempoFinalizado = millis();
      }
      break;

    case FINALIZADO:
      musica->estadoActual = ESPERANDO_REPETICION;
      break;

    case ESPERANDO_REPETICION:
      if (millis() - musica->tiempoFinalizado >= 1000) {
        musica->estadoActual = REPRODUCIENDO;
        musica->notaActual = 0;
        musica->tiempoNotaAnterior = millis();
        musica->notaEnCurso = false;
      }
      break;
  }
}

Musica setupMusica(int pin){
    Musica m;
  	pinMode(pin, OUTPUT);
    m.estadoActual = REPRODUCIENDO;
    m.buzzerPin = pin;
    m.totalNotas = sizeof(melodia) / sizeof(melodia[0]);
    m.tiempoNotaAnterior = 0;
    m.tiempoFinalizado = 0;
    m.notaActual = 0;
    m.notaEnCurso = false;
    m.tiempoActual = 0;
    m.melodia = melodia;
    m.duraciones = duraciones;
    return m;
}

// LCD
LCD setupLCD(int nTri, int nPoints, int resol){
    LCD p;
    p.estadoActual = INICIO;

    p.tiempoAnterior = 0;
    p.intervaloCarga = 100;
    p.tiempoInicio = 0;
    p.tiempoPresionado = 0;
    p.tiempoBotonAnterior = 0;
    p.tiempoCancelacion = 0;
    p.tiempoFinalizacion = 0;

    p.esperandoAntiRebote = false;

    // Variables de escaneo
    p.carga = 0;

    // Resultados simulados
    p.numTriangulos = nTri;
    p.numPuntos = nPoints;
    p.resolucion = resol;

    return p;
}

void LCD_void(LCD *lcd) {
  switch (lcd->estadoActual) {
    case INICIO:
      if (millis() - lcd->tiempoInicio < 3000) {
        lcd->carga = 0;
        mostrarPantallaInicio();  
      } else {
        lcd->estadoActual = BIENVENIDA;
      }

      break;

    case BIENVENIDA:
     mostrarPantallaBienvenida();
      if (pulsed && !lcd->esperandoAntiRebote) {
        lcd->esperandoAntiRebote = true;
        lcd->tiempoBotonAnterior = millis();
      }

      if (lcd->esperandoAntiRebote && (millis() - lcd->tiempoBotonAnterior >= 200)) {
        lcd->esperandoAntiRebote = false;
        lcd->carga = 0;
        lcd->tiempoAnterior = millis();
        lcd->estadoActual = ESCANEANDO;
      }
      break;

    case ESCANEANDO:
      if (pulsed) {
        if (lcd->tiempoPresionado == 0) {
          lcd->tiempoPresionado = millis();
        } else if (millis() - lcd->tiempoPresionado >= 2000) {
          mostrarCancelacion();
          lcd->tiempoCancelacion = millis();
          lcd->estadoActual = CANCELADO;
        }
      } else {
        lcd->tiempoPresionado = 0;
      }

      actualizarCarga(lcd);
      dibujarEscaneo(lcd);

      if (lcd->carga >= 100.0) {
        mostrarResultados(lcd);
        lcd->tiempoFinalizacion = millis();
        lcd->estadoActual = FINALIZA;
      }
      break;

   case CANCELADO:
      if (millis() - lcd->tiempoCancelacion >= 2000) {
        mostrarPantallaBienvenida();
        lcd->estadoActual = BIENVENIDA;
      } else {
        mostrarCancelacion();
      }
      break;

    case FINALIZA:
      if (pulsed && !lcd->esperandoAntiRebote) {
        lcd->esperandoAntiRebote = true;
        lcd->tiempoBotonAnterior = millis();
      }

      if (lcd->esperandoAntiRebote && (millis() - lcd->tiempoBotonAnterior >= 200)) {
        lcd->esperandoAntiRebote = false;
        mostrarPantallaBienvenida();
        lcd->estadoActual = BIENVENIDA;
      }
      break;
  }
}

void mostrarPantallaInicio() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 10);
  display.println("ESCANER 3D");
  display.setCursor(0, 20);
  display.println("By Equipo Dinamita");
  display.display();
}

void mostrarCancelacion() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 10);
  display.println("SE CANCELO");
  display.setCursor(30, 20);
  display.println("PROCESO");
  display.display();
}

void mostrarPantallaBienvenida() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 0);
  display.println("BIENVENIDO");
  display.setCursor(0, 15);
  display.println("Press RED button");
  display.setCursor(0, 45);
  display.println("para iniciar el escaneo");
  display.display();
}

void actualizarCarga(LCD *lcd) {
  if (millis() - lcd->tiempoAnterior >= lcd->intervaloCarga) {
    lcd->tiempoAnterior = millis();
    lcd->carga += 1.0;
    if (lcd->carga > 100.0) {
      lcd->carga = 100.0;
    }
  }
}

void dibujarEscaneo(LCD *lcd) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Escaneando: ");
  display.print(lcd->carga, 1);
  display.print("%");
  display.drawRect(0, 10, 128, 10, WHITE);
  display.fillRect(0, 10, (128 * lcd->carga / 100.0), 10, WHITE);
   display.setCursor(0, 23);
  display.setTextSize(1);
  display.print("Scan Dist:");
  display.setCursor(0, 48);
 display.print(machine.s.distancia, 2);
 display.print(" cm");
  display.display();
}

void mostrarResultados(LCD *lcd) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SCAN COMPLETE");

  display.setCursor(0, 8);
  display.print("Triangulos: ");
  display.println(lcd->numTriangulos);

  display.setCursor(0, 16);
  display.print("Puntos: ");
  display.println(lcd->numPuntos);

  display.setCursor(0, 24);
  display.print("Resolucion: ");
  display.print(lcd->resolucion, 1);
  display.println(" mm");

  display.display();
}