
#include "dcmotor.hpp"

void setup() {
  DC_Init();
  Serial.begin(9600);
}

void loop() {
  example1();
  DC_loop();
}

