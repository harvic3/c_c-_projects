#include <Wire.h>
#include <LiquidCrystal_I2C.h>


volatile float time = 0;
volatile float time_last = 0;
volatile int rpm_array[5] = {0,0,0,0,0};

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{
  //Digital Pin 2 Set As An Interrupt
 attachInterrupt(0, motor_interrupt, FALLING);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(1000);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on

  lcd.setCursor(0,0); //Start at character 4 on line 0
  lcd.print("Velocidad  Motor");
     
}

//Main Loop To Calculate RPM and Update LCD Display
void loop()
{
  int rpm = 0;
  
  while(1){    

  //Slow Down The LCD Display Updates
  delay(400);    

    //Clear The Bottom Row
  lcd.setCursor(3, 1);
  lcd.print("       RPM");
  
  //Update The Rpm Count
  lcd.setCursor(3, 1);
  lcd.print(rpm);   
  
  ////lcd.setCursor(4, 1);
  ////lcd.print(time);   

  //Update The RPM
  if(time > 0)
  {
    //5 Sample Moving Average To Smooth Out The Data
      rpm_array[0] = rpm_array[1];
      rpm_array[1] = rpm_array[2];
      rpm_array[2] = rpm_array[3];
      rpm_array[3] = rpm_array[4];
      rpm_array[4] = 60*(1000000/(time*7));    
    //Last 5 Average RPM Counts Eqauls....
      rpm = (rpm_array[0] + rpm_array[1] + rpm_array[2] + rpm_array[3] + rpm_array[4]) / 5;
  }
 
 }
}

//Capture The IR Break-Beam Interrupt
void motor_interrupt()
{
   time = (micros() - time_last); 
   time_last = micros();
}
