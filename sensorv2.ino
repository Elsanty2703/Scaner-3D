typedef enum{ FOR, REG}Estado;
typedef struct {
    Estado estadoActual;
    const int noSamples;
    int senseValue;
    double sumOfSamples;
    int analog;
    double Voltaje;
    float distancia;
    const int chipSelect;
    int Datos[200][80];
    int count;
    unsigned long prev; // para el tiempo de lectura
} sensor;

sensor setupSensor();
bool readSensor(sensor *s);



// Declaración de la función mapDouble
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

sensor setupSensor() {
    pinMode(36, INPUT); // Pin para el sensor
    sensor s;
    s.estadoActual = FOR;
    s.noSamples = 100;
    s.senseValue = 0;
    s.sumOfSamples = 0;
    s.analog = 0;
    s.Voltaje = 0;
    s.distancia = 0;
    s.chipSelect = 10
    s.count = 0;
    s.prev = millis(); // Inicializa el tiempo de lectura
    return s;
}

bool readSensor(sensor *s){
    case(s->estadoActual) {
        case FOR:
            if( millis() - s->prev < 2) {
            } else if(s->count < s->noSamples) {
                s->sumOfSamples += analogRead(36); // Leer el valor del sensor
                s->count++;
                s->prev = millis(); 
            } else {
                // Calcular el promedio de voltaje
                s->analog = s->sumOfSamples / s->noSamples;
                s->Voltaje = mapDouble((double)s->analog, 0.0, 1023.0, 0.0, 5.0); // Convertir a voltaje
                Serial.print("Voltaje: ");
                Serial.print(s->Voltaje);
                Serial.print(" V | ");
                s->estadoActual = REG; // Cambiar el estado a REG después de leer
            }
            break

        case REG:
            if (s->Voltaje >= 2.9) {  // 6
                s->distancia = 40.82545 * exp(-0.65439 * s->Voltaje) + 0.3;  // Modelo 5
            } else if (s->Voltaje >= 2.7) {  // 7
                s->distancia = 21.83057 * pow(s->Voltaje, -1.03006) - 0.50;  // Modelo 4
            } else if (s->Voltaje >= 2.4) {  // 8
                s->distancia = 21.83057 * pow(s->Voltaje, -1.03006) - 0.34;  // Modelo 4
            } else if (s->Voltaje >= 2.2) {  // 9
                s->distancia = 40.82545 * exp(-0.65439 * s->Voltaje) + 0.06;  // Modelo 5
            } else if (s->Voltaje >= 2.0) {  // 10
                s->distancia = 21.83057 * pow(s->Voltaje, -1.03006) - 0.09;   // Modelo 4
            } else if (s->Voltaje >= 1.8) {  // 11
                s->distancia = 21.12816 * pow(s->Voltaje, -0.98062) - 0.17 ;   // Modelo 3
            } else if (s->Voltaje >= 1.72) {  // 12
                s->distancia = 21.12816 * pow(s->Voltaje, -0.98062) - 0.10;    // Modelo 3
            } else if (s->Voltaje >= 1.59) {  // 13
                s->distancia = 21.12816 * pow(s->Voltaje, -0.98062) - 0.20;  // Modelo 3
            } else if (s->Voltaje >= 1.49) {  // 14
                s->distancia = 21.12816 * pow(s->Voltaje, -0.98062);  // Modelo 3
            } else if (s->Voltaje >= 1.39) {  // 15
                s->distancia = 21.12816 * pow(s->Voltaje, -0.98062) + 0.12; // Modelo 3
            } else if (s->Voltaje >= 1.29) {  // 16
                s->distancia = 20.4026 * pow(s->Voltaje, -0.90119) - 0.11;  // Modelo 2
            } else if (s->Voltaje >= 1.20) {  // 17
                s->distancia = 20.4026 * pow(s->Voltaje, -0.90119) ;  // Modelo 2
            } else if (s->Voltaje >= 1.09) {  // 18 y 19
                s->distancia = 21.12816 * pow(s->Voltaje, -0.98062) + 0.15;  // Modelo 3
            } else {  // 20++
                s->distancia = 21.12816 * pow(s->Voltaje, -0.98062);  // Modelo 3
            }
            s->Datos[i][j]=s->distancia;
            j++;
            if(j==80)i++;
        }
    }
}