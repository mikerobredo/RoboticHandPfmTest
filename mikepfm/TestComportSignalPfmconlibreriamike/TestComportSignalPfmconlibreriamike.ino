
#include <ServPfm.h>
int input =10;
unsigned long duration,duration2,duration3;


ServPfm myservo2,myservo;  
int pos = 0;  

void setup() {
  Serial.begin(9600);  
  pinMode(10 , input);
  myservo2.attachPfm(9);
  myservo.attachPfm(11);
  
}

void loop() {

  for (pos = 0; pos <= 180; pos += 1) { 
   
  myservo2.writePfm(pos);             
  //duration = pulseIn(10,HIGH);
  duration=myservo2.readPfm(11,1000,2000,10); 
  Serial.println(duration);
  delay(0);

  /*
  duration3 = pulseIn(10,LOW);
  duration2 =map(duration3, 1000, 2000, 0, 180);
  //Serial.println(duration);
  if(duration2<=200)
  {
    Serial.println(duration2);
    }
  delay(50);
  */
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
  myservo2.writePfm(pos); 
  duration=myservo2.readPfm(11,1000,2000,10); 
  Serial.println(duration);
  delay(15);  
   /*       
  //duration = pulseIn(10,HIGH);
  duration3 = pulseIn(10,LOW);
  duration2 =map(duration3, 1000, 2000, 0, 180);
  //Serial.println(duration);
    if(duration2<=200)
      {
        Serial.println(duration2);
      }  */
  }
 }
