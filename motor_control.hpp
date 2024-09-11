#pragma once

class Motor {
  const int forwardPin;
  const int reversePin;

public:
  Motor(int forwardPin, int reversePin)
      : forwardPin(forwardPin), reversePin(reversePin) {}

  void init() const;
  void drive(float speed) const;
};

class CurvatureDrive {
  const Motor leftMotor;
  const Motor rightMotor;

  const int frontBackChannel;
  const int leftRightChannel;
  const int throttleChannel;
  const int turnInPlaceChannel;

public:
  CurvatureDrive(Motor leftMotor, Motor rightMotor, int frontBackChannel,
                 int leftRightChannel, int throttleChannel,
                 int turnInPlaceChannel)
      : leftMotor(leftMotor), rightMotor(rightMotor),
        frontBackChannel(frontBackChannel), leftRightChannel(leftRightChannel),
        throttleChannel(throttleChannel),
        turnInPlaceChannel(turnInPlaceChannel) {}
  void init() const;
  void drive() const;
};
