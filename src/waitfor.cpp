
#include "waitfor.hpp"

/**
 * This functions waits for 'duration' milliseconds but unlike delay(),
 * it's non-blocking so it can be used in a multitasking situation.
 * 
 * This function can be in two states : idle or on a wait
 * If the function is idle, when called it goes to the on-a-wait state
 * If the funtion is on a wait, when called it returns whether time's up,
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
 *            if (!wait_for(0, 42)) break;   // If time's up, go to next state
 *            state = DO_SMTH_ELSE;
 *            break;
 *          
 *          case DO_SMTH_ELSE :
 *            ...
 *            break;
 *          ...
 *       }
 * 
 * The example above may be the most widely used, but in some cases one might
 * want to stop waiting when a certain event occurs (e.g. when foo = 1)
 * In this case, one has to reset the state of the function wait_for()
 * otherwise, at the next waiting, the function will not be in idle state.
 *
 * Here is a second example, where an event can break the waiting :
 *
 *       switch (state) {
 *          case DO_SMTH:
 *            ...
 *            state = WAIT_FOR_42MS_EXCEPT_IF_EVENT_ON_FOO;
 *            break;
 *
 *          case WAIT_FOR_42MS_EXCEPT_IF_EVENT_ON_FOO :
 *            if (foo == 1) {
 *              wait_for(0, -1);     // Reset wait_for()
 *              state = SOME_OTHER_STATE;
 *              break;
 *            }
 *            if (!wait_for(0, 42)) break;   // If time's up, exit state
 *            state = DO_SMTH_ELSE;
 *            break;
 *
 *          case DO_SMTH_ELSE :
 *            ...
 *            break;
 *          ...
 *       }
 *
 * Several threads can use this same function concurrently, but they
 * have to use a unique timer, so there should not be different threads
 * giving the same id when calling wait_for().
 *
 * There are WAITFOR_MAX_TIMER timers in total, if you need more timers
 * because you have many threads, change this macro in waitfor.hpp
 *
 * @param id          - The id of the timer you are using
 * @param duration    - how much time to wait (in milliseconds)
 * @return            - 1 if time's up, -2 if error, 0 otherwise
 */
int   wait_for(int id, unsigned long duration)
{
  static int            waiting[WAITFOR_MAX_TIMER] = {0};
  static unsigned long  timestamp[WAITFOR_MAX_TIMER];

  /* If the id is out of range */
  if (id < 0 || id >= WAITFOR_MAX_TIMER)
    return -2;

  /* If the caller made a special call to reset the function state */
  if (duration == -1)
  {
    waiting[id]= 0;
    return 0;
  }

  /* If the function was idle (not on a wait) */
  if (!waiting[id])
  {
    /* Start the wait */
    timestamp[id] = millis();
    waiting[id] = 1;
    return 0;
  }
  /* Else if the function was already on a wait */
  else
  {
    /* If time's up, return 1 and go back to idle state */
    if (millis() - timestamp[id] > duration)
    {
      waiting[id] = 0;
      return 1;
    }
    /* Otherwise keep waiting... */
    else
      return 0;
  }
}

