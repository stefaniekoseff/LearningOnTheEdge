// teachable machines audio input drives 2 servo motors 


#include <Servo.h>
int input; // variable to hold incoming data
int pos = 0;
Servo myservo1;
Servo myservo2; 


void setup() {
    
    pinMode(led, OUTPUT);
     myservo1.attach(9);
     myservo2.attach(10);
    // start serial
  Serial.begin(9600);
}

void loop() {
  // if something is in the buffer
  if (Serial.available() > 0) {
    input = Serial.read(); // read it
  }
  if (input == 2) {       //tree
    myservo1.write(0);
  }
    if (input == 4) {       // cat
      myservo1.write(180);
    }
      if (input == 3) {       // lady
         myservo2.write(0);
      } 
      if (input == 5) {         //banana 
         myservo2.write(180);
      }

    // if (input == 0) {
    //     myservo1.write(90);
    //     myservo2.write(90);
    //   }
  
  delay(2);
}
