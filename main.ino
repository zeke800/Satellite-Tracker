#include <Stepper.h>
// Azimuth stepper pins //
#define AZmotorPin1  9       // IN1 on the ULN2003 driver
#define AZmotorPin2  10      // IN2 on the ULN2003 driver
#define AZmotorPin3  11      // IN3 on the ULN2003 driver
#define AZmotorPin4  12      // IN4 on the ULN2003 driver
// Elevation stepper pins //
#define ELmotorPin1  2  
#define ELmotorPin2  3      
#define ELmotorPin3  4     
#define ELmotorPin4  5 
int posaz = 0;
int posel = 0; //We assume we are at 0 el and pointing north.  
int fullrotation = 512;
int stepsperdeg = round(fullrotation/360);
Stepper az(fullrotation, AZmotorPin1, AZmotorPin2, AZmotorPin3, AZmotorPin4);  
Stepper el(fullrotation, ELmotorPin1, ELmotorPin2, ELmotorPin3, ELmotorPin4);  
void setup() {
  // put your setup code here, to run once:
  /*
  pinMode(AZmotorPin1, OUTPUT);
  pinMode(AZmotorPin2, OUTPUT);
  pinMode(AZmotorPin3, OUTPUT);
  pinMode(AZmotorPin4, OUTPUT);
  pinMode(ELmotorPin1, OUTPUT);
  pinMode(ELmotorPin2, OUTPUT);
  pinMode(ELmotorPin3, OUTPUT);
  pinMode(ELmotorPin4, OUTPUT);
  */
  while (!Serial);
  
  Serial.begin(9600);
  az.setSpeed(20);
  el.setSpeed(20);
 
  Serial.print("READY");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (posaz == 360){
    posaz = 0;
  }
  if (Serial.available()){
    String azm  = Serial.readStringUntil(',');
    Serial.read(); //next character is comma, so skip it using this
    String elv = Serial.readStringUntil(',');
     Serial.read();
    Serial.print("AZ: "+azm+" EL: "+elv);
    int finalaz= round(azm.toInt())-posaz;
    int finalel = round(elv.toInt())-posel;
    int azmult = round(finalaz*stepsperdeg);
    int elmult = round(finalel*stepsperdeg);
    az.step(azmult);
    Serial.print("Stepping: "+String(azmult));
    el.step(elmult);
    Serial.print("Stepping: "+String(elmult));
    posaz = posaz+finalaz;
    posel = posel+finalel;
    
  }
  
}
