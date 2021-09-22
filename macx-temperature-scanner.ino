#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

/*
 * 2 - IR
 * 3 - RELAY 1
 * 4 - TX
 * 5 - RX
 * 6 - TRIG
 * 7 - ECHO
 */

 #define IR1 2
 #define RELAY 3
 #define RELAY_BUTTON 8
 #define DFTX 4
 #define RFTX 5
 #define TRIG 6
 #define ECHO 7

unsigned long millis_time = 0;
unsigned long period = 10000;

SoftwareSerial musicSerial(DFTX, RFTX);

// Initialize Music Device
void initializeMusic(){
  musicSerial.begin(9600);
  mp3_set_serial(musicSerial);
  delay(50);
  mp3_set_volume(30);
}

// Initialize Sensors
void initializeDevice(){
  Serial.begin(9600);
  Serial.println("Initializing Device...");
  initializeMusic();
  
  pinMode(IR1, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(RELAY_BUTTON, OUTPUT);

  // HC-SR04 
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  digitalWrite(RELAY, HIGH);
  digitalWrite(RELAY_BUTTON, HIGH);
  delay(5000);
}

int getDistance(){
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

/*
 * Detect if person is detected
 * (0) when detected, (1) when not detected
 */
bool personDetected(){
  if(digitalRead(IR1) == 0){
    return true;
  }
  return false;
}

/*
 * Dispensing Hand Sanitizer
 */
void dispenseHandSanitizer(int pre_made_delay=95){
  // Allow to dispense the hand sanitizer
  int counter = 0;
  delay(500);
  mp3_play(2);
  delay(100);
  int one_time = 0;
  while(counter <= pre_made_delay){
    counter++;
    Serial.println("Dispensing");
    if(one_time == 0){
      digitalWrite(RELAY, LOW);
      delay(1000);
      one_time = 1;
      digitalWrite(RELAY_BUTTON, LOW);
      delay(70);
      digitalWrite(RELAY_BUTTON, HIGH);
    }
    delay(50);
  }
  mp3_play(3);
  digitalWrite(RELAY, HIGH);
  delay(2000);
}

void setup() {
  initializeDevice();
  delay(1000);
}

void loop() {

  /* Trigger voice when person approaching 
   * Distance Trigger : 40 CM */
  if(getDistance() < 40){
    Serial.println("Triggering Sound");
    if(millis() >= millis_time + period){
      mp3_play(1);
      millis_time = millis();
    }
  }
  
  /* Infrared Detect */
  if(personDetected()){
    dispenseHandSanitizer();
    delay(300);
  } 
  
}
