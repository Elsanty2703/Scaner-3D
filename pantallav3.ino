#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// ==== OLED SPI (ESP32) ====
#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     32
#define OLED_RESET  26
#define OLED_CS     25

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// ==== ESTRUCTURA SENSOR ====
enum Estado_Sensor { INICIO, MIDIENDO, ESPERANDO, PROCESANDO, GIRANDO, SUBIENDO };

struct Sensor {
  Estado_Sensor estadoActual;
  int analogPin;
  int buzzerPin;
  int noSamples;
  double sumOfSamples;
  int k;
  int i;
  int contador;
  double Voltaje;
  float distancia;
  unsigned long timer;
};

Sensor sensor;

// ==== RESULTADOS FINALES ====
int numTriangulos = 1245;
int numPuntos = 3250;
float resolucion = 0.5; // mm

// ==== Temporizador para pantalla inicial ====
unsigned long inicioDisplayMillis = 0;
bool mostrarInicio = true;

void setup() {
  Serial.begin(9600);
  pinMode(33, INPUT); // A0
  pinMode(27, OUTPUT); // Buzzer opcional

  sensor = { MIDIENDO, 33, 27, 40, 0, 0, 0, 0, 0.0, 0.0, 0 };

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 27);
  display.println("ESCANEO 3D");
  display.display();
  inicioDisplayMillis = millis();
}

void loop() {
  if (mostrarInicio) {
    if (millis() - inicioDisplayMillis >= 2000) {
      mostrarInicio = false;
      display.clearDisplay();
    } else {
      return; // Esperar hasta que pasen los 2 segundos
    }
  }

  SensorFSM();

  if (sensor.estadoActual == MIDIENDO || sensor.estadoActual == PROCESANDO || sensor.estadoActual == GIRANDO || sensor.estadoActual == SUBIENDO) {
    mostrarEscaneoOLED();
  }

  if (sensor.estadoActual == INICIO) {
    mostrarResultadosOLED();
  }
}

void SensorFSM() {
  double promedio = 0.0; // Declarar fuera del switch para evitar error

  switch (sensor.estadoActual) {
    case INICIO:
      break;

    case MIDIENDO:
      if (sensor.k < sensor.noSamples) {
        sensor.sumOfSamples += analogRead(sensor.analogPin);
        sensor.k++;
        sensor.estadoActual = ESPERANDO;
        sensor.timer = millis();
      } else {
        sensor.k = 0;
        sensor.estadoActual = PROCESANDO;
      }
      break;

    case ESPERANDO:
      if (millis() - sensor.timer >= 2) {
        sensor.estadoActual = MIDIENDO;
      }
      break;

    case PROCESANDO:
      promedio = sensor.sumOfSamples / sensor.noSamples;
      sensor.Voltaje = mapDouble(promedio, 0.0, 4095.0, 0.0, 3.3); // ESP32 ADC de 12 bits

      if (sensor.Voltaje >= 2.9)
        sensor.distancia = 40.82 * exp(-0.65 * sensor.Voltaje) + 0.3;
      else if (sensor.Voltaje >= 2.7)
        sensor.distancia = 21.83 * pow(sensor.Voltaje, -1.03) - 0.50;
      else if (sensor.Voltaje >= 2.4)
        sensor.distancia = 21.83 * pow(sensor.Voltaje, -1.03) - 0.34;
      else
        sensor.distancia = 21.13 * pow(sensor.Voltaje, -0.98);

      Serial.print("Distancia medida: ");
      Serial.print(sensor.distancia, 2);
      Serial.println(" cm");

      if (sensor.distancia > 30) {
        sensor.distancia = 100;
        sensor.contador++;
      } else {
        sensor.contador = 0;
      }

      sensor.estadoActual = GIRANDO;
      sensor.timer = millis();
      break;

    case GIRANDO:
      if (millis() - sensor.timer >= 100) {
        sensor.sumOfSamples = 0;
        sensor.i++;
        sensor.estadoActual = SUBIENDO;
      }
      break;

    case SUBIENDO:
      sensor.sumOfSamples = 0;
      sensor.estadoActual = MIDIENDO;
      if (sensor.i >= 100) {
        sensor.estadoActual = INICIO;
      }
      break;
  }
}

void mostrarEscaneoOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Escaneando: ");
  display.print(sensor.i);
  display.print("%");

  display.drawRect(0, 8, 128, 10, WHITE);
  display.fillRect(0, 8, (128 * sensor.i / 100), 10, WHITE);

  display.setCursor(0, 20);
  display.print("Distancia: ");
  display.print(sensor.distancia, 1);
  display.print(" cm");

  display.display();
}

void mostrarResultadosOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESCANEO COMPLETO");
  display.setCursor(0, 8);
  display.print("Triangulos: ");
  display.println(numTriangulos);
  display.setCursor(0, 16);
  display.print("Puntos: ");
  display.println(numPuntos);
  display.setCursor(0, 24);
  display.print("Resolucion: ");
  display.print(resolucion, 1);
  display.println(" mm");
  display.display();
}

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}