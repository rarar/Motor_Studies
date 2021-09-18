/*
  This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
  It won't work with v1.x motor shields! Only for the v2's with built in PWM
  control

  For use with the Adafruit Motor Shield v2
  ---->	http://www.adafruit.com/products/1438
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *m0 = AFMS.getMotor(1);
Adafruit_DCMotor *m1 = AFMS.getMotor(2);
Adafruit_DCMotor *m2 = AFMS.getMotor(3);
Adafruit_DCMotor *m3 = AFMS.getMotor(4);
// Pointer to DC_Motors
Adafruit_DCMotor* my_motors[4];
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  my_motors[0] = m0;
  my_motors[1] = m1;
  my_motors[2] = m2;
  my_motors[3] = m3;

  // Set the speed to start, from 0 (off) to 255 (max speed)
  my_motors[0]->setSpeed(random(75, 255));
  my_motors[1]->setSpeed(random(75, 255));
  my_motors[2]->setSpeed(random(75, 255));
  my_motors[3]->setSpeed(random(75, 255));

  my_motors[0]->run(FORWARD);
  my_motors[1]->run(FORWARD);
  my_motors[2]->run(FORWARD);
  my_motors[3]->run(FORWARD);
  // turn on motor
  my_motors[0]->run(RELEASE);
  my_motors[1]->run(RELEASE);
  my_motors[2]->run(RELEASE);
  my_motors[3]->run(RELEASE);
}

void loop() {
  my_motors[0]->setSpeed(random(75, 255));
  my_motors[1]->setSpeed(random(75, 255));
  my_motors[2]->setSpeed(random(75, 255));
  my_motors[3]->setSpeed(random(75, 255));
  my_motors[0]->run(FORWARD);
  my_motors[1]->run(FORWARD);
  my_motors[2]->run(FORWARD);
  my_motors[3]->run(FORWARD);
  delay(random(2000, 10000));
  my_motors[0]->run(RELEASE);
  my_motors[1]->run(RELEASE);
  my_motors[2]->run(RELEASE);
  my_motors[3]->run(RELEASE);
  delay(random(5000, 10000));
  my_motors[0]->setSpeed(random(75, 255));
  my_motors[1]->setSpeed(random(75, 255));
  my_motors[2]->setSpeed(random(75, 255));
  my_motors[3]->setSpeed(random(75, 255));
  my_motors[0]->run(BACKWARD);
  my_motors[1]->run(BACKWARD);
  my_motors[2]->run(BACKWARD);
  my_motors[3]->run(BACKWARD);
  delay(random(2000, 10000));
  my_motors[0]->run(RELEASE);
  my_motors[1]->run(RELEASE);
  my_motors[2]->run(RELEASE);
  my_motors[3]->run(RELEASE);
  delay(random(5000, 10000));

}
