
int input =10;
unsigned long duration,duration2,duration3;
#include <Servo.h>

Servo myservo;  
int pos = 0;  

void setup() {
  Serial.begin(9600);
  Serial.println("intruzca los numeros");
  pinMode(10 , input);
  myservo.attach(9);
}

void loop() {

  for (pos = 0; pos <= 180; pos += 1) {
  myservo.write(pos);             
  duration = pulseIn(10,HIGH);
  duration3 = pulseIn(10,LOW);
  duration2 =map(duration, 544, 2400, 0, 180);
  Serial.println(duration2);
  Serial.println(duration3);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
  myservo.write(pos);              
  duration = pulseIn(10,HIGH);
  duration3 = pulseIn(10,LOW);
  duration2 =map(duration, 544, 2400, 0, 180);
  Serial.println(duration2);
  Serial.println(duration3);
  }
}
