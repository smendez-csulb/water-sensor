// declaring pins
int button = 7;
int laser = 6;

int led = 11;
int sensor = 12;


// declaring states
int buttonState;
bool laserState = 0;
int sensorState;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // assigning each pin as in/output
  pinMode(button, INPUT);
  pinMode(laser, OUTPUT);
  
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  //read the digital value of the button and sensor's pin
  buttonState = digitalRead(button);
  sensorState = digitalRead(sensor);


  //Serial.println((String)"laserState: "+laserState);
  //Serial.println((String)"buttonState: "+buttonState);
  
  // if btn is pressed and laser is off, turn on laser
  if ((buttonState == LOW) && (laserState == LOW)) {
    digitalWrite(laser, HIGH);
    laserState = 1;
  }
  // or if btn is pressed and laser is on, turn off laser
  else if ((buttonState == LOW) && (laserState == HIGH)) {
    digitalWrite(laser, LOW);
    laserState = 0;
  }

  //Serial.println((String)"sensorState: "+sensorState);
  
  // if light sensor is high, turn on led, else turn off led
  if (sensorState == HIGH){
    digitalWrite(led, HIGH);
    Serial.println("LIGHTT");
  }
  else{
    digitalWrite(led, LOW);
  }
  delay(200);
}
