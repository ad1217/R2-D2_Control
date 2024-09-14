#include <IBusBM.h>

#include "motor_control.hpp"

IBusBM ibus;

// HACK: technically designated initializers aren't allowed until c++20
// (but it sure does make it easier to read)
const CurvatureDrive curvatureDrive = {
    .leftMotor = {.forwardPin = 3, .reversePin = 9},
    .rightMotor = {.forwardPin = 10, .reversePin = 11},
    .frontBackChannel = 1 - 1,
    .leftRightChannel = 2 - 1,
    .throttleChannel = 8 - 1,
    .turnInPlaceChannel = 10 - 1,
};

// dome using pins 5 and 6 as they have slightly different performance, as per
// Arduino docs
const Motor domeMotor = {.forwardPin = 5, .reversePin = 6};
const int domeChannel = 4 - 1;

// returns a float in range -1:1
float readChannelSignedScaled(int channel) {
  int value = ibus.readChannel(channel);

  // if values are outside of range, go to 0
  if (value < 1000 || value > 2000) {
    return 0.0;
  }
  return float(value - 1500) / 1000 * 2;
}

// returns a float in range 0:1
float readChannelUnsignedScaled(int channel) {
  int value = ibus.readChannel(channel);

  // if values are outside of range, go to 0
  if (value < 1000 || value > 2000) {
    return 0.0;
  }
  return float(value - 1000) / 1000;
}

void Motor::init() const {
  pinMode(forwardPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
}

void Motor::drive(float speed) const {
  float scaled_speed = speed * 255.0;
  if (scaled_speed < 0) {
    analogWrite(forwardPin, 0);
    analogWrite(reversePin, -scaled_speed);
  } else {
    analogWrite(forwardPin, scaled_speed);
    analogWrite(reversePin, 0);
  }
}

void CurvatureDrive::init() const {
  leftMotor.init();
  rightMotor.init();
}

// based on algorithm from WPILib DifferentialDrive.cpp
// https://github.com/wpilibsuite/allwpilib/blob/main/wpilibc/src/main/native/cpp/drive/DifferentialDrive.cpp
void CurvatureDrive::drive() const {
  float throttle = readChannelUnsignedScaled(throttleChannel);
  float frontBackValue = readChannelSignedScaled(frontBackChannel) * throttle;
  float leftRightValue = readChannelSignedScaled(leftRightChannel) * throttle;
  boolean turnInPlace = ibus.readChannel(turnInPlaceChannel) > 1500;

  Serial.print("Differential drive");
  Serial.print(" throttle: ");
  Serial.print(throttle);
  Serial.print("\tFB: ");
  Serial.print(frontBackValue);
  Serial.print("\tLR: ");
  Serial.print(leftRightValue);
  Serial.print("\tturn?: ");
  Serial.print(turnInPlace);

  double leftSpeed = 0.0;
  double rightSpeed = 0.0;

  if (turnInPlace) {
    leftSpeed = frontBackValue - leftRightValue;
    rightSpeed = frontBackValue + leftRightValue;
  } else {
    leftSpeed = frontBackValue - abs(frontBackValue) * leftRightValue;
    rightSpeed = frontBackValue + abs(frontBackValue) * leftRightValue;
  }

  // Desaturate wheel speeds
  double maxMagnitude = max(abs(leftSpeed), abs(rightSpeed));
  if (maxMagnitude > 1.0) {
    leftSpeed /= maxMagnitude;
    rightSpeed /= maxMagnitude;
  }

  Serial.print("\tleftSpeed: ");
  Serial.print(leftSpeed);
  Serial.print("\trightSpeed: ");
  Serial.println(rightSpeed);

  leftMotor.drive(leftSpeed);
  rightMotor.drive(rightSpeed);
}

void setup() {
  Serial.begin(115200);

  ibus.begin(Serial);

  domeMotor.init();
  curvatureDrive.init();
}

void loop() {
  curvatureDrive.drive();
  domeMotor.drive(readChannelSignedScaled(domeChannel));

  delay(10);
}
