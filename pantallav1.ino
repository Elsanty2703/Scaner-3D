#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// Pines  (ESP32)  pueden cambiar

#define OLED_CLK    9  // D0
#define OLED_MOSI   10 // D1
#define OLED_DC     11
#define OLED_RESET  13
#define OLED_CS     12
#define PIN_INTERRUPTOR   6

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Estados de la máquina
enum Estado {
  BIENVENIDO,
  INICIO,
  ESCANEANDO,
  FINALIZADO
};

Estado estadoActual = BIENVENIDO;


// Variables de tiempo
unsigned long tiempoAnterior = 0;
unsigned long intervaloCarga = 100;
unsigned long intervaloDistancia = 200;
unsigned long ultimoCambioBoton = 0;
unsigned long debounceDelay = 50;
bool botonPresionado = false;


// Variables de escaneo
int carga = 0;
float distancia = 0.0;
bool escaneoCompleto = false;
/////



// Resultados simulados
int numTriangulos = 1245;
int numPuntos = 3250;
float resolucion = 0.5; // en mm

////////

void setup() {
  pinMode(PIN_INTERRUPTOR, INPUT);  // Botón con resistencia pull-up interna
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();
  tiempoAnterior = millis();
}

void loop() {
  switch (estadoActual) {
    case BIENVENIDA:
      mostrarPantallaBienvenida();
      manejarBotonInicio();
      break;

    case INICIO:
      case BIENVENIDA:
      mostrarPantallaBienvenida();
      manejarBotonInicio();
      break;

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(10, 25);
      display.println("ESCANEO 3D");
      display.display();
      delay(2000);  // Espera breve antes de comenzar
      carga = 0;
      estadoActual = ESCANEANDO;
      break;

    case ESCANEANDO:
      actualizarCargaYDistancia();
      dibujarEscaneo();
      if (carga >= 100) {
        escaneoCompleto = true;
        estadoActual = FINALIZADO;
        delay(500); // breve pausa
      }
      break;

    case FINALIZADO:
      mostrarResultados();
      manejarBotonReinicio();
      break;
  }
}

//                         === FUNCIONES ===



void pantallaBienvenida() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 10);
  display.println("BIENVENIDO");
  display.setCursor(0, 30);
  display.println("Presiona el boton rojo");
  display.setCursor(0, 45);
  display.println("para iniciar el escaneo");
  display.display();

  int lectura = digitalRead(PIN_INTERRUPTOR);
  unsigned long tiempoActual = millis();

  if (lectura == HIGH && !botonPresionado && (tiempoActual - ultimoCambioBoton > debounceDelay)) {
    botonPresionado = true;
    ultimoCambioBoton = tiempoActual;
    estadoActual = INICIO;
  }

  if (lectura == LOW) {
    botonPresionado = false;
  }
}

void pantallaInicio() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 25);
  display.println("ESCANEO 3D");
  display.display();

  // Reiniciar variables
  carga = 0;
  distancia = 0;
  escaneoCompleto = false;
  tiempoAnteriorCarga = millis();

  estadoActual = ESCANEANDO;
}

void actualizarCargaYDistancia() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnteriorCarga >= intervaloCarga) {
    tiempoAnteriorCarga = tiempoActual;
    if (carga < 100) carga++;
  }

  distancia = 10 + 20 * sin(millis() / 1000.0); // Simulación
}

void dibujarEscaneo() {
  display.clearDisplay();

  // Barra de carga
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Escaneando: ");
  display.print(carga);
  display.print("%");
  display.drawRect(0, 10, 128, 10, WHITE);
  display.fillRect(0, 10, (128 * carga / 100), 10, WHITE);

  // Distancia
  display.setCursor(0, 28);
  display.setTextSize(2);
  display.print("Dist:");
  display.setTextSize(1);
  display.setCursor(0, 48);
  display.print(distancia, 1);
  display.print(" cm");

  display.display();
}

void mostrarResultados() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESCANEO COMPLETO");

  display.setCursor(0, 15);
  display.print("Triangulos: ");
  display.println(numTriangulos);

  display.setCursor(0, 28);
  display.print("Puntos: ");
  display.println(numPuntos);

  display.setCursor(0, 41);
  display.print("Resolucion: ");
  display.print(resolucion, 1);
  display.println(" mm");

  display.display();
}

void manejarBotonReinicio() {
  int lectura = digitalRead(PIN_INTERRUPTOR);
  unsigned long tiempoActual = millis();

  if (lectura == HIGH && !botonPresionado && (tiempoActual - ultimoCambioBoton > debounceDelay)) {
    botonPresionado = true;
    ultimoCambioBoton = tiempoActual;
    estadoActual = BIENVENIDA; // Volver a pantalla de bienvenida
  }

  if (lectura == LOW) {
    botonPresionado = false;
  }
}