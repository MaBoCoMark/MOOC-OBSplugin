#define websockets_server_host "192.168.0.2"
#define websockets_server_port 4444

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSockets2_Generic.h>
ESP8266WiFiMulti wifiMulti;
using namespace websockets2_generic;
WebsocketsClient client;
const char *ssid = "a8266";
const char *password = "12345678"; 
bool button=0;
int scene;//cam0 ppt1
void SwitchScene()
{
    client.connect(websockets_server_host, websockets_server_port, "/");
    if(0 == scene){
      String PPT_msg = String("{\"message-id\":\"1\",\"request-type\":\"SetCurrentScene\",\"scene-name\":\"Slideshow Scene\"}");
      client.send(PPT_msg);
      scene = 1;
      }
    else if(1 == scene){
      String CAM_msg = String("{\"message-id\":\"1\",\"request-type\":\"SetCurrentScene\",\"scene-name\":\"Camera Scene\"}");
      client.send(CAM_msg);
      scene = 0;
      }
    button=0;
    delay(50);
}
void onEventsCallback(WebsocketsEvent event, String data) 
{
  if (event == WebsocketsEvent::ConnectionOpened) 
  {
    Serial.println("Connnection Opened");
  } 
  else if (event == WebsocketsEvent::ConnectionClosed) 
  {
    Serial.println("Connnection Closed");
  } 
  else if (event == WebsocketsEvent::GotPing) 
  {
    Serial.println("Got a Ping!");
  } 
  else if (event == WebsocketsEvent::GotPong) 
  {
    Serial.println("Got a Pong!");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(D1,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);//LED D2 so..
  WiFi.softAP(ssid, password);
  Serial.print("Access Point: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  wifiMulti.addAP("6009", "021-6009B"); 
  wifiMulti.addAP("7300HQ5947", "60091234"); 
Serial.println("Connecting ...");         
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");            
  Serial.println(WiFi.localIP()); 

  Serial.println(websockets_server_host);
 
  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) 
  {
    Serial.print("Got Message: ");
    Serial.println(message.data());
  });

  // run callback when events are occuring
  client.onEvent(onEventsCallback);
  
  // try to connect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  
  if (connected) 
  {
    Serial.println("Connected!");

    String WS_msg = String("{\"message-id\":\"1\",\"request-type\":\"SetCurrentScene\",\"scene-name\":\"Camera Scene\"}");
    client.send(WS_msg);
    scene=0;
  } 
  else 
  {
    Serial.println("Not Connected!");
  }
  digitalWrite(LED_BUILTIN, HIGH);
}
 
 
void loop() {
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(200);
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(730);
  
  delay(100);//StatusRefreshFreq
  if(0 == digitalRead(D1))
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    while(0 == digitalRead(D1))
    {
        delay(100);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    button = 1;
  }
  if(1 == button){SwitchScene();}
}
