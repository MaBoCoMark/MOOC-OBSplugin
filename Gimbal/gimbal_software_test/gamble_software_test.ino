void setup() {
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
}
bool led=0;
void loop() {
  if(Serial.available())
  {
    if(Serial.read()=='x');//找到首位地址x
    {
      digitalWrite(13,led);
      led=!led;
    }
  }
}
