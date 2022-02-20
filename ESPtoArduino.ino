const int motorOUT = 13;
const int motorIN = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(motorOUT, OUTPUT);
  pinMode(motorIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  if(digitalRead(motorIN) == HIGH){
    digitalWrite(motorOUT, HIGH);
  }
  else{
    digitalWrite(motorOUT, LOW);
  }
  
}
