#include <AccelStepper.h>

// Define two steppers and the pins they will use
AccelStepper stepper1(AccelStepper::DRIVER, 9, 8);
AccelStepper stepper2(AccelStepper::DRIVER, 7, 5);

int pos1 = 2000;
int pos2 = 4000;

void setup()
{  
  stepper1.setMaxSpeed(75);
  stepper1.setAcceleration(5);
  stepper2.setMaxSpeed(75);
  stepper2.setAcceleration(5);
}

void loop()
{
  if (stepper1.distanceToGo() == 0)
  {
     pos1 = -pos1; 
    stepper1.moveTo(pos1);
  }
  if (stepper2.distanceToGo() == 0)
  {
    pos2 = -pos2;
    stepper2.moveTo(pos2);
  }
  stepper1.run();
  stepper2.run();
}
