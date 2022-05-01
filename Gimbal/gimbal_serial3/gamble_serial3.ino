float target_angle;
int axis_data[4],data_size=0;


float analysis(int* t);
void read_data();
void print_data(int* t);


void setup() {
  Serial.begin(115200);
}

void loop()
{
  if(Serial.available())
  {read_data();}
  Serial.println(target_angle);
}
void read_data()
{
   if(4 == data_size)
   {
      print_data(axis_data);
    delay(500);
    target_angle=analysis(axis_data);
    Serial.println(target_angle);

    delay(1000);
      data_size=0;
   }
   else
   {
      char tempChar = Serial.read();
      if('x' == tempChar)
      {
        data_size = 0;
      }
      else
      {
        if(tempChar >=48)
        {
        axis_data[data_size]=tempChar-48;
        data_size++;
        }
    }
   }
}
float analysis(int* t)
{
  float x=0;
  for(int a=3;a>=0;a--){
//    Serial.println (pow(10,a));
    x+=(*(t+3-a))*pow(10,a);
  }
  return x;
}

void print_data(int* t)
{
  for(int a=3;a>=0;a--)
    {
      Serial.print(*(t+3-a));
    }
    Serial.println(' ');
}
