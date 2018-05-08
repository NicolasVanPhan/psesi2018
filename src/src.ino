

#include "dcmotor.hpp"
#include "bluetooth.hpp"

void setup() {
  DC_init();
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
  char    cmd;
  long    duration;
  if (BT_available()) {
    str = BT_read();
    Serial.print(str);
    cmd = str.charAt(0);
    str.remove(0, 1);
    Serial.println(str);
    duration = str.toInt();
    Serial.println(duration);
    DC_dstop(duration);
    switch (cmd) {
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
