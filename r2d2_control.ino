#include <IBusBM.h>

IBusBM ibus;

// dome has 5/6 as they have slightly different performance, as per Arduino docs
int domeForwardPin = 5;
int domeBackwardPin = 6;
int leftLegForwardPin = 3;
int leftLegBackwardPin = 9;
int rightLegForwardPin = 10;
int rightLegBackwardPin = 11;

void setup() {
  Serial.begin(115200);

  ibus.begin(Serial);

  pinMode(domeForwardPin, OUTPUT);
  pinMode(domeBackwardPin, OUTPUT);
  pinMode(leftLegForwardPin, OUTPUT);
  pinMode(leftLegBackwardPin, OUTPUT);
  pinMode(rightLegForwardPin, OUTPUT);
  pinMode(rightLegBackwardPin, OUTPUT);
}

void updateChannel(int channel, int forward_pin, int reverse_pin) {
  int value = ibus.readChannel(channel);
  // if values are outside of range, stop motor
  if (value < 1000 || value > 2000) {
    analogWrite(forward_pin, 0);
    analogWrite(reverse_pin, 0);
    return;
  }
  int scaled_value = float(value - 1500) / 1000 * (255 * 2);

  Serial.print("Channel ");
  Serial.print(channel);
  Serial.print(": ");
  Serial.print(value);
  Serial.print(" scaled: ");
  Serial.println(scaled_value);

  if (scaled_value < 0) {
    analogWrite(forward_pin, 0);
    analogWrite(reverse_pin, -scaled_value);
  } else {
    analogWrite(forward_pin, scaled_value);
    analogWrite(reverse_pin, 0);
  }
}

void loop() {
  updateChannel(0, leftLegForwardPin, leftLegBackwardPin);
  updateChannel(1, rightLegForwardPin, rightLegBackwardPin);
  updateChannel(2, domeForwardPin, domeBackwardPin);

  delay(10);
}
