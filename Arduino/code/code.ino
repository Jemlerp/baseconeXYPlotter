#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

AccelStepper stepper1 (AccelStepper::DRIVER, 2, 5);
AccelStepper stepper2(AccelStepper::DRIVER, 3, 6);
MultiStepper steppers;

Servo servo;

int servoCurPos = 0; //is 90

void setup() {
  Serial.begin(9600);  
  
  servo.attach(11);
  servo.write(90);

  stepper1.setMaxSpeed(3200);
  stepper2.setMaxSpeed(3200);
  
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);
}

void loop(){
                  // x y s
  bool nextFor[3] = {0,0,0};
  String commandX = "";
  String commandY = "";
  String commandS = "";   

  if(Serial.available() > 0){
    char c;
    do{
      delay(4); // wait for serial buffer
      c = Serial.read();      
      if(c == 'x' || c == 'y' || c == 's'){
        nextFor[0] = 0;
        nextFor[1] = 0;
        nextFor[2] = 0;
        switch (c){
          case 'x':
            nextFor[0] = 1;
          break;
          case 'y':
            nextFor[1] = 1;
          break;
          case 's':
            nextFor[2] = 1;
          break;
        }
      }else{
        if(nextFor[0]){ commandX += c; }
        if(nextFor[1]){ commandY += c; }
        if(nextFor[2]){ commandS += c; }
      }
    } while(c != '\n'); 

    long moveXYS[3] = {0,0,servoCurPos};
    
    if(commandX.length() > 0){
      moveXYS[0] = commandX.toInt() * -1;
    }
    if(commandY.length() > 0){
      moveXYS[1] = commandY.toInt() * -1;
    }
    if(commandS.length() > 0){
      moveXYS[2] = commandS.toInt();
    }

    move(moveXYS);
    Serial.println("Done");
  }  
}

void move(long values[3]){
  //servo
  while(values[2] != servoCurPos){
    if(values[2] < servoCurPos){
      if(servoCurPos == -50){ break; }
      servo.write((int)map(servoCurPos - 1, -50, 50, 40, 140));
      servoCurPos--;
    }else{
      if(servoCurPos == 50){ break; }
      servo.write((int)map(servoCurPos + 1, -50, 50, 40, 140));
      servoCurPos++;
    }
    delay(15); // wait for servo
  }

  //stepper //een stepper rotatie = ~5cm beweging = 72? 79 steps per cm
  //int_8_t overflow somewhere....
  long noIntOverflow[2] = {0,0};
  if(abs(values[0]) > 400){
    values[0] = values[0] > 0 ? values[0] -= 200 : values[0] += 200;
    noIntOverflow[0] = values[0] > 0 ? 200 : -200;
  }
  if(abs(values[1]) > 400){
    values[1] = values[1] > 0 ? values[1] -= 200 : values[1] += 200;
    noIntOverflow[1] = values[1] > 0 ? 200 : -200;
  }
  if(noIntOverflow[0] != 0 || noIntOverflow[1] != 0){
    long weMove[3] = {noIntOverflow[0] * 79, noIntOverflow[1] * 79};
    steppers.move(weMove);
    //steppers.move((long[]) {noIntOverflow[0] * 79, noIntOverflow[1] * 79}));
    steppers.runSpeedToPosition();
  }

  //steppers.move((long[]) {values[0] * 79, values[1] * 79}));
  long weMove[3] = {values[0] * 79, values[1] * 79};
  steppers.move(weMove);
  steppers.runSpeedToPosition();
}
