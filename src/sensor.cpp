
#include "sensor.hpp"

const unsigned long   MEASURE_TIMEOUT = 25000;
const float           SOUND_SPEED = 340.0 / 1000000;
static int            US_angle; // between -85 and 85
static Servo          US_servo;

void    US_init()
{
  /* Initialise les broches */
  pinMode(PIN_US_TRIG, OUTPUT);
  digitalWrite(PIN_US_TRIG, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(PIN_US_ECHO, INPUT);

  US_servo.attach(PIN_US_SERVO);
  US_angle = 0; 
  US_servo.write(US_angle + US_ANGLE_OFFSET);
}

/**
 * This function makes the ultrasonic sensor read and returns the read distance
 * 
 * @return -  The distance read in cm
 */
long    US_read()
{
  //int new_val;
  //int enable;
  unsigned long   measure;
  float           distance_m;
  float           distance_cm;

  //new_val = Serial.read();
  
  //if(new_val != -1)
  //{
      //Serial.flush();

  /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
      digitalWrite(PIN_US_TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(PIN_US_TRIG, LOW);
  
  /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
      measure = pulseIn(PIN_US_ECHO, HIGH, MEASURE_TIMEOUT);
      Serial.println(measure, 2);

  /* 3. Calcul la distance à partir du temps mesuré */
      //float distance_m = measure / 2.0 * SOUND_SPEED;
      distance_m =  measure * SOUND_SPEED / 2.0;
      distance_cm = (distance_m * 100.0) - 1;
   
  /* 4. Affiche les résultats en mm, cm et m */
  if (distance_cm)
  {
      Serial1.println("Obstacle !");    
      Serial1.print("Distance: ");
      Serial1.print(distance_cm, 2);
      Serial1.println(" cm");
  }
  else
  {
    Serial1.println("Pas d'obstacle !");
  }

  return (long)distance_cm;
      //Serial.print(distance_mm);
      //Serial.print("mm (");
      //Serial.print(distance_mm / 10.0, 2);
      //Serial.print("cm, ");
      //Serial.print(distance_mm / 1000.0, 2);
      //Serial.println("m)");
   
  /* Délai d'attente pour éviter d'afficher trop de résultats à la seconde */
      //delay(500);
  //}
}

void  US_set_angle(int angle)
{
  if (abs(angle) > 85)
    return;
  US_angle = angle;
  US_servo.write(US_angle + US_ANGLE_OFFSET);
}

int   US_get_angle()
{
  return US_angle;
}


