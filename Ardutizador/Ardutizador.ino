
//Definición de Sensores
#define EntSenPresionDer1   A0
#define EntSenPresionDer2   A1   
#define EntSenPresionDer3   A2 
#define EntSenPresionDer4   A3 
#define EntModuDer          A4
#define EntSenPresionIzq1   A5 
#define EntSenPresionIzq2   A6 
#define EntSenPresionIzq3   A7
#define EntSenPresionIzq4   A8
#define EntModuIzq          A9
#define EntSenTemperatura   A10 
#define EntSenLuz           A11

#define EntSenSonido        23

#define EntSenCapIzq1       24
#define EntSenCapIzq2       25
#define EntSenCapIzq3       26
#define EntSenCapDer1       27
#define EntSenCapDer2       28
#define EntSenCapDer3       29

#define SalLedDer1          2
#define SalLedDer2          3
#define SalLedDer3          4
#define SalLedDer4          5
#define SalLedDer5          6 
#define SalLedIzq1          7
#define SalLedIzq2          8
#define SalLedIzq3          9
#define SalLedIzq4          10
#define SalLedIzq5          11

#define SalLedTactil        45
#define SalLedPower         47
#define EntRunStop          48
#define SalLedRunStop       49

unsigned long tiempoTemp = 0;
unsigned long tiempoTotal = 0;

//Definición de variables
const bool simular = false; //Variable para indicarle al sistema que simule los valores
const int escala = 300; //Escala de suma o de resta al valor simulado. No puede superar 1024
const int valorMax = 1024; //Limite del ADC
const int valorMaximoPresion = 1000;

bool Run = false;
bool Enviado = false;

//Definición de Variables de los valores de los sensores
int SenPresIzq1Value = 0;
int SenPresIzq2Value = 0;
int SenPresIzq3Value = 0;
int SenPresIzq4Value = 0;
int SenPresDer1Value = 0;
int SenPresDer2Value = 0;
int SenPresDer3Value = 0;
int SenPresDer4Value = 0;
int ModuIzqValue = 0;
int ModuDerValue = 0;

byte SenSonidoValue = 0;
int SenTempValue = 0;
int SenLuzValue = 0;
int SenVibraValue = 0;
byte SenCapIzq1Value = 0;
byte SenCapIzq2Value = 0;
byte SenCapIzq3Value = 0;
byte SenCapDer1Value = 0;
byte SenCapDer2Value = 0;
byte SenCapDer3Value = 0;

//Operaciones S = Sumar, R = Restar, A = Aleatorio
char opeS1 = 'S';
char opeS2 = 'S';
char opeS3 = 'S';
char opeS4 = 'S';
char opeS5 = 'S';
char opeS6 = 'S';
char opeS7 = 'S';
char opeS8 = 'S';

void setup() {
  Serial.begin(9600);
  pinMode(EntRunStop, INPUT_PULLUP);
  pinMode(SalLedPower, OUTPUT);
  pinMode(SalLedRunStop, OUTPUT);
  pinMode(SalLedTactil, OUTPUT);

  pinMode(SalLedDer1, OUTPUT);
  pinMode(SalLedDer2, OUTPUT);
  pinMode(SalLedDer3, OUTPUT);
  pinMode(SalLedDer4, OUTPUT);
  pinMode(SalLedDer5, OUTPUT);
  pinMode(SalLedIzq1, OUTPUT);
  pinMode(SalLedIzq2, OUTPUT);
  pinMode(SalLedIzq3, OUTPUT);
  pinMode(SalLedIzq4, OUTPUT);
  pinMode(SalLedIzq5, OUTPUT);
  
  //Ojo, trabajar con lógica inversa
  pinMode(EntSenSonido, INPUT_PULLUP);
  pinMode(EntSenCapIzq1, INPUT_PULLUP);
  pinMode(EntSenCapIzq2, INPUT_PULLUP);
  pinMode(EntSenCapIzq3, INPUT_PULLUP);
  pinMode(EntSenCapDer1, INPUT_PULLUP);
  pinMode(EntSenCapDer2, INPUT_PULLUP);
  pinMode(EntSenCapDer3, INPUT_PULLUP);
  
  digitalWrite(SalLedRunStop, LOW);
  digitalWrite(SalLedPower, HIGH);
  digitalWrite(SalLedTactil, LOW);
  resetSensorsValue();
}

void loop() {
  tiempoTemp = micros();
  startStop();
  if (true){
  //if (Run){
    if (simular){
      simularValores();
    }else{
      leerSensores();    
    }
    intensidadLeds();
    enviarLecturas();
  }
  tiempoTotal = micros() - tiempoTemp;  
}

void startStop(){
  if (digitalRead(EntRunStop) == LOW){
    bool estado = false;
    while (!estado){
      estado = digitalRead(EntRunStop);
      if(Run && !Enviado){
        resetSensorsValue();
        Enviado = true;
        digitalWrite(SalLedRunStop, LOW);
      }
    }
    Run = !Run;
    if (Run){
      digitalWrite(SalLedRunStop, HIGH);
    }
    Enviado = false;
    delay(250);
  }
}

void resetSensorsValue(){
  SenPresIzq1Value = 0;
  SenPresIzq2Value = 0;
  SenPresIzq3Value = 0;
  SenPresIzq4Value = 0;
  SenPresDer1Value = 0;
  SenPresDer2Value = 0;
  SenPresDer3Value = 0;
  SenPresDer4Value = 0;
  ModuIzqValue = 0;
  ModuDerValue = 0;
  SenSonidoValue = 0;
  SenTempValue = 0;
  SenLuzValue = 0;
  SenVibraValue = 0;
  SenCapIzq1Value = 0;
  SenCapIzq2Value = 0;
  SenCapIzq3Value = 0;
  SenCapDer1Value = 0;
  SenCapDer2Value = 0;
  SenCapDer3Value = 0;
  enviarLecturas();
}

void simularValores() {
  SenPresIzq1Value = calcularValor(SenPresIzq1Value, 1, opeS1, 10);
  SenPresIzq2Value = calcularValor(SenPresIzq2Value, 1, opeS2, 20);
  SenPresIzq3Value = calcularValor(SenPresIzq3Value, 1, opeS3, 30);
  SenPresIzq4Value = calcularValor(SenPresIzq4Value, 1, opeS4, 40);
  SenPresDer1Value = calcularValor(SenPresDer1Value, 1, opeS5, 50);
  SenPresDer2Value = calcularValor(SenPresDer2Value, 1, opeS6, 60);
  SenPresDer3Value = calcularValor(SenPresDer3Value, 1, opeS7, 70);
  SenPresDer4Value = calcularValor(SenPresDer4Value, 1, opeS8, 80);
}

int calcularValor(int valorActual, byte ganancia, char &operacion, int cambio){
  int value = cambio;
  switch (operacion){
    case 'A':
      value = random(1, escala);
      //Si es par sumar, sino restar
      if (ganancia > 1){
        value = value * ganancia;
      }
      if (value % 2 == 0){
        if ((valorActual + value) <= valorMax){
          valorActual += value;
        }else{
          valorActual -= value;
        }
      }else{
        if ((valorActual - value) >= 0){
          valorActual -= value;
        }else{
          valorActual += value;
        }
      }
    break;
    case 'S':
      if ((valorActual + value) <= valorMax){
        valorActual += value;
      }else{
        valorActual = valorMax;
        operacion = 'R';
      }
    break;
    case 'R':
      if ((valorActual - value) >= 0){
        valorActual -= value;
      }else{
        valorActual = 0;
        operacion = 'S';
      }
    break;
  }  
  return valorActual;
}

void leerSensores() {  
  leerSensoresManoIzquierda();
  leerSensoresManoDerecha();
  leerTemperatura();
  leerSonido();
  //leerVibracion();
  leerIluminacion();
  leerCapacitivos();
}

void leerSensoresManoIzquierda(){
  leerControlandoMaximo(SenPresIzq1Value, EntSenPresionIzq1, valorMaximoPresion);
  leerControlandoMaximo(SenPresIzq2Value, EntSenPresionIzq2, valorMaximoPresion);
  leerControlandoMaximo(SenPresIzq3Value, EntSenPresionIzq3, valorMaximoPresion);
  leerControlandoMaximo(SenPresIzq4Value, EntSenPresionIzq4, valorMaximoPresion);
  leerControlandoMaximo(ModuIzqValue, EntModuIzq, valorMaximoPresion);
}

void leerSensoresManoDerecha(){
  leerControlandoMaximo(SenPresDer1Value, EntSenPresionDer1, valorMaximoPresion);
  leerControlandoMaximo(SenPresDer2Value, EntSenPresionDer2, valorMaximoPresion);
  leerControlandoMaximo(SenPresDer3Value, EntSenPresionDer3, valorMaximoPresion);
  leerControlandoMaximo(SenPresDer4Value, EntSenPresionDer4, valorMaximoPresion);
  leerControlandoMaximo(ModuDerValue, EntModuDer, valorMaximoPresion);
}

void leerControlandoMaximo(int &valorActual, int entrada, int maximo){
  valorActual = analogRead(entrada);
  if (valorActual > maximo){
    valorActual = maximo;
  }
}

void leerTemperatura(){
  // SenTempValue = analogRead(EntSenTemperatura);
  // SenTempValue = (5.0 * SenTempValue * 100.0)/1024.0;
  float tempT = analogRead(EntSenTemperatura) * 0.004882812 * 100;    //Se lee la temperatura en Kelvin
  SenTempValue = tempT - 273.15;    //Convierte de Kelvin a Centigrado 
}

void leerSonido(){
  SenSonidoValue = digitalRead(EntSenSonido);
}

void leerIluminacion(){
  SenLuzValue = analogRead(EntSenLuz);
}

void leerCapacitivos(){
  SenCapIzq1Value = digitalRead(EntSenCapIzq1);
  SenCapIzq2Value = digitalRead(EntSenCapIzq2);
  SenCapIzq3Value = digitalRead(EntSenCapIzq3);
  SenCapDer1Value = digitalRead(EntSenCapDer1);
  SenCapDer2Value = digitalRead(EntSenCapDer2);
  SenCapDer3Value = digitalRead(EntSenCapDer3);
  encenderApagarLedCapacitivo();
}

void encenderApagarLedCapacitivo(){
  if(SenCapIzq1Value == HIGH || SenCapIzq2Value == HIGH || SenCapIzq3Value == HIGH || SenCapDer1Value == HIGH || SenCapDer2Value == HIGH || SenCapDer3Value == HIGH){
    digitalWrite(SalLedTactil, HIGH);
  }else{
    digitalWrite(SalLedTactil, LOW);
  }
}

void intensidadLeds(){
  modificarLed(SalLedDer1, SenPresIzq1Value);
  modificarLed(SalLedDer2, SenPresIzq2Value);
  modificarLed(SalLedDer3, SenPresIzq3Value);
  modificarLed(SalLedDer4, SenPresIzq4Value);
  modificarLed(SalLedDer5, ModuIzqValue);
  modificarLed(SalLedIzq1, SenPresDer1Value);
  modificarLed(SalLedIzq2, SenPresDer2Value);
  modificarLed(SalLedIzq3, SenPresDer3Value);
  modificarLed(SalLedIzq4, SenPresDer4Value);
  modificarLed(SalLedIzq5, ModuDerValue);
}

void modificarLed(byte ledOut, int valorRef){
  byte nivel = valorRef * 255 / valorMax;
  analogWrite(ledOut, nivel);
}

void enviarLecturas() {
  Serial.println(
    String(SenPresIzq1Value) + ' ' + 
    String(SenPresIzq2Value) + ' ' + 
    String(SenPresIzq3Value) + ' ' + 
    String(SenPresIzq4Value) + ' ' +
    String(ModuDerValue) + ' ' +
    String(SenPresDer1Value) + ' ' + 
    String(SenPresDer2Value) + ' ' + 
    String(SenPresDer3Value) + ' ' + 
    String(SenPresDer4Value) + ' ' +  
    String(ModuIzqValue) + ' ' +
    String(SenTempValue) + ' ' + 
    String(SenLuzValue) + ' ' + 
    String(SenVibraValue) + ' ' + 
    String(SenSonidoValue) + ' ' + 
    String(SenCapIzq1Value) + ' ' +  
    String(SenCapIzq2Value) + ' ' +  
    String(SenCapIzq3Value) + ' ' +  
    String(SenCapDer1Value) + ' ' +  
    String(SenCapDer2Value) + ' ' +  
    String(SenCapDer3Value) + ' ' +
    String(tiempoTotal)
  );
}

