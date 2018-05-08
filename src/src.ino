

#include "dcmotor.hpp"
#include "bluetooth.hpp"

void setup() {
  DC_init();
  BT_init();
  Serial.begin(9600);
}

void loop() {

  BT_loop();
  DC_loop();

  /* --------- Command the wheels from bluetooth ------ */
  String  str;
  char    cmd;
  long    duration;
  if (BT_available()) {
    /* Decode message */
    str = BT_read();
    cmd = str.charAt(0);
    str.remove(0, 1);
    duration = str.toInt();
    /* Command the wheels */
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
