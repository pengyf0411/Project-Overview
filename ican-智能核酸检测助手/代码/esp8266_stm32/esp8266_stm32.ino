/*
 * 本程序用于stm32端的esp8266，用于连接物联网平台，订阅主题
 * 获取payload信息
 * 通过串口通讯传输给stm32，最终输出到显示屏上
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

const char* ssid = "HUAWEI Mate 40E Pro";
const char* password = "a15dd00f424d";
const char* mqttServer = "test.ranye-iot.net";//物联网平台网址

Ticker ticker;
WiFiClient wifiClient;//用于连接WiFi
PubSubClient mqttClient(wifiClient);//用于连接服务器，发布订阅消息
//SoftwareSerial mySerial(28,29);//用于软串口通讯，RX/TX

int counter;//定义计时器

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);//无线终端模式
  connectWifi();//连接Wifi

  mqttClient.setServer(mqttServer, 1883);//设置TCP端口号
  mqttClient.setCallback(receiveCallback);//设置调回函数

  connectMQTTServer();//连接mqtt服务器，并订阅主题。
  
  ticker.attach(1,tickerCounter);//每隔一秒，计时器++
}

void loop() {
  // put your main code here, to run repeatedly
  if(mqttClient.connected()){
    mqttClient.loop();//保持心跳机制
  }else{
    Serial.println("Fail to connect");
    delay(1000);
    Serial.print("State:");
    Serial.println(mqttClient.state());
    delay(2000);
    Serial.println("Reconnecting...");
    connectMQTTServer();//断线重连
  }
}

void connectWifi(){
  WiFi.begin(ssid, password);
 
  //等待WiFi连接,成功连接后输出成功信息
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");

  }
  Serial.println("");
  Serial.println("WiFi Connected!");  
  Serial.println(""); 
  Serial.println("macAddress:");
  Serial.println(WiFi.macAddress());
  
}

void connectMQTTServer(){
  String clientId = "Esp8266/QRcode/5C:CF:7F:B1:CB:01";
  // 连接MQTT服务器
  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.println("ClientId:");
    Serial.println(clientId);
     
    subscribeTopic();
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(2000);
  }   
}

void tickerCounter(){
  counter++;
}

//订阅指定主题
void subscribeTopic(){
  String topicString1 = "Esp8266/QRcode/" + WiFi.macAddress();
  char topicChar1[topicString1.length()+1];

  strcpy(topicChar1,topicString1.c_str());
  
  if(mqttClient.subscribe(topicChar1)){

    Serial.println("Subscribe Topic:");
    Serial.println(topicChar1);
      
  }else{
    Serial.print("Subscribe Fail...");
  }

}

//收到信息后的回调函数，用于返回信息
void receiveCallback(char* topic,byte* payload,unsigned int length){

  Serial.print("Received:[");
  Serial.print(topic);
  Serial.println("]");
    
  for(int i = 0;i<length;i++){
    Serial.print((char)payload[i]);
  }
  Serial.println("");//将信息输出到硬串口（调试用）
}
