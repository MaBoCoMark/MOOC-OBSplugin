//typedef enum {OFF,ON}     State;

enum Stat {OFF,ON};

//void PWM(int,int,int,int,enum Stat);

void PWM(int PinPin,int Hertz,int DutyCycle,int Tms,enum Stat stat)//ESP32 0.23GHz
{
  int us=10000/Hertz,lefts=10000%Hertz,TLimit=(Tms*1000)-lefts-1;
  for(long Tcount=0;TLimit>Tcount;Tcount++)
  {
    digitalWrite(PinPin,HIGH);//OFF
    delayMicroseconds(us*(100-DutyCycle));
    digitalWrite(PinPin,LOW);//ON
    delayMicroseconds(us*DutyCycle);
    Tcount+=(us*100);
  }
  delayMicroseconds(lefts*100);
  if(OFF==stat){digitalWrite(PinPin,HIGH);}
  if(ON==stat){digitalWrite(PinPin,LOW);}
}
