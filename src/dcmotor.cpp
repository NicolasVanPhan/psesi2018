
#include "dcmotor.hpp"

static float  DC_LeftMotorSpeed;
static float  DC_RightMotorSpeed;
static float  DC_FrontWheelAngle;   /* Between -90 (left) to +90 (right) */

static int    DC_msg_flag;          /* Indicates whether there's a new message
                                       for DC_loop() to handle */
static char   DC_msg;               /* The message to be treated by DC_loop() */

static char   DC_fwheel_aligned;    /* = 1 if front wheel is well oriented */
static char   DC_dstop_state;
static char   DC_dstop_flag;
static unsigned long DC_dstop_time; /* When should the car stop */
static unsigned long DC_dstop_delay; /* In how much time from the moment the
                                        front wheel gets aligned should the
                                        car stop */

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
/* ----------- API 1 : Command the vehicle move ----------------------------- */
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
 * This functions activates the DC motors, it should be called once in setup()
 *
 * @return          - none
 */
void    DC_init ()
{
  pinMode(PIN_EN12, OUTPUT);
  pinMode(PIN_1A, OUTPUT);
  pinMode(PIN_2A, OUTPUT);
  pinMode(PIN_EN34, OUTPUT);
  pinMode(PIN_3A, OUTPUT);
  pinMode(PIN_4A, OUTPUT);
  myservo.attach(PIN_FWHEEL);

  DC_LeftMotorSpeed = 0;
  DC_RightMotorSpeed = 0;
  DC_FrontWheelAngle = DC_ANGLE_FWD;
  DC_dstop_flag = 0;
  DC_dstop_state = DC_DSTOP_IDLE;
  DC_Refresh();
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
  static int    angle = 0;
  static int    leftSpeed = 0;
  static int    rightSpeed = 0;
         int    vmax;

  DC_dstop_loop();
  
  switch (state) {
    /* ============== WAITCMD STATE ================ */
    case DCSTATE_WAITCMD :

      /* If no message, keep waiting... */
      if (not DC_msg_flag)   break;
    
      /* If message received, set the commande accordingly */
      DC_msg_flag = 0;  // acknowledge the message reception
      vmax = analogRead(PIN_POT);
      switch (DC_msg) {
        case 'l' :
          if (!DC_car_is_stopped()) break;
          angleCmd = DC_ANGLE_TURN;
          leftSpeedCmd = - vmax / 1023.0 * 255 / DC_ROTSPEED_RATIO;
          rightSpeedCmd = + vmax / 1023.0 * 255 / DC_ROTSPEED_RATIO;
          break;
        case 'r' :
          if (!DC_car_is_stopped()) break;
          angleCmd = DC_ANGLE_TURN;
          leftSpeedCmd = + vmax / 1023.0 * 255 / DC_ROTSPEED_RATIO;
          rightSpeedCmd = - vmax / 1023.0 * 255 / DC_ROTSPEED_RATIO;
          break;
        case 'f' :
          if (!DC_car_is_stopped()) break;
          angleCmd = DC_ANGLE_FWD;
          leftSpeedCmd = + vmax / 1023.0 * 255;
          rightSpeedCmd = + vmax / 1023.0 * 255;
          break;
        case 'b' :
          if (!DC_car_is_stopped()) break;
          angleCmd = DC_ANGLE_FWD;
          leftSpeedCmd = - vmax / 1023.0 * 255;
          rightSpeedCmd = - vmax / 1023.0 * 255;
          break;
        case 's' :
          angleCmd = angle;
          leftSpeedCmd = 0;
          rightSpeedCmd = 0;
          break;
      }
      /* Then start turning the front wheel... */
      DC_fwheel_aligned = 0;
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
      /* Now that the front wheel is correctly oriented, accelerate or decelerate */
      DC_fwheel_aligned = 1;  // Now the front wheel is well aligned
      state = DCSTATE_CHGSPEED;
      break;
      
    /* ============== TURNWHEEL DELAY STATE ======== */
    case DCSTATE_TURNWHEEL_DELAY :
      if (DC_msg_flag) {
        wait_for(DC_ID, -1);
        state = DCSTATE_WAITCMD;
        break;
      }
      if (!wait_for(DC_ID, DC_TURNDELAY)) break;
      state = DCSTATE_TURNWHEEL;
      break;

    /* ============== SPEED CHANGE STATE ======== */
    case DCSTATE_CHGSPEED :
      /* While the current speed is not the speed wanted */
      if (leftSpeed != leftSpeedCmd or rightSpeed != rightSpeedCmd) {
        /* Accelerate/Decelerate by a step */
        leftSpeed += leftSpeed < leftSpeedCmd ? DC_CHGSPEEDPACE : -DC_CHGSPEEDPACE;
        rightSpeed += rightSpeed < rightSpeedCmd ? DC_CHGSPEEDPACE : -DC_CHGSPEEDPACE;
        /* Readjust the speed to speedCmd if they were close enough */
        if (abs(leftSpeedCmd - leftSpeed) < DC_CHGSPEEDPACE)  leftSpeed = leftSpeedCmd;
        if (abs(rightSpeedCmd - rightSpeed) < DC_CHGSPEEDPACE)  rightSpeed = rightSpeedCmd;
        /* Validate the changes */
        DC_SetLeftSpeed(leftSpeed);
        DC_SetRightSpeed(rightSpeed);
        DC_Refresh();
        state = DCSTATE_CHGSPEED_DELAY;
        break;
      }

      /* Now that the speed is correct, go back to idle state */
      state = DCSTATE_WAITCMD;
      break;

    /* ============== SPEED CHANGE DELAY STATE ======== */
    case DCSTATE_CHGSPEED_DELAY :
      if (DC_msg_flag) {
        wait_for(DC_ID, -1);
        state = DCSTATE_WAITCMD;
        break;
      }
      if (!wait_for(DC_ID, DC_CHGSPEEDDELAY)) break;
      state = DCSTATE_CHGSPEED;
      break;
  }
}

/* -------------------------------------------------------------------------- */
/* ----------- API 2 : Raw control over the DC Motor speed ------------------ */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/* ----------- Internal functions ------------------------------------------- */
/* -------------------------------------------------------------------------- */

/**
 * This function programms a stop for later
 *
 * @param pdelay  - in how much time (in milliseconds) should the car stop
 *                  (not taking into account the time to align the front wheel)
 *                  (give pdelay = 0 to cancel the current deferred stop)
 */
void    DC_dstop(unsigned long pdelay)
{
  if (pdelay == 0) { // Cancel the current programmed stop
    DC_dstop_state = DC_DSTOP_IDLE; // Reset the DSTOP FSM
    DC_dstop_flag = 0;
  }
  else {            // Else program a new deferred stop
    DC_dstop_delay = pdelay;
    DC_dstop_flag = 1;
  }
}

/**
 * This function should be called on loop
 */
void    DC_dstop_loop()
{
  switch (DC_dstop_state) {
    case DC_DSTOP_IDLE :
      if (DC_dstop_flag) {
        DC_dstop_flag = 0;
        DC_dstop_state = DC_DSTOP_PENDING;
        break;
      }
      break;

    case DC_DSTOP_PENDING :
      if (DC_fwheel_aligned) {
        DC_dstop_time = millis() + DC_dstop_delay;
        DC_dstop_state = DC_DSTOP_RUNNING;
        break;
      }
      break;

    case  DC_DSTOP_RUNNING :
      if (millis() >= DC_dstop_time) {
        DC_stop();
        DC_dstop_state = DC_DSTOP_IDLE;
        break;
      }
      break;
  }
}

int     DC_car_is_stopped()
{
  return (DC_LeftMotorSpeed == 0 && DC_RightMotorSpeed == 0);
}

int     DC_distance_to_duration(long D) // [mm]
{
  long   vmax;
  long   dv;
  long   dt;
  long   T1;
  long   T; // [ms]

  D *= 1000;                                      // [um]
  vmax = analogRead(PIN_POT) / 1023.0 * DC_VMAX;  // [um/ms]
  dv = DC_CHGSPEEDPACE / 255.0 * DC_VMAX;         // [um/ms]
  dt = DC_CHGSPEEDDELAY;                          // [ms]
  T1 = (vmax * 1.0 / dv - 1) * DC_CHGSPEEDDELAY;  // [ms]
  if (T1 < 0)
    T1 = 0;

  if (D >= vmax * T1)
    T = D / vmax;
  else
    T = sqrt(D * T1 / vmax);

  BT_print("PIN_POT: ");
  BT_print(analogRead(PIN_POT));
  BT_print("\n");

  BT_print("vmax: ");
  BT_print(vmax);
  BT_print("\n");

  BT_print("dv: ");
  BT_print(dv);
  BT_print("\n");

  BT_print("dt: ");
  BT_print(dt);
  BT_print("\n");

  BT_print("T1: ");
  BT_print(T1);
  BT_print("\n");

  BT_print("duration: ");
  BT_print(T);
  BT_print("\n");

  return T;
}

