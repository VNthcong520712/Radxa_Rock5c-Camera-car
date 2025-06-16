#include <Arduino.h>
#include <CNCShield.h>
#include <Servo.h>
#include <AccelStepper.h>

// ----- CONFIGS -----
#define W 185 				// Car length
#define L 180				// Car width 
#define steer 9				// Servo pin

const int speed_min = 200;	
const int speed_lim = 1200;	
const int turn_min = 65;	// Adjust for your system
const int turn_max = 130;	// Adjust for your system
float rampStep = 7;  		// Ramp smoothness, smaller is smoother
int curr_speed = speed_min;

// ----- GLOBALS -----
CNCShield cnc_shield;
StepperMotor* motorl = cnc_shield.get_motor(0);  // Left motor, X axis
StepperMotor* motorr = cnc_shield.get_motor(1);  // Right motor, Y axis

Servo wheel;

// Soft ramp
float currentSpeedL = 0;
float targetSpeedL = 0;
float currentSpeedR = 0;
float targetSpeedR = 0;
int dirL = 1, dirR = 1; 

// ----- AccelStepper WRAPPER -----
void forwardstepL() {
  motorl->step(CLOCKWISE);
}
void backwardstepL() {
  motorl->step(COUNTER);
}
AccelStepper stepperL(forwardstepL, backwardstepL);

void forwardstepR() {
  motorr->step(CLOCKWISE);
}
void backwardstepR() {
  motorr->step(COUNTER);
}
AccelStepper stepperR(forwardstepR, backwardstepR);

// ----- SERIAL NON-BLOCKING -----
#define SERIAL_BUFFER_SIZE 64
char serialBuffer[SERIAL_BUFFER_SIZE];
uint8_t serialIndex = 0;

// ----- FUNCTION DECL -----
void processSerialCommand(const char* cmd);
void handleSerial();

void N_(int ste = 90) {
  wheel_adj(ste);
  curr_speed = speed_min;
  cnc_shield.disable();
  targetSpeedL = 0;
  targetSpeedR = 0;
  Serial.println("Stop");
}

// ----- MAIN SETUP -----
void setup() {
  Serial.begin(9600);
  cnc_shield.begin();
  cnc_shield.enable();

  stepperL.setMaxSpeed(speed_lim);
  stepperR.setMaxSpeed(speed_lim);

  N_(90);
  wheel.attach(steer);
  wheel.write(90);
  Serial.println("Ready!");
}

void wheel_adj(int angle){
  if (angle >= 90) {
    angle = map(angle, 90, 115, 90, turn_max);
  }
  
  wheel.write(angle);
}

// ----- STOP & PARK -----
void P_(int ste = 90) {
  curr_speed = speed_min;
  wheel.write(90);
  cnc_shield.enable();
  targetSpeedL = 0;
  targetSpeedR = 0;
  Serial.println("Parking");
}

// ----- RAMP AND CONTROL -----
void setMotorSpeeds(int base_speed, int steering_deg, int dL, int dR) {
  dirL = dL;
  dirR = dR;

  // IF GO AHEAD
  if (abs(steering_deg - 90) < 3) {
    targetSpeedL = dirL * base_speed;
    targetSpeedR = dirR * base_speed;
    return;
  }

  // IF TURN
  float theta = float(steering_deg - 90) * PI / 180.0;
  float R = L / tan(theta);
  float r_right = R + W / 2;
  float r_left = R - W / 2;

  if (abs(R) < 1e-3) {
    targetSpeedL = dirL * base_speed;
    targetSpeedR = dirR * base_speed;
    return;
  }

  int left_speed = int(base_speed * (r_left / R));
  int right_speed = int(base_speed * (r_right / R));
  left_speed = constrain(left_speed, 0, speed_lim);
  right_speed = constrain(right_speed, 0, speed_lim);
  Serial.print(left_speed);
  Serial.print("-");
  Serial.println(right_speed);

  targetSpeedL = dirL * left_speed;
  targetSpeedR = dirR * right_speed;
}

void goahead(int speed, int steering) {
  if (speed == 0) {
    N_();
    curr_speed = 0;
    return;
  }
  if (speed < 0) speed = curr_speed;
  if (steering <= 0) steering = 90;

  speed = constrain(speed, speed_min, speed_lim);
  steering = constrain(steering, 90-25, 90+25);

  cnc_shield.enable();
  Serial.print("Go ahead: ");
  Serial.print(speed);
  Serial.print(", steering: ");
  Serial.println(steering);

  wheel.write(steering);
  curr_speed = speed;
  setMotorSpeeds(speed, steering, 1, 1);
}

void goback(int speed, int steering) {
  if (speed == 0) {
    N_();
    curr_speed = 0;
    return;
  }
  if (speed < 0) speed = curr_speed;
  if (steering <= 0) steering = 90;

  speed = constrain(speed, speed_min, speed_lim);
  steering = constrain(steering, turn_min, turn_max);

  cnc_shield.enable();
  Serial.print("Go back: ");
  Serial.print(speed);
  Serial.print(", steering: ");
  Serial.println(steering);

  wheel.write(steering);
  curr_speed = speed;
  setMotorSpeeds(speed, steering, -1, -1);
}

// ----- SERIAL NON-BLOCKING -----
void handleSerial() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (serialIndex > 0) {
        serialBuffer[serialIndex] = '\0';
        processSerialCommand(serialBuffer);
        serialIndex = 0;
      }
    } else {
      if (serialIndex < SERIAL_BUFFER_SIZE - 1) {
        serialBuffer[serialIndex++] = inChar;
      } else {
        serialIndex = 0;  // too long, reset
      }
    }
  }
}

void processSerialCommand(const char* cmd) {
  char type = 0;
  int speed = -1, steering = 0;
  int n = sscanf(cmd, "%c %d %d", &type, &speed, &steering);

  if (n < 1) return;
  type = toupper(type);
  if (n < 2) speed = -1;
  if (n < 3) steering = 90;

  if (type == 'T') {
    speed = map(speed, 0, 100, speed_min, speed_lim);
    if (abs(curr_speed - speed) > 300) {
      Serial.println("Set speed exceed 30%");
    } else {
      goahead(speed, steering);
    }
  } else if (type == 'L') {
    speed = map(speed, 0, 100, speed_min, speed_lim);
    if (abs(curr_speed - speed) > 300) {
      Serial.println("Set speed exceed 30%");
    } else {
      goback(speed, steering);
    }
  } else if (type == 'N') {
    N_(steering);
  } else if (type == 'P') {
    P_(steering);
  }
}

// ----- LOOP -----
void loop() {
  handleSerial();

  // Soft ramp for left motor
  if (abs(currentSpeedL - targetSpeedL) > rampStep) {
    if (currentSpeedL < targetSpeedL) currentSpeedL += rampStep;
    else currentSpeedL -= rampStep;
  } else {
    currentSpeedL = targetSpeedL;
  }

  // Soft ramp for right motor
  if (abs(currentSpeedR - targetSpeedR) > rampStep) {
    if (currentSpeedR < targetSpeedR) currentSpeedR += rampStep;
    else currentSpeedR -= rampStep;
  } else {
    currentSpeedR = targetSpeedR;
  }

  stepperL.setSpeed(currentSpeedL);
  stepperR.setSpeed(currentSpeedR);

  stepperL.runSpeed();
  stepperR.runSpeed();
}