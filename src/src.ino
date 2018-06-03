

#include "dcmotor.hpp"
#include "bluetooth.hpp"
#include "sensor.hpp"

void setup() {
  DC_init();
  BT_init();
  US_init();
  Serial.begin(9600);
}

void loop() {

  BT_loop();
  DC_loop();

  /* --------- Command the wheels from bluetooth ------ */
  String  str;
  char    cmd;
  long    nbr;
  if (BT_available()) {
    /* Decode message */
    str = BT_read();
    cmd = str.charAt(0);
    str.remove(0, 1);
    nbr = str.toInt();

    switch (cmd) {
      /* Command the wheels */
      case 'l' :
        DC_dstop(nbr);
        DC_left();
        break;
      case 'r' :
        DC_dstop(nbr);
        DC_right();
        break;
      case 'f' :
        DC_dstop(DC_distance_to_duration(nbr));
        DC_forward();
        break;
      case 'b' :
        DC_dstop(DC_distance_to_duration(nbr));
        DC_backward();
        break;
      case 's' :
        DC_stop();
        break;
      case 'k' :
        US_read();
        break;
      case 'i' :
        US_set_angle(nbr);
        break;
    }
  }
}
