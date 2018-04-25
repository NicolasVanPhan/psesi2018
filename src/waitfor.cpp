
#include "waitfor.hpp"

/**
 * This functions waits for 'duration' milliseconds but unlike delay(),
 * it's non-blocking so it can be used in a multitasking situation.
 * 
 * This function can be in two states : idle or on a wait
 * If the function is idle, when called it goes to the on-a-wait state (and return 0)
 * If the funtion is on a wait, when called it returns whether time's up or not,
 * if time's up, it returns 1 and goes back to idle state
 *
 * This way, it can typically be used in a finite state machine :
 * 
 *       switch (state) {
 *          case DO_SMTH:
 *            ...
 *            state = WAIT_FOR_42MS;
 *            break;
 *          
 *          case WAIT_FOR_42MS :
 *            if (!wait_for(42)) break;
 *            state = DO_SMTH_ELSE;
 *            break;
 *          
 *          case DO_SMTH_ELSE :
 *            ...
 *            break;
 *          ...
 *       }
 * 
 * @param duration    - how much time to wait (in milliseconds)
 * @return            - 1 if time's up, 0 otherwise
 */
int   wait_for(unsigned long duration)
{
  static int            waiting = 0;
  static unsigned long  timestamp;

  /* If the function was idle (not on a wait) */
  if (!waiting)
  {
    /* Start the wait */
    timestamp = millis();
    waiting = 1;
    return 0;
  }
  /* Else if the function was already on a wait */
  else
  {
    /* If time's up, return 1 and go back to idle state */
    if (millis() - timestamp > duration)
    {
      waiting = 0;
      return 1;
    }
    /* Otherwise keep waiting... */
    else
      return 0;
  }
}

