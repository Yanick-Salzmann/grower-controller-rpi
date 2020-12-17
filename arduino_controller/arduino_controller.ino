#include <Arduino.h>
#include <Servo.h>

enum class output_commands {
  analog_read_value,
  digital_read_value,
  current_value
};

enum class serial_command {
  pwm_set_value,
  digital_set_value,
  digital_read_value,
  digital_set_mode,
  analog_set_value,
  analog_read_value,
  servo_set_angle
};

Servo cam_servo0;
Servo cam_servo1;

void setup() {
  Serial.begin(115200);
  while(!Serial);

  cam_servo0.attach(10);
  cam_servo1.attach(11);
  cam_servo0.write(90);
  cam_servo1.write(90);

  analogWrite(3, 0);
  analogWrite(5, 0);
  analogWrite(6, 0);
  analogWrite(9, 0);
}

void read_serial_command() {
  int cmd = Serial.read();
  int pin = Serial.read();
  int val0 = Serial.read();
  int val1 = Serial.read();
  
  switch(cmd) {
    case (int) serial_command::analog_set_value:
    case (int) serial_command::pwm_set_value:
      analogWrite(A0 + pin, val0);
      break;

    case (int) serial_command::digital_set_mode:
      pinMode(pin, val0 == 0 ? INPUT : OUTPUT);
      break;

    case (int) serial_command::digital_set_value:
      digitalWrite(pin, val0 == 0 ? LOW : HIGH);
      break;

    case (int) serial_command::analog_read_value: {
      int val = analogRead(pin);
      Serial.write((byte) output_commands::analog_read_value);
      Serial.write((byte) pin);
      Serial.write((byte) val0);
      Serial.write((byte) val1);
      Serial.write((byte) (val & 0xFF));
      Serial.write((byte) (val >> 8) & 0xFF);
      Serial.println();
      break;  
    }

    case (int) serial_command::digital_read_value: {
      int val = digitalRead(pin) == LOW ? 0 : 1;
      Serial.write((byte) output_commands::digital_read_value);
      Serial.write((byte) pin);
      Serial.write((byte) val0);
      Serial.write((byte) val1);
      Serial.write((byte) val);
      Serial.println();
      break;
    }

    case (int) serial_command::servo_set_angle: {
      int angle = min(max(20, val0), 160);
      if(pin == 0) {
        cam_servo0.write(angle);
      } else if(pin == 1) {
        cam_servo1.write(angle);
      }
      break;
    }
  }
}

void loop() {
  for(int i = 4; i < 6; ++i) {
    Serial.write((byte) output_commands::current_value);
    Serial.print(String(i));
    Serial.print(" ");
    Serial.println(String(analogRead(A0 + i)));
  }

  while(Serial.available() >= 4) {
    read_serial_command();
  }

  delay(1);
}
