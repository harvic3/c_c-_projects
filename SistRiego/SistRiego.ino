/*
Sistema de Control de Riego Automático a partir de la humedad 
del Suelo, la humedad del Aire y la temperatura.
*/

//Directivas
#include <LiquidCrystal.h> //Libreria Display 2x16
#include <DHT.h>
#include <EEPROM.h>

//Configuración del PIN para recibir los datos del Sensor DHT11
#define DHTPIN 2
//Defiinimos Tipo de Sensor
#define DHTTYPE DHT11
//Indica el pin con el que trabajamos y el tipo de sensor
DHT dht(DHTPIN, DHTTYPE);

//Configuración de Pines LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Configuración de pines
const int Teclado = A0; //Entrada del Teclado
const int Estado = 13;//Led Indicador del Motor
const int SensorHT = A1;//Entrada Sensor Humedad Tierra
const int PWMPin = 3;//PWM para el motor de la bomba
const short FMC = 10; //Factor de Modulacion de Control del Riego MAX 10, MIN 2
const int DHC = 5; //Constante de Control de Humedad de la Tierra

//declaracion de variables
short HumTierra = 0;//Humedad Tierra
short HumAir = 0;//Humedad Aire
short Temp = 0;//Temperatura
short SetPoint = 0;//Humedad a Controlar
short TempProm = 0; //Temperatura Promedio
short HumAirProm = 0; //Humedad del Aire Promedio
short MaxH = 0;
short MinH = 100;
short MaxT = 0;
short MinT = 100;
int PWMValue = 0;
int OutValue = 0;
int Button = 0;

void setup() {
  //Inicio comunicacion serie para ver los datos en el ordenador
  Serial.begin(9600); 
  //Mensaje de inicio
  Serial.println("Comprobacion sensor DHT11:");
  //Iniciamos el sensor
  dht.begin();
  //Iniciamos el Display  
  lcd.begin(16, 2); //LCD dof 16x2
  //Iniciamos Pin de Estado como Salida
  pinMode(Estado, OUTPUT);
  //Iniciamos Pin PWM como Salida
  pinMode(PWMPin, OUTPUT);  
  //Se lee SetPoint
  SetPoint = EEPROM.read(0);
  TempProm = EEPROM.read(1);
  HumAirProm = EEPROM.read(2);
  //Iniciar Mensajes en el LCD
  lcd.clear();
  lcd.setCursor(3, 0); //Colunma - Fila
  lcd.print("Bienvenido");
  lcd.setCursor(0, 1);
  lcd.print("Sistema de Riego");
  delay(1000);
  lcd.clear();    
  lcd.setCursor(0, 1);
  lcd.print(SetPoint);  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HR: ");
  lcd.setCursor(8, 0);
  lcd.print("HS:");
  lcd.setCursor(0, 1);
  lcd.print("T: ");
  lcd.setCursor(8, 1);
  lcd.print("SP: ");
  lcd.setCursor(12, 1);
  lcd.print(SetPoint);
  lcd.print("%");
}

void loop() {
  //Se lee Humedad y Temperatura del Aire
  HumAir = dht.readHumidity();
  Temp = dht.readTemperature();
  //Se lee Humedad de la tierra
  HumTierra = CalcHumedad();
  
  Button = analogRead(Teclado);
  if (Button < 800){
    DefinirParametros();
  }
   
  if (isnan(Temp) || isnan(HumAir)) {// funcion que comprueba si son numeros las variables indicadas 
    Serial.println("Fallo al leer del sensor DHT"); //Mostramos mensaje de fallo si no son numeros
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Falla del Sensor");
  }
  else { //Mostramos mensaje con valores actuales de humedad y temperatura, asi como maximos y minimos de cada uno de ellos
    Serial.print("Humedad relativa: "); 
    Serial.print(HumAir);
    lcd.setCursor(4, 0);
    lcd.print(HumAir);
    lcd.setCursor(6, 0);
    lcd.print("%");
    lcd.setCursor(12, 0);
    lcd.print(HumTierra);
    lcd.print("%");
    Serial.print(" %\t");
    Serial.print("Temperatura: "); 
    Serial.print(Temp);
    Serial.println(" *C");
    lcd.setCursor(3, 1);
    lcd.print(Temp);
    lcd.setCursor(5, 1);
    lcd.print("*C");
    
    
    //Comprobacion de maximos y minimos de humedad y temperatura
    if (MaxH < HumAir)
      MaxH = HumAir;
    if (HumAir < MinH)
      MinH = HumAir;
    if (MaxT < Temp)
      MaxT = Temp;
    if (Temp < MinT)
      MinT = Temp;
    Serial.print("Max: ");
    Serial.print(MaxH);
    Serial.print(" % ");
    Serial.print("Min: ");
    Serial.print(MinH);
    Serial.print(" %\t");
    Serial.print("Max: ");
    Serial.print(MaxT);
    Serial.print(" *C ");
    Serial.print("Min: ");
    Serial.print(MinT);
    Serial.println(" *C");
    Serial.print("Promedio Temperatura: ");
    Serial.print(TempProm);
    Serial.println(" *C");
    Serial.print("SetPoint Humedad Suelo ");
    Serial.print(SetPoint);
    Serial.println(" %");    
    Serial.print("Promedio HR: ");
    Serial.print(HumAirProm);
    Serial.println(" %");
    Serial.print("PWM: ");
    Serial.print(PWMValue);
    Serial.println("\n");
    
    
    //Controlamos la Humedad si cae del nivel del Setpoint
    if ((SetPoint - HumTierra) > DHC) {//Se debe encender la bomba
      PWMValue = Controlar();
      digitalWrite(Estado,HIGH);
      //Enviamos Valor a Bomba
      //PWMValue = map(OutValue, 0, 1023, 0, 255);
      analogWrite(PWMPin, PWMValue);
    }
    else if (PWMValue > 0) {//Se debe apagar la Bomba si está encendida.
      PWMValue = 0;      
      analogWrite(PWMPin, PWMValue);
      digitalWrite(Estado,LOW);
    }  
    delay(5000);
  
  }
}//Fin Programa
/*
Autores:
Manuela Lopez
Victor Higuita
*/


//Configuración de Parametros del Control de Riego
void DefinirParametros() {
  int Aux = 0;
  short SW = 0;  
  
  //Inicializar el SetPoint
  lcd.clear();
  lcd.setCursor(3, 0); //Colunma - Fila
  lcd.print("Configurar");
  lcd.setCursor(0, 1);
  lcd.print("Sistema de Riego");
  delay(2000);
  lcd.clear();
  
  //Introduce SetPoint
  lcd.setCursor(0, 0);
  lcd.print("Asigne SetPoint:");
  lcd.setCursor(0, 1);
  lcd.print(SetPoint);
  while (SW == 0) {
    Aux = analogRead(Teclado);
    if (Aux < 200) {
      SetPoint = SetPoint + 1;
      if (SetPoint > 90) {
        SetPoint = 90;
      }
    }
    else if (Aux < 400) {
      SetPoint = SetPoint - 1;
      if (SetPoint < 10) {
        SetPoint = 10;
      }
    }
    else if (Aux < 800){
      EEPROM.write(0, SetPoint);
      SW = 1;
    }
    lcd.setCursor(0, 1);
    lcd.print(SetPoint);
    lcd.print("%");
    delay(250);
  }

  //Introduce Temp Promedio
  SW = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Asigne Temp Prom:");
  lcd.setCursor(0, 1);
  lcd.print(TempProm);
  while (SW == 0) {
    Aux = analogRead(Teclado);
    if (Aux < 200) {
      TempProm = TempProm + 1;
      if (TempProm > 55) {
        TempProm = 55;
      }
    }
    else if (Aux < 400) {
      TempProm = TempProm - 1;
      if (TempProm < 0) {
        TempProm = 0;
      }
    }
    else if (Aux < 800){
      EEPROM.write(1, TempProm);
      SW = 1;
    }
    lcd.setCursor(0, 1);
    lcd.print(TempProm);
    lcd.print("*C");
    delay(250);
  }
  
  //Introduce Humedad Relativa Promedio
  SW = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Asigne HR Prom:");
  lcd.setCursor(0, 1);
  lcd.print(HumAirProm);
  while (SW == 0) {
    Aux = analogRead(Teclado);
    if (Aux < 200) {
      HumAirProm = HumAirProm + 1;
      if (HumAirProm > 90) {
        HumAirProm = 90;
      }
    }
    else if (Aux < 400) {
      HumAirProm = HumAirProm - 1;
      if (HumAirProm < 10) {
        HumAirProm = 10;
      }
    }
    else if (Aux < 800) {
      EEPROM.write(2, HumAirProm);
      SW = 1;
    }
    lcd.setCursor(0, 1);
    lcd.print(HumAirProm);
    lcd.print("%");
    delay(250);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HR: ");
  lcd.setCursor(8, 0);
  lcd.print("HS: ");
  lcd.setCursor(0, 1);
  lcd.print("T: ");
  lcd.setCursor(8, 1);
  lcd.print("SP: ");
  lcd.setCursor(12, 1);
  lcd.print(SetPoint);
  lcd.print("%");
}

//Control de PWM
int Controlar() {
  int PWMBomba, Vel;
  short VCT, VCHT, VCHA;
  	
  VCT = Temp - TempProm;
  VCHT = SetPoint - HumTierra;
  VCHA = HumAirProm - HumAir;
	
  if (VCT < 0) {
    VCT = 1;    
  }
  
  if (VCHT < 0) {
    VCHT = 0;
  }
  
  if (VCHA < 0) {
    VCHA = 1;
  }
  
  Vel = (VCT + VCHA) * VCHT;
  		
  PWMBomba = Vel * FMC;
  
  if (PWMBomba > 255) {
    PWMBomba = 255;
  }	
  else if ((PWMBomba > 0)&&(PWMBomba < 10)) {
    PWMBomba = 10;
  }  
  return(PWMBomba);
}

//Calcular Humedad
int CalcHumedad()  {
  short Hum, HumP;
  int ValSen, AuxHR;

  ValSen = analogRead(SensorHT); //Leemos el Sensor  
  if (ValSen < 205) {
    ValSen = 205;
  }  
  AuxHR = (ValSen - 205)/10.23; 
  AuxHR = AuxHR + 10; 
  Hum = 100 - AuxHR;
  return(Hum);
}
