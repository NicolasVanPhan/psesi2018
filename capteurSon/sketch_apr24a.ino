
const byte TRIGGER_PIN = 40;
const byte ECHO_PIN = 42;

const unsigned long MEASURE_TIMEOUT = 2500;

const float SOUND_SPEED = 240.0 / 1000;

void setup() {
  // put your setup code here, to run once:
  /* Initialise le port série */
  Serial.begin(115200);
   
  /* Initialise les broches */
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
  int val = -1;
  int new_val;
  int enable;

  new_val = Serial.read();
  
  //if (Serial.available())
  //{
   // enable = enable ^ 1;
   // Serial.println(enable);
  //}
  
  if(new_val != -1)
  {
      Serial.flush();
      digitalWrite(TRIGGER_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGGER_PIN, LOW);
  
  /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
      long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
   
  /* 3. Calcul la distance à partir du temps mesuré */
      float distance_mm = measure / 2.0 * SOUND_SPEED;
      float distance_cm = distance_mm / 10.0;
   
  /* Affiche les résultats en mm, cm et m */
  if (distance_cm)
  {
      Serial.println("Obstacle !");    
      Serial.print("Distance: ");
      Serial.print(distance_cm, 2);
      Serial.println(" cm");
  }
  else
  {
    Serial.println("Pas d'obstacle !");
  }
      //Serial.print(distance_mm);
      //Serial.print("mm (");
      //Serial.print(distance_mm / 10.0, 2);
      //Serial.print("cm, ");
      //Serial.print(distance_mm / 1000.0, 2);
      //Serial.println("m)");
   
  /* Délai d'attente pour éviter d'afficher trop de résultats à la seconde */
      delay(500);
  
    
  }
}
