HardwareSerial *moduleSerial = &Serial1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  moduleSerial->begin(115200);  // Verify your module's baud rate/home/marc-fortier/Arduino/libraries/AT-Command-Library-master/ATcommands.cpp
  //module.begin(*moduleSerial);

  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(9, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(10, 1);
  digitalWrite(11, 1);
  digitalWrite(12, 1);
  
  delay(1000);

  digitalWrite(10, 0);
  digitalWrite(11, 0);
  digitalWrite(12, 0);
  
  delay(500);

  Serial.println(digitalRead(9));
  moduleSerial->println("dude");
}
