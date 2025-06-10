#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// Pines SPI personalizados (ESP32)
#define OLED_MOSI   19
#define OLED_CLK    18
#define OLED_DC     22
#define OLED_RESET  21
#define OLED_CS     23

#define BOTON_PIN   33  // Pin del botón para iniciar/reiniciar

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Estados de la máquina
enum Estado {
  INICIO,
  BIENVENIDA,
  ESCANEANDO,
  FINALIZADO,
  CANCELADO
};

Estado estadoActual = INICIO;

// Variables de tiempo
unsigned long tiempoAnterior = 0;
unsigned long intervaloCarga = 100;
unsigned long tiempoInicio = 0;
unsigned long tiempoPresionado = 0;
unsigned long tiempoBotonAnterior = 0;
unsigned long tiempoCancelacion = 0;
unsigned long tiempoFinalizacion = 0;

bool esperandoAntiRebote = false;

// Variables de escaneo
int carga = 0;
float distancia = 0.0;

// Resultados simulados
int numTriangulos = 1245;
int numPuntos = 3250;
float resolucion = 0.5; // en mm

void setup() {
  pinMode(BOTON_PIN, INPUT_PULLDOWN); // Botón con resistencia pull-up
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();
  tiempoInicio = millis();
  mostrarPantallaInicio();
}

void loop() {
  unsigned long ahora = millis();

  switch (estadoActual) {
    case INICIO:
      if (ahora - tiempoInicio >= 3000) {
        carga = 0;
        mostrarPantallaBienvenida();
        estadoActual = BIENVENIDA;
      }
      break;

    case BIENVENIDA:
      if (digitalRead(BOTON_PIN) == HIGH && !esperandoAntiRebote) {
        esperandoAntiRebote = true;
        tiempoBotonAnterior = ahora;
      }

      if (esperandoAntiRebote && (ahora - tiempoBotonAnterior >= 200)) {
        esperandoAntiRebote = false;
        carga = 0;
        tiempoAnterior = ahora;
        estadoActual = ESCANEANDO;
      }
      break;

    case ESCANEANDO:
      if (digitalRead(BOTON_PIN) == HIGH) {
        if (tiempoPresionado == 0) {
          tiempoPresionado = ahora;
        } else if (ahora - tiempoPresionado >= 3000) {
          mostrarCancelacion();
          tiempoCancelacion = ahora;
          estadoActual = CANCELADO;
        }
      } else {
        tiempoPresionado = 0;
      }

      actualizarCargaYDistancia();
      dibujarEscaneo();

      if (carga >= 100) {
        mostrarResultados();
        tiempoFinalizacion = ahora;
        estadoActual = FINALIZADO;
      }
      break;

    case CANCELADO:
      if (ahora - tiempoCancelacion >= 2000) {
        mostrarPantallaBienvenida();
        estadoActual = BIENVENIDA;
      }
      break;

    case FINALIZADO:
      if (digitalRead(BOTON_PIN) == HIGH && !esperandoAntiRebote) {
        esperandoAntiRebote = true;
        tiempoBotonAnterior = ahora;
      }

      if (esperandoAntiRebote && (ahora - tiempoBotonAnterior >= 200)) {
        esperandoAntiRebote = false;
        mostrarPantallaBienvenida();
        estadoActual = BIENVENIDA;
      }
      break;
  }
}

// === FUNCIONES ===

void mostrarPantallaInicio() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 10);
  display.println("ESCANEO 3D");
  display.setCursor(0, 20);
  display.println("By GRUPO COOL");
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
  delay(2000);
}

void mostrarPantallaBienvenida() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 0);
  display.println("BIENVENIDO");
  display.setCursor(0, 15);
  display.println("Press RED button");
  display.setCursor(0, 45);
  display.println("para iniciar el escaneo");
  display.display();
}

void actualizarCargaYDistancia() {
  unsigned long ahora = millis();
  if (ahora - tiempoAnterior >= intervaloCarga) {
    tiempoAnterior = ahora;
    if (carga < 100) carga++;
  }

  distancia = 10 + 20 * sin(millis() / 1000.0); // Simulación
}

void dibujarEscaneo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Escaneando: ");
  display.print(carga);
  display.print("%");
  display.drawRect(0, 10, 128, 10, WHITE);
  display.fillRect(0, 10, (128 * carga / 100), 10, WHITE);
  display.setCursor(0, 23);
  display.setTextSize(1);
  display.print("Scan Dist:");
  display.setCursor(0, 48);
  display.print(distancia, 1);
  display.print(" cm");
  display.display();
}

void mostrarResultados() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SCAN COMPLETE");

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