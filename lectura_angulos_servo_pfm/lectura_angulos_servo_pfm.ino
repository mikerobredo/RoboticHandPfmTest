//USAMOS PULSEIN POR FUERA
int input =10;
unsigned long duration,grados,duration3;
#include <ServPfm.h>

ServPfm myservo;  
int pos = 0;  

void setup() {
  Serial.begin(9600);
  pinMode(10 , input);
  myservo.attachPfm(9);
}

void loop() {

  int cont =0
  for (pos = 0; pos <= 180; pos += 1) {
  myservo.writePfm(pos);              
  duration = pulseIn(10,HIGH);
  delay (10);
  duration3 = pulseIn(10,LOW);
  grados =map(duration, 544, 2400, 0, 180);
  Serial.println("enalta");
  Serial.println(duration);
  Serial.println(duration3);
  if (duration3<2400)
  {Serial.println("ok");
    
    }
  }
  
  
  for (pos = 180; pos >= 0; pos -= 1) {
  myservo.writePfm(pos);              
  duration = pulseIn(10,HIGH);
  delay (10);
  duration3 = pulseIn(10,LOW);
  grados =map(duration, 544, 2400, 0, 180);
  Serial.println("enalta");
  Serial.println(duration);
  Serial.println(duration3);
  if (duration3<2400)
  Serial.println("ok");
  
  }
}
