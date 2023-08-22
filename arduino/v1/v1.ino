/*
nano v3 328P

dependency:
StepperDriver by Laurentiu Badea https://github.com/laurb9/StepperDriver

To-do:
get to required position always from the left
*/
#include <AccelStepper.h>

//steppers
#define STEPS_PER_REVO 200
#define DIR_1 1
#define STEP_1 2
#define DIR_2 0
#define STEP_2 3

AccelStepper stepper(AccelStepper::DRIVER, STEP_1, DIR_1);

void setup() {
  stepper.setMaxSpeed(300);
  stepper.setAcceleration(150);  
}

void loop() {
  
  int x_speed = analogRead(A7)-512;
  
  if(abs(x_speed) < 100){
    x_speed = 0;
  }else{
    x_speed -= 100*abs(x_speed)/x_speed;
    stepper.moveTo(x_speed*0.5 + stepper.currentPosition());
  }
  stepper.run();

}
