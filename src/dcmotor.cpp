
#include "dcmotor.hpp"

static float  DC_LeftMotorSpeed;
static float  DC_RightMotorSpeed;
static float  DC_FrontWheelAngle;   /* Between -90 (left) to +90 (right) */

static int    DC_msg_flag;          /* Indicates whether there's a new message
                                       for DC_loop() to handle */
static char   DC_msg;               /* The message to be treated by DC_loop() */

static Servo  myservo;

/* -------------------------------------------------------------------------- */
/* ----------- Example functions -------------------------------------------- */
/* -------------------------------------------------------------------------- */

/**
 * Control the vehicule from the USB serial
 */
void    example1() {
  if (Serial.available()) {
    switch (Serial.read()) {
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

/* -------------------------------------------------------------------------- */
/* ----------- API 2 : Command the vehicle move ----------------------------- */
/* -------------------------------------------------------------------------- */

/**
 * The five following functions send a move commmand for DC_loop() to treat
 */
void    DC_forward()
{
  DC_msg_flag = 1;
  DC_msg = 'f';
}

void    DC_backward()
{
  DC_msg_flag = 1;
  DC_msg = 'b';
}

void    DC_left()
{
  DC_msg_flag = 1;
  DC_msg = 'l';
}

void    DC_right()
{
  DC_msg_flag = 1;
  DC_msg = 'r';
}

void    DC_stop()
{
  DC_msg_flag = 1;
  DC_msg = 's';
}

/**
 * This function constantly listens to move commands received through
 * the DC_msg_flag and DC_msg variables, and treats these commands.
 * 
 * N.B. This function should be called in the main loop() function.
 */
void DC_loop ()
{
  static int    state = DCSTATE_WAITCMD;
  static float  angleCmd;
  static int    leftSpeedCmd;
  static int    rightSpeedCmd;
  static int    angle;
         int    value;
  
  switch (state) {
    /* ============== WAITCMD STATE ================ */
    case DCSTATE_WAITCMD :
      /* If no message, keep waiting... */
      if (not DC_msg_flag)   break;
    
      /* If message received, set the commande accordingly */
      DC_msg_flag = 0;  // acknowledge the message reception
      value = analogRead(PIN_POT);
      switch (DC_msg) {
        case 'l' :
          angleCmd = 90;
          leftSpeedCmd = - value / 1024.0 * 255 / 2;
          rightSpeedCmd = + value / 1024.0 * 255 / 2;
          break;
        case 'r' :
          angleCmd = 90;
          leftSpeedCmd = + value / 1024.0 * 255 / 2;
          rightSpeedCmd = - value / 1024.0 * 255 / 2;
          break;
        case 'f' :
          angleCmd = 0;
          leftSpeedCmd = + value / 1024.0 * 255 / 2;
          rightSpeedCmd = + value / 1024.0 * 255 / 2;
          break;
        case 'b' :
          angleCmd = 0;
          leftSpeedCmd = - value / 1024.0 * 255 / 2;
          rightSpeedCmd = - value / 1024.0 * 255 / 2;
          break;
        case 's' :
          angleCmd = angle;
          leftSpeedCmd = 0;
          rightSpeedCmd = 0;
          break;
      }
      /* Then start turning the front wheel... */
      state = DCSTATE_TURNWHEEL;
      break;

    /* ============== TURNWHEEL STATE ============== */
    case DCSTATE_TURNWHEEL :
      /* While the front wheel is not correctly oriented */
      if (angle != angleCmd) {
        /* Move the angle one step */
        angle += angle < angleCmd ? DC_TURNPACE : -DC_TURNPACE;
        /* readjust angle to angleCmd if they were close enough */
        if (abs(angleCmd - angle) < DC_TURNPACE)   angle = angleCmd;
        /* Validate the changes */
        DC_SetWheelAngle(angle);
        DC_Refresh();
        state = DCSTATE_TURNWHEEL_DELAY;
        break;
      }
      /* If the front wheel is corectly oriented */
      /* Move the back wheels */
      DC_SetLeftSpeed(leftSpeedCmd);
      DC_SetRightSpeed(rightSpeedCmd);
      DC_Refresh();
      /* The request is now fullfilled, go back to idle state */
      state = DCSTATE_WAITCMD;
      break;
      
    /* ============== TURNWHEEL DELAY STATE ======== */
    case DCSTATE_TURNWHEEL_DELAY :
      if (!wait_for(DC_TURNDELAY)) break;
      state = DCSTATE_TURNWHEEL;
      break;
  }
}

/* -------------------------------------------------------------------------- */
/* ----------- API 1 : Direct control over the DC Motor speed --------------- */
/* -------------------------------------------------------------------------- */

/**
 * This functions activates the DC motors, it should be called once in setup()
 * 
 * @return          - none
 */
void    DC_Init ()
{
  pinMode(PIN_EN12, OUTPUT);
  pinMode(PIN_1A, OUTPUT);
  pinMode(PIN_2A, OUTPUT);
  pinMode(PIN_EN34, OUTPUT);
  pinMode(PIN_3A, OUTPUT);
  pinMode(PIN_4A, OUTPUT);
  myservo.attach(9);

  DC_LeftMotorSpeed = 0;
  DC_RightMotorSpeed = 0;
  DC_FrontWheelAngle = 0;
  DC_Refresh();
}

/**
 * This functions sets the speed of the left motor
 * 
 * @return          - -1 if error, 0 otherwise
 */
int     DC_SetLeftSpeed(int param_speed)
{
  if (abs(param_speed) > DC_MAX_SPEED)
    return -1;
  DC_LeftMotorSpeed = param_speed;
  return 0;
}

/**
 * This functions sets the speed of the right motor
 * 
 * @return          - -1 if error, 0 otherwise
 */
int     DC_SetRightSpeed(int param_speed)
{
  if (abs(param_speed) > DC_MAX_SPEED)
    return -1;
  DC_RightMotorSpeed = param_speed;
  return 0;
}

/**
 * This functions sets the orientation of the front wheel
 * 
 * @return          - -1 if error, 0 otherwise
 */
int     DC_SetWheelAngle(int angle)
{
  if (abs(angle) > DC_MAX_ANGLE)
    return -1;
  DC_FrontWheelAngle = angle;
  return 0;
}

/**
 * This function updates the current values written on the Arduino output pins
 * One should call it when one wants the changes made by calling the above
 * functions to be effective.
 * 
 * @return        - none
 */
void    DC_Refresh ()
{
  int   val_en12;
  int   val_1a;
  int   val_2a;
  int   val_en34;
  int   val_3a;
  int   val_4a;
  int   val_servo;
  
  /* Right Wheel */
  val_en12 = HIGH;
  val_1a = 0;
  val_2a = 0;
  if (DC_RightMotorSpeed > 0)
    val_1a = abs(DC_RightMotorSpeed);
  else
    val_2a = abs(DC_RightMotorSpeed);

  /* Left Wheel */
  val_en34 = HIGH;
  val_3a = 0;
  val_4a = 0;
  if (DC_LeftMotorSpeed > 0)
    val_4a = abs(DC_LeftMotorSpeed);
  else
    val_3a = abs(DC_LeftMotorSpeed);

  /* Front wheel */
  val_servo = DC_FrontWheelAngle + 90;

  /* Final write */
  digitalWrite(PIN_EN12, val_en12);
  analogWrite(PIN_1A, val_1a);
  analogWrite(PIN_2A, val_2a);
  digitalWrite(PIN_EN34, val_en34);
  analogWrite(PIN_3A, val_3a);
  analogWrite(PIN_4A, val_4a);
  myservo.write(val_servo);
}

