#include <math.h>

#define SKY 1e2;
#define ancho 80

typedef enum{ INICIO, MIDIENDO, ESPERANDO, PROCESANDO, GIRANDO, SUBIENDO} Estado_Sensor;
typedef enum{ REPRODUCIENDO, FINALIZADO, ESPERANDO_REPETICION } Estado_Musica;

typedef struct {
    Estado_Sensor estadoActual;
    int buzzerPin;
    int buzzerBotton;
    double Bits;
    int noSamples;
    double sumOfSamples;
    int analog;
    double Voltaje;
    float distancia;
    int contador;
    int i,k;
    double timer;
} Sensor;

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

//bool readSensor(sensor *s);
Sensor setupSensor(int pin, int botton, double bits, int noSamples){
    Sensor s;
    s.estadoActual = MIDIENDO;
    s.buzzerPin = pin;
    s.buzzerBotton = botton;
    s.Bits = bits;
    s.noSamples = noSamples;
    s.sumOfSamples = 0;
    s.distancia = 0;
    s.contador = 0;
    s.i = 0;
    s.k = 0 ;
    return s;
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

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);
int SensorLeyendo();
double LaserDistancia();
int ScannerFinalizado();
void Sensor_void();
Sensor sensor;

void Musica_void();
Musica musica;

void setup() {
  Serial.begin(9600); // Inicia la comunicación serial a 9600 bps
  sensor = setupSensor(5, 4, 1023, 100);
  musica = setupMusica(6);

  pinMode(sensor.buzzerPin, INPUT_PULLDOWN);
  pinMode(sensor.buzzerBotton, INPUT_PULLDOWN);
  pinMode(musica.buzzerPin, OUTPUT);
}

void loop() {
  Sensor_void();
  Musica_void();
}

void Sensor_void() {
  switch(sensor.estadoActual){

      case INICIO:

      break;

      case MIDIENDO:
        if(sensor.k < sensor.noSamples){
          sensor.sumOfSamples += analogRead(A0); // Suma acumulada de las lecturas
          sensor.k++;
          sensor.estadoActual = ESPERANDO;
          sensor.timer = millis();
        } else{
          sensor.k = 0;
          sensor.estadoActual = PROCESANDO;
        }
      break;

      case ESPERANDO:
        if( millis() - sensor.timer >= 2 ){
          sensor.estadoActual = MIDIENDO;
        }
      break;

      case PROCESANDO:
        sensor.analog = sensor.sumOfSamples / sensor.noSamples; // Calcula el promedio
        sensor.Voltaje = mapDouble((double)sensor.analog, 0.0, 1023.0, 0.0, 5.0); // Convierte a voltaje

        if (sensor.Voltaje >= 2.9) {  // 6
          sensor.distancia = 40.82545 * exp(-0.65439 * sensor.Voltaje) + 0.3;  // Modelo 5
        }
        else if (sensor.Voltaje >= 2.7) {  // 7
          sensor.distancia = 21.83057 * pow(sensor.Voltaje, -1.03006) - 0.50;  // Modelo 4
        }
        else if (sensor.Voltaje >= 2.4) {  // 8
          sensor.distancia = 21.83057 * pow(sensor.Voltaje, -1.03006) - 0.34;  // Modelo 4
        }
        else if (sensor.Voltaje >= 2.2) {  // 9
          sensor.distancia = 40.82545 * exp(-0.65439 * sensor.Voltaje) + 0.06;  // Modelo 5
        }
        else if (sensor.Voltaje >= 2.0) {  // 10
          sensor.distancia = 21.83057 * pow(sensor.Voltaje, -1.03006) - 0.09;   // Modelo 4
        }
        else if (sensor.Voltaje >= 1.8) {  // 11
          sensor.distancia = 21.12816 * pow(sensor.Voltaje, -0.98062) - 0.17 ;   // Modelo 3
        }
        else if (sensor.Voltaje >= 1.72) {  // 12
          sensor.distancia = 21.12816 * pow(sensor.Voltaje, -0.98062) - 0.10;    // Modelo 3
        }
        else if (sensor.Voltaje >= 1.59) {  // 13
          sensor.distancia = 21.12816 * pow(sensor.Voltaje, -0.98062) - 0.20;  // Modelo 3
        }
        else if (sensor.Voltaje >= 1.49) {  // 14
          sensor.distancia = 21.12816 * pow(sensor.Voltaje, -0.98062);  // Modelo 3
        }
        else if (sensor.Voltaje >= 1.39) {  // 15
          sensor.distancia = 21.12816 * pow(sensor.Voltaje, -0.98062) + 0.12; // Modelo 3
        }
        else if (sensor.Voltaje >= 1.29) {  // 16
          sensor.distancia = 20.4026 * pow(sensor.Voltaje, -0.90119) - 0.11;  // Modelo 2
        }
        else if (sensor.Voltaje >= 1.20) {  // 17
          sensor.distancia = 20.4026 * pow(sensor.Voltaje, -0.90119) ;  // Modelo 2
        }
        else if (sensor.Voltaje >= 1.09) {  // 18 y 19
          sensor.distancia = 21.12816 * pow(sensor.Voltaje, -0.98062) + 0.15;  // Modelo 3
        }
        else {  // 20++
          sensor.distancia = 21.12816 * pow(sensor.Voltaje, -0.98062);  // Modelo 3
        }

        //float Distance1 = 41.7847 * exp(-0.68313 * Voltaje);
        //float Distance2 = 20.4026 * pow(Voltaje, -0.90119);
        //float Distance3 = 21.12816 * pow(Voltaje, -0.98062);
        //float Distance4 = 21.83057 * pow(Voltaje, -1.03006);
        //float Distance5 = 40.82545 * exp(-0.65439 * Voltaje);

        Serial.print("Voltaje: ");
        Serial.print(sensor.Voltaje);
        Serial.print(" V | ");
        Serial.print("Distancia: ");
        Serial.print(sensor.distancia);
        Serial.println(" cm");

        if(sensor.distancia > 30){
          sensor.distancia = SKY;
          sensor.contador++;
        } else{
          sensor.contador = 0;
        }

        sensor.estadoActual = GIRANDO;
        sensor.timer = millis();
      break;

      case GIRANDO:
        if( millis() - sensor.timer >= 100 ){
          sensor.sumOfSamples = 0;
          Serial.println(sensor.i);
          sensor.i++;
          sensor.estadoActual = SUBIENDO;
        }
      break;

      case SUBIENDO:
        sensor.sumOfSamples = 0;
        sensor.estadoActual = MIDIENDO;
        if( sensor.i == 80 ){
          sensor.i = 0;
          Serial.println();
        }
        if(sensor.contador == 240)
            sensor.estadoActual = INICIO;
      break;
  }
}

void Musica_void() {
  musica.tiempoActual = millis();

  switch (musica.estadoActual) {

    case REPRODUCIENDO:
      if (musica.notaActual < musica.totalNotas) {
        if (!musica.notaEnCurso) {
          if (musica.melodia[musica.notaActual] != 0) {
            tone(musica.buzzerPin, musica.melodia[musica.notaActual]*8);
          } else {
            noTone(musica.buzzerPin);
          }
          musica.tiempoNotaAnterior = musica.tiempoActual;
          musica.notaEnCurso = true;
        } else if (musica.tiempoActual - musica.tiempoNotaAnterior >= musica.duraciones[musica.notaActual]) {
          noTone(musica.buzzerPin);
          musica.notaActual++;
          musica.tiempoNotaAnterior = musica.tiempoActual;
          musica.notaEnCurso = false;
        }
      } else {
        musica.estadoActual = FINALIZADO;
        musica.tiempoFinalizado = musica.tiempoActual;
      }
      break;

    case FINALIZADO:
      musica.estadoActual = ESPERANDO_REPETICION;
      break;

    case ESPERANDO_REPETICION:
      if (musica.tiempoActual - musica.tiempoFinalizado >= 2000) {
        musica.estadoActual = REPRODUCIENDO;
        musica.notaActual = 0;
        musica.tiempoNotaAnterior = musica.tiempoActual;
        musica.notaEnCurso = false;
      }
      break;
  }
}

// Definición de la función mapDouble
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int SensorLeyendo(){
  if(sensor.estadoActual == PROCESANDO)
   return 1;
  return 0;
}

double LaserDistancia(){
  if(sensor.estadoActual == GIRANDO)
   return sensor.distancia;
  return 6969;
}