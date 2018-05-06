

#include "dcmotor.hpp"
#include "bluetooth.hpp"

void setup() {
  DC_Init();
  BT_init();
  Serial.begin(9600);
}

void loop() {

  BT_loop();
  //BT_example();
  //BT_example2();

  DC_loop();
  //example1();

  /* --------- Command the wheels from bluetooth ------ */
  String  str;
  if (BT_available()) {
    str = BT_read();
    switch (str.charAt(0)) {
      case 'l' :
        DC_left();
        break;
      case 'r' :
        DC_right();
        break;
      case 'f' :
        DC_forward();
        break;
      case 'b' :
        DC_backward();
        break;
      case 's' :
        DC_stop();
        break;
    }
  }
}
