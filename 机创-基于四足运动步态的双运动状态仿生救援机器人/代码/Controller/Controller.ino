#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

const char* ssid = "pengyf";
const char* password = "12345678";
const char* mqttServer = "test.ranye-iot.net";//本项目使用然也服务器

Ticker ticker;
WiFiClient wifiClient;//用于连接WiFi
PubSubClient mqttClient(wifiClient);//用于连接服务器，发布订阅消息

int counter;//定义计时器

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  connectWifi();

  mqttClient.setServer(mqttServer, 1883);//设置TCP端口号
  mqttClient.setCallback(receiveCallback);//设置调回函数

  ticker.attach(1,tickerCounter);//每隔一秒，计时器++
}

void loop() {
  // put your main code here, to run repeatedly:
  if(mqttClient.connected()){
    mqttClient.loop();//保持心跳机制
  }else{
    delay(1000);
    connectMQTTServer();//断线重连
  }
}

void connectWifi(){
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(1000);
  }
}

void connectMQTTServer(){
  String clientId = "esp8266-" + WiFi.macAddress();//获取MAC地址，命名客户端。
}
void publishMQTT(){
  String topicString1 = "Esp8266/control/";//用mac地址命名主题1
  char topicChar1[topicString1.length() + 1];//将string转换为char数组
  strcpy(topicChar1,topicString1.c_str());

  String topicString2 = "Esp8266/mode/..."  + WiFi.macAddress();//用mac地址命名主题2
  char topicChar2[topicString2.length() + 1];
  strcpy(topicChar2,topicString2.c_str());
  /***
   * 更多主题......
   */
  String msgString1 = "message1";
  char publishMsg1[msgString1.length() + 1];
  strcpy(publishMsg1,msgString1.c_str());
  
  String msgString2 = "message2";
  char publishMsg2[msgString2.length() + 1];
  strcpy(publishMsg2,msgString2.c_str());
   
  mqttClient.publish(topicChar1,publishMsg1);
  
  mqttClient.publish(topicChar2,publishMsg2);

}
