#include <SimpleFOC.h>
MagneticSensorSPI sensor = MagneticSensorSPI(5, 14, 0x3FFF);
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver =BLDCDriver3PWM(26,27,14,12);

float target_angle=0;
int axis_data[4],data_size=0;

float analysis(int* t);
void read_data();




void setup() {
  sensor.init();
  motor.linkSensor(&sensor);

  driver.pwm_frequency = 100000;
  driver.voltage_power_supply = 12;
  driver.voltage_limit = 8;
  driver.init();
  motor.linkDriver(&driver);
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  motor.controller = MotionControlType::velocity;

  motor.PID_velocity.P = 2.0;
  motor.PID_velocity.I = 16;
  motor.PID_velocity.D = 0.02;
  motor.voltage_limit = 12;


  motor.PID_velocity.output_ramp = 40;
  motor.LPF_velocity.Tf = 0.002;


  Serial.begin(115200);
  motor.init();
  
  motor.initFOC();
  Serial.println("FOC Ready!!");
  _delay(1000);
  
}

void loop() {
  if(4 == data_size)
   {
      target_angle=analysis(axis_data);
      target_angle=constrain(target_angle, -2, 2);

      Serial.println(target_angle);
      data_size=0;
   }//读完一帧
  else if(Serial.available())
  {
    read_data();
  }//没读完呢，我要接着读！
  motor.loopFOC();
  motor.move(target_angle);
}

void read_data()
{
      int tempChar = Serial.read();
      if('x' == tempChar){
        data_size = 0;
      }
      else{
        if(tempChar >=48){
          axis_data[data_size]=tempChar-48;
          data_size++;
        }
      }
}
float analysis(int* t)
{
  float x=0;
  for(int a=3;a>=0;a--){
    x+=(*(t+3-a))*pow(10,a);
  }
  Serial.print(x);
  Serial.print(' ');
  
  x=(x/500-2);
  Serial.println(x);
  return x;
}



/*
(x+1000)/637-1.57;
x * 3.14 / 2000 - 1.57;
float decimals_map(float x,float in_min 0,float in_max 2000,float out_min ,float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  (x-20)*9/150+3
}
 */
