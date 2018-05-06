
#include "bluetooth.hpp"

static int    i;
static char   c;
static String msg_buf;
static int    BT_flag;    // Indicates that a new message is available
static String BT_msg;     // The String containing the new message

/* ----------- Example functions -------------------------------------------- */
void    BT_example () {
  if (BT_available()) {
    Serial.println("New msg!");
    Serial.println(BT_read());
  }
}

void    BT_example2 () {
  static String str = "";
  char          c;

  if (Serial.available()) {
    c = (char)Serial.read();
    str.concat(c);
    if (c == 10) {
      BT_write(str);
      str = "";
    }
  }
}

/* ----------- Bluetooth API ------------------------------------------------ */
int     BT_available () {
  return BT_flag;
}

String  BT_read () {
  BT_flag = 0;
  return BT_msg;
}
void    BT_init() {
  pinMode(PIN_BT_RX, INPUT);
  pinMode(PIN_BT_TX, OUTPUT);
  Serial1.begin(9600);
  BT_msg = "";
  BT_flag = 0;
}

void    BT_write(String msg) {
  Serial1.print(msg);
}

void    BT_loop () {
  // If a received message hasn't been read already, ignore input
  if (BT_flag) return;

  if (Serial1.available()) {    // If a new char is available
    c = (char)Serial1.read();   // Retrieve it
    msg_buf.concat(c);          // Add it to the message buffer
    if (c == BT_MSG_END_CHAR) { // If this is the message end
      BT_msg = msg_buf;
      BT_flag = 1;
      msg_buf = "";
    }
  }
}
