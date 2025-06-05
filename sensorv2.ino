typedef enum{ Midiendo, Procesando, Girando, Subiendo}Estado;
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
} sensor;

Estado setupSensor();
bool readSensor(sensor *s);



// Declaración de la función mapDouble
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

Estado setupSensor() {
    sensor s;
    s.estadoActual = Girando;
    s.noSamples = 100;
    s.senseValue = 0;
    s.sumOfSamples = 0;
    s.analog = 0;
    s.Voltaje = 0;
    s.distancia = 0;
    s.chipSelect = 10
    return s.estadoActual;
}