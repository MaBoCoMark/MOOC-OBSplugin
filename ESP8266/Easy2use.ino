#include "define.h"
#include "PWM.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSockets2_Generic.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer esp8266_server(80);
using namespace websockets2_generic;
WebsocketsClient client;
const char *ssid = "a8266";
const char *password = "12345678"; 
bool button=0;
bool handleRootSuccess=1;
bool wifiTimeOut=0;
bool wifiReconnectSuccess=1;
bool IPconfirm=0;
int scene=0;//cam0 ppt1
String websockets_server_host="192.168.0.2";
int websockets_server_port=4444;
void InstantBlink()
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);  
}
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
  if (event == WebsocketsEvent::ConnectionOpened)   {    Serial.println("Connnection Opened");  } 
  else if (event == WebsocketsEvent::ConnectionClosed)   {    Serial.println("Connnection Closed");  } 
  else if (event == WebsocketsEvent::GotPing)   {    Serial.println("Got a Ping!");  } 
  else if (event == WebsocketsEvent::GotPong)   {    Serial.println("Got a Pong!");  }
}

//////////////////////////////////////////////////////HTML//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void handleRoot() 
{     
  if((0==wifiTimeOut && 0==IPconfirm)){esp8266_server.send(200, "text/html", sendWLAN());delay(500);}
  else{esp8266_server.send(200, "text/html", sendRoot());delay(500);}
  if((0==wifiTimeOut && 1==IPconfirm)){handleRootSuccess = 0;}
}
          String sendRoot()
          {
          
          String htmlCode = "<!DOCTYPE html><meta charset=\"utf-8\" />";
          htmlCode +="<title>WebSocket Settings</title>";
          
          if(0==wifiTimeOut)
          {
            if(1 == handleRootSuccess){htmlCode +="<h3>Type OBS address and port below :D</h3>";}
            else{htmlCode +="<h3>OOPS! Error occured. Type OBS address and port below :(</h3>";}
            htmlCode +="<form action=\"ez\" method=\"get\"><input type=\"text\" name=\"addr\" value=\"\">";
            htmlCode +="<input type=\"text\" name=\"port\" value=\"4444\"><input type=\"submit\" value=\"Submit\"></form>";
          }
          else if(1==wifiTimeOut)
          {
            if(1 == wifiReconnectSuccess){htmlCode +="<h3>No WLAN/WiFi connection Σ（ﾟдﾟ！）</h3>";}
            else if(0 == wifiReconnectSuccess){htmlCode +="<h3>Still No connection Σ（ﾟдﾟ!!!）</h3>";}        
            htmlCode +="<h4>Type your wifi SSID and password down below ↓</h4>";
            htmlCode +="<form action=\"WLANconnect\" method=\"get\">";
            htmlCode +="<input type=\"text\" name=\"ssid\" value=\"SSID\"><br>";
            htmlCode +="<input type=\"text\" name=\"password\" value=\"P@ssW0rd\"><br>";
            htmlCode +="<input type=\"submit\" value=\"Let's go have a try\"></form>";
          }     
          return htmlCode;
          }

void handleEz() 
{  
  websockets_server_host = esp8266_server.arg("addr");
  websockets_server_port = esp8266_server.arg("port").toInt();
  Serial.println(websockets_server_host);
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connected) 
  {
    String WS_msg = String("{\"message-id\":\"1\",\"request-type\":\"SetCurrentScene\",\"scene-name\":\"Camera Scene\"}");
    client.send(WS_msg);
    handleRootSuccess = 1;
    scene=0;
    esp8266_server.send(200, "text/html", sendEz());
    delay(500);
    for(int Secs=5;0<Secs;Secs--) 
    {
    for(int i=0;25>i;i++){PWM(LED_BUILTIN,100,i,5,OFF);}
    for(int i=25;0<i;i--){PWM(LED_BUILTIN,100,i,5,OFF);}
    delay(200);
    } 
  }
  else 
  {
    handleRootSuccess = 0;
    esp8266_server.send(200, "text/html", sendRoot());
    delay(500);
  }
}
          String sendEz()
          {          
          String htmlCode = "<!DOCTYPE html><meta charset=\"utf-8\" />";
          htmlCode +="<title>WS Easy2Use</title>";
          htmlCode +="<h2>WebSocket address already set as : ";
          htmlCode +=websockets_server_host;
          htmlCode +=":";
          htmlCode +=websockets_server_port;
          htmlCode +="</h2><br><h4>It is Easy to use, Right?</h4><h5>and OBS should turn to \"Camera Scene\""; 
          return htmlCode;
          }

void handleWLAN() 
{  
  esp8266_server.send(200, "text/html", sendConnecting());
  delay(500);
//  handleRootSuccess = 0;
  int wifiTimeOutCounter=0;  
  String fixedSSID =  esp8266_server.arg("ssid");
  String fixedPW =  esp8266_server.arg("password"); 
  char cFixedSSID[fixedSSID.length() + 1];
  fixedSSID.toCharArray(cFixedSSID, fixedSSID.length() + 1);
  char cFixedPW[fixedPW.length() + 1];
  fixedPW.toCharArray(cFixedPW, fixedPW.length() + 1);
  wifiMulti.addAP(cFixedSSID,cFixedPW); 
  wifiTimeOut=0;     
  wifiReconnectSuccess=1;
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    for(int i=0;100>i;i++){PWM(LED_BUILTIN,100,i,5,ON);}
    delay(10);
    for(int i=100;0<i;i--){PWM(LED_BUILTIN,100,i,5,OFF);}
    delay(800);
    wifiTimeOutCounter++;
    if(8<=wifiTimeOutCounter){wifiTimeOut=1;wifiReconnectSuccess=0;break;}
  }   
}
          String sendWLAN()
          {
          String htmlCode = "<!DOCTYPE html><meta charset=\"utf-8\" />";
          htmlCode +="<title>Easy2Use WiFi Stats</title>";
          htmlCode +="<h2>Connected to : ";
          htmlCode +=WiFi.SSID();
          htmlCode +=" as [";
          htmlCode +=WiFi.localIP().toString();
          htmlCode +="] </h2><h3>Please Copy the IP address then click</h3>"; 
          htmlCode +="<form action=\"confirm\" method=\"get\"><input type=\"submit\" value=\"Confirm\"></form>";
          htmlCode +="<br><h4>It is Easy to use, Right?</h4><h5>and connect your WLAN plz :D";
          return htmlCode;
          }
          String sendConnecting()
          {
          String htmlCode = "<!DOCTYPE html><meta charset=\"utf-8\" />";
          htmlCode +="<title>WLAN Connecting...</title>";
          htmlCode +="<h1>Connecting...</h1><h5>Click ↓ when start to blink</h5>";
          htmlCode +="<button onclick=\"location.href='http://192.168.4.1/'\" type=\"button\">BACK</button>";
          htmlCode +="<h5>Click ↑ when start to blink</h5>";
          return htmlCode;
          }
      
void handleConfirm() 
{  
  IPconfirm=1;
  esp8266_server.send(200, "text/html", sendConfirm()); 
  delay(500);  
}
          String sendConfirm()
          {
          String htmlCode = "<!DOCTYPE html><meta charset=\"utf-8\" />";
          htmlCode +="<title>Easy2Use Confirm</title>";
          htmlCode +="<h1>Confirmed!</h1>";
          return htmlCode;
          }
          
void handleNotFound()
{  esp8266_server.send(404, "text/plain", "404: Not found"); delay(500);  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////HTML//////////////////////////////////////////////////////////////

 

void setup() {
  Serial.begin(115200);
  pinMode(D1,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);//LED D2 so..
  WiFi.softAP(ssid, password,1,0,1);//channel=1,hidden=0,MaxConnections=1
  Serial.print("Access Point: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  wifiMulti.addAP("SEBE", "aoeui@2333"); 
  Serial.println("Connecting ...");   
  int wifiTimeOutCounter=0;                                  
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    for(int i=0;100>i;i++){PWM(LED_BUILTIN,100,i,5,ON);}
    delay(10);
    for(int i=100;0<i;i--){PWM(LED_BUILTIN,100,i,5,OFF);}
    delay(800);
    wifiTimeOutCounter++;
    if(8<=wifiTimeOutCounter){wifiTimeOut=1;break;}
  }
  if(1 != wifiTimeOut)
  {
    InstantBlink();
    Serial.println('\n');
    InstantBlink();
    Serial.print("Connected to ");
    InstantBlink();
    Serial.println(WiFi.SSID());
    InstantBlink();
    Serial.print("IP address:\t");
    InstantBlink();          
    Serial.println(WiFi.localIP()); 
    InstantBlink();
  }

  esp8266_server.begin();                  
  esp8266_server.on("/",handleRoot);
  esp8266_server.on("/ez",handleEz);
  esp8266_server.on("/WLANconnect",handleWLAN);
  esp8266_server.on("/confirm",handleConfirm);
  esp8266_server.onNotFound(handleNotFound); 

 
  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) 
  {
    Serial.print("Got Message: ");
    Serial.println(message.data());
  });
  client.onEvent(onEventsCallback);

  digitalWrite(LED_BUILTIN, LOW);
}
 
 
void loop() {
  esp8266_server.handleClient();
  if(1==wifiTimeOut)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(400);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
  else if(0==IPconfirm){digitalWrite(LED_BUILTIN, LOW);delay(200);digitalWrite(LED_BUILTIN, HIGH);delay(800);}
  else if(1==IPconfirm){
//  {WiFi.softAP("HIDDEN_SSID","TheM0st*C0mplic@ted*P@ssw0rd",1,1,0);}
    if(0 == handleRootSuccess)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(800);
    }  
    delay(100);//StatusRefreshFreq
    if(0 == digitalRead(D1))
    {
      delay(10);
      while(0 == digitalRead(D1))
      {delay(10);}
      button = 1;
    }
    if(1 == button)
    {
      SwitchScene();
      if(0==scene){digitalWrite(LED_BUILTIN, HIGH);}
      if(1==scene){digitalWrite(LED_BUILTIN, LOW);}
    }
  }
}
