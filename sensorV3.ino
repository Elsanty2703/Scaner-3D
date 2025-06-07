#include <math.h>

typedef enum{ Midiendo, Esperando, Procesando, Girando, Subiendo} Estado_Sensor;
typedef enum{ REPRODUCIENDO, FINALIZADO, ESPERANDO_REPETICION } Estado_Musica;

typedef struct {
    Estado_Sensor estadoActual;
    int buzzerPin;
    int noSamples;
    int senseValue;
    double sumOfSamples;
    int analog;
    double Voltaje;
    float distancia;
    int chipSelect;
    int i,j,k;
    double timer;
    //int Datos[200][80];
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
    int melodia[84];
    int duraciones[84];
} Musica;

//bool readSensor(sensor *s);
Sensor setupSensor(){
    Sensor s;
    s.estadoActual = Midiendo;
    s.buzzerPin = 27;
    s.noSamples = 100;
    s.senseValue = 0;
    s.sumOfSamples = 0;
    s.analog = 0;
    s.Voltaje = 0;
    s.distancia = 0;
    s.chipSelect = 10;
    s.i = 199 ;
    s.j = 0 ;
    s.k = 0 ;
    return s;
}
Musica setupMusica(){
    Musica m;
    m.estadoActual = REPRODUCIENDO;
    m.buzzerPin = 35;
    m.totalNotas = sizeof(m.melodia) / sizeof(m.melodia[0]);
    m.tiempoNotaAnterior = 0;
    m.tiempoFinalizado = 0;
    m.notaActual = 0;
    m.notaEnCurso = false;
    m.tiempoActual = 0;
    return m;
}

int Melodia[84] = {
      660, 660, 0, 660, 0, 510, 660, 0, 770, 0, 380, 0,
      510, 0, 380, 0, 320, 0, 440, 0, 480, 0, 450, 430,
      0, 380, 660, 0, 760, 0, 860, 0, 700, 770,
      0, 660, 0, 520, 580, 480, 0, 510, 0, 380, 0, 320, 0,
      440, 0, 480, 0, 450, 430, 0, 380, 660, 0,
      760, 0, 860, 0, 700, 770, 0, 660, 0, 520,
      580, 0, 660, 0, 870, 0, 760, 0, 660, 0,
      520, 580, 0, 480
    };
int Duraciones[84] = {
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150, 150, 150, 225, 225,
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 225, 225, 150, 150, 150, 150,
      150, 150, 150, 150, 150, 150,
      150, 150, 150, 150, 150
    };

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);
Sensor sensor = setupSensor();
void Sensor_void();

Musica musica = setupMusica();
void Musica_void();

void setup() {
  Serial.begin(9600); // Inicia la comunicación serial a 9600 bps
  pinMode(sensor.buzzerPin, INPUT); // Configura el pin A0 como entrada

  pinMode(musica.buzzerPin, OUTPUT);
}

void loop() {
  Sensor_void();
  Musica_void();
}

void Sensor_void() {
  switch(sensor.estadoActual){
      case Midiendo:
        if(sensor.k < sensor.noSamples){
          sensor.sumOfSamples += analogRead(sensor.buzzerPin); // Suma acumulada de las lecturas
          sensor.k++;
          sensor.estadoActual = Esperando;
          sensor.timer = millis();
        } else{
          sensor.k = 0;
          sensor.estadoActual = Procesando;
        }
      break;

      case Esperando:
        if( millis() - sensor.timer >= 2 ){
          sensor.estadoActual = Midiendo;
        }
      break;

      case Procesando:
        sensor.analog = sensor.sumOfSamples / sensor.noSamples; // Calcula el promedio
        sensor.Voltaje = mapDouble((double)sensor.analog, 0.0, 4095.0, 0.0, 5.0); // Convierte a voltaje

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

        //sensor.Datos[sensor.i][sensor.j] = sensor.distancia;
        sensor.estadoActual = Girando;
        sensor.timer = millis();
      break;

      case Girando:
        if( millis() - sensor.timer >= 1000 ){
          sensor.sumOfSamples = 0;
          sensor.j++;
          sensor.estadoActual = Subiendo;
        }
      break;

      case Subiendo:
        if( sensor.j == 79 ){
          sensor.j = 0;
          sensor.i--;
          Serial.print(sensor.i);
          Serial.println("");
        }
        Serial.print(" | ");
        Serial.print(sensor.j);
        sensor.sumOfSamples = 0;
        sensor.estadoActual = Midiendo;
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
            tone(musica.buzzerPin, musica.melodia[musica.notaActual]);
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
      if (musica.tiempoActual - musica.tiempoFinalizado >= 5000) {
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
