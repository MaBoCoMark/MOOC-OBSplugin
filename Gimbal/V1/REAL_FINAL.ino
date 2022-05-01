#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
uint8_t degree[8]  = {0x1c,0x14,0x1c,0x0,0x0,0x0,0x0};
uint8_t backslash[8]  = {0x0,0x10,0x8,0x4,0x2,0x1,0x0};
float KP=-5,KI,KD;
char cmd[50];
boolean Emergency=false;
int AimSpeed_X,AimSpeed_Y,PID_X,PID_Y;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degree);
  lcd.createChar(1, backslash);
  lcd.setCursor(0, 0);
  lcd.print("Holy");
  lcd.setCursor(8,0);
  lcd.print("OpenVINO");
  pinMode(3,INPUT_PULLUP);
  pinMode(LED_BUILTIN,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3),Stop,LOW);
  Emergency=false;
  delay(1000);//等待舵机初始化
//  Initialze_Moter();
//  Serial.println("#255PMOD5!");
  if(Serial.available()) Serial.readString();//清除缓存区的返回值
  lcd.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////Initialze_Moter
void Initialze_Moter()
{
  Serial.println("#255PMOD3!");//全部切成舵机模式，归中
  delay(100);
  Serial.println("#255P1500T0500!");
  for(int i=0;i<=20;i++) Blink();
}
//////////////////////////////////////////////////////////////////////////////////////Blink
void Blink()
{
  digitalWrite(LED_BUILTIN,HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN,LOW);
  delay(100);
}
///////////////////////////////////////////////////////////////////////////////////////Stop
void Stop() {Emergency=true;}
///////////////////////////////////////////////////////////////////////////////Position_PID
int Position_PID (float Actual,float Target)
{
  float Deviation,Output,Integral_Deviation,Last_Deviation;
  Deviation=Actual-Target; //计算偏差
  Integral_Deviation+=Deviation; //求出偏差的积分
  KI/=Deviation;
  
  if(abs(Deviation)>5)  KI=0.0;//变积分过程
  else KI=100.0-abs(Deviation)*20.0;
  
  Output=KP*Deviation+KI*Integral_Deviation+KD*(Deviation-Last_Deviation); //位置式PID控制器
  Last_Deviation=Deviation; //更新上一次偏差
//  Serial.print(Deviation);
//  Serial.println(Output);
  return Output; //增量输出
}
////////////////////////////////////////////////////////////////////////////

int x,y;

void loop() {
  if(Emergency==true)
  {
//    Serial.println("#255PDST!");
    while(1);
  }
  else
  {
      if(Serial.available())
      {
//          lcd.clear();
          if(Serial.find('x'))
          {
            x=Serial.parseInt();
            y=Serial.parseInt();
//            Serial.println(x);
//            Serial.println(y);
            PID_X=Position_PID (x,300);//x分辨率中点
            PID_Y=Position_PID (y,250);//y分辨率中点
//            delay(200);
          
            AimSpeed_Y=1500+PID_Y;
            AimSpeed_X=1500+PID_X;
            if(AimSpeed_X>2500) AimSpeed_X=2500;
            if(AimSpeed_X<500) AimSpeed_X=500;
            if(AimSpeed_Y>2500) AimSpeed_Y=2500;
            if(AimSpeed_Y<500) AimSpeed_Y=500;
          
            sprintf(cmd,"#001P%04dT0001!#000P%04dT0001!",AimSpeed_Y,AimSpeed_X);
//            Serial.println(cmd);
Serial.println(AimSpeed_X);
          }
      }
  //    lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print(x);
//      lcd.setCursor(8,0);
//      lcd.print(y);
  //    lcd.write(0);
  }
}
