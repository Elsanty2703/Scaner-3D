#include <math.h>

enum Estado { Midiendo, Procesando, Girando, Subiendo};
Estado estadoActual = Girando;

const int chipSelect = 10;
int Datos[200][80];

// Declaración de la función mapDouble
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

void setup() {
  Serial.begin(9600); // Inicia la comunicación serial a 9600 bps
  pinMode(A0, INPUT); // Configura el pin A0 como entrada
}

void loop() {
  const int noSamples = 100;
  int senseValue = 0;
  double sumOfSamples = 0;
  int analog = 0;
  double Voltaje = 0;
  float distancia = 0;

  // Realiza múltiples lecturas para obtener un promedio
  for (int i = 0; i < noSamples; i++) {
    sumOfSamples += analogRead(A0); // Suma acumulada de las lecturas
    delay(2); // Pequeña pausa para estabilizar la lectura
  }

  analog = sumOfSamples / noSamples; // Calcula el promedio

  Voltaje = mapDouble((double)analog, 0.0, 1023.0, 0.0, 5.0); // Convierte a voltaje

  Serial.print("Voltaje: ");
  Serial.print(Voltaje);
  Serial.print(" V | ");

if (Voltaje >= 2.9) {  // 6
    distancia = 40.82545 * exp(-0.65439 * Voltaje) + 0.3;  // Modelo 5
  }
  else if (Voltaje >= 2.7) {  // 7
    distancia = 21.83057 * pow(Voltaje, -1.03006) - 0.50;  // Modelo 4
  }
  else if (Voltaje >= 2.4) {  // 8
    distancia = 21.83057 * pow(Voltaje, -1.03006) - 0.34;  // Modelo 4
  }
  else if (Voltaje >= 2.2) {  // 9
    distancia = 40.82545 * exp(-0.65439 * Voltaje) + 0.06;  // Modelo 5
  }
  else if (Voltaje >= 2.0) {  // 10
    distancia = 21.83057 * pow(Voltaje, -1.03006) - 0.09;   // Modelo 4
  }
  else if (Voltaje >= 1.8) {  // 11
    distancia = 21.12816 * pow(Voltaje, -0.98062) - 0.17 ;   // Modelo 3
  }
  else if (Voltaje >= 1.72) {  // 12
    distancia = 21.12816 * pow(Voltaje, -0.98062) - 0.10;    // Modelo 3
  }
  else if (Voltaje >= 1.59) {  // 13
    distancia = 21.12816 * pow(Voltaje, -0.98062) - 0.20;  // Modelo 3
  }
  else if (Voltaje >= 1.49) {  // 14
    distancia = 21.12816 * pow(Voltaje, -0.98062);  // Modelo 3
  }
  else if (Voltaje >= 1.39) {  // 15
    distancia = 21.12816 * pow(Voltaje, -0.98062) + 0.12; // Modelo 3
  }
  else if (Voltaje >= 1.29) {  // 16
    distancia = 20.4026 * pow(Voltaje, -0.90119) - 0.11;  // Modelo 2
  }
  else if (Voltaje >= 1.20) {  // 17
    distancia = 20.4026 * pow(Voltaje, -0.90119) ;  // Modelo 2
  }
  else if (Voltaje >= 1.09) {  // 18 y 19
    distancia = 21.12816 * pow(Voltaje, -0.98062) + 0.15;  // Modelo 3
  }
  else {  // 20++
    distancia = 21.12816 * pow(Voltaje, -0.98062);  // Modelo 3
  }

  //float Distance1 = 41.7847 * exp(-0.68313 * Voltaje);
  //float Distance2 = 20.4026 * pow(Voltaje, -0.90119);
  //float Distance3 = 21.12816 * pow(Voltaje, -0.98062);
  //float Distance4 = 21.83057 * pow(Voltaje, -1.03006);
  //float Distance5 = 40.82545 * exp(-0.65439 * Voltaje);

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  Datos[i][j]=distancia;
  j++;
  if(j==80)
    i++;

  delay(1000);
}

// Definición de la función mapDouble
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  
  return (x - in_min) * (out_max - out_min) / (in_max - in_min)+out_min;
}
