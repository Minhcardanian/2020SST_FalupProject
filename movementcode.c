#include<Servo.h>

enum class Status {
  IDLE,
  EXTENDING,
  EXTENDED,
  RETRACTING
};

class NetServo {
  private:
    Servo servo;
    
    int idleAngle;
    int extendedAngle;
    int pin;
  public:
    NetServo() {}
  
    NetServo(int pin, int idleAngle, int extendedAngle) {
      servo = Servo();
      servo.attach(pin);
      this->idleAngle = idleAngle;
      this->extendedAngle = extendedAngle;
  
      retract();
    }
  
    void extend() {
      servo.write(extendedAngle);
    }
  
    void retract() {
      servo.write(idleAngle);
    }
};

// element pins:
const int sensorPin = 1; // IR sensor pin
const int leftWallServoPin = 9;
const int rightWallServoPin = 10;


// angles:
const int wallServoIdleAngle = 97;
const int wallServoExtendedAngle = 97;


// times:
const int retractCoolDown = 10000; // milliseconds
const int servoMovementWaitTime = 20; // milliseconds
const int timeBetweenSamples = 2; //milliseconds

Status status;
NetServo leftWallServo;
NetServo rightWallServo;

int currentCoolDown = 0;

void setup() {
  Serial.begin(9600);
  Status status = Status::IDLE;
  pinMode(sensorPin, INPUT);

  leftWallServo = NetServo(leftWallServoPin, wallServoIdleAngle, wallServoExtendedAngle);
  rightWallServo = NetServo(rightWallServoPin, wallServoIdleAngle, wallServoExtendedAngle);
 
}

void loop() {
  switch (status)
  {
    case Status::IDLE:
      if (hasFoundObject()) {
        extendServos();
        status = Status::EXTENDING;
      }
      break;
    case Status::EXTENDING:
      delay(servoMovementWaitTime);
      currentCoolDown = retractCoolDown;

      status = Status::EXTENDED;
      break;
    case Status::EXTENDED:
      if (hasFoundObject()) {
        currentCoolDown = retractCoolDown;
      } else {
        currentCoolDown -= timeBetweenSamples;
      }

      if (currentCoolDown <= 0) {
        retractServos();
      }
      break;
    case Status::RETRACTING:
      delay(servoMovementWaitTime);
      status = Status::IDLE;
      break;
  }

  delay(timeBetweenSamples);
}

void extendServos() {
  leftWallServo.extend();
  rightWallServo.extend();
 
}

void retractServos() {
  leftWallServo.retract();
  rightWallServo.retract();

}

bool hasFoundObject() {
  return digitalRead(sensorPin);
}

