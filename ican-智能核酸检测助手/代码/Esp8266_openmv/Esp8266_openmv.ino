
/*
 * 本程序用于openmv端，接收payload中的内容，发送到MQTT物联网平台
 * 将esp8266与openmv相连，通过串口通讯传输payload的信息
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
 
// wifi信息
const char* ssid = "HUAWEI Mate 40E Pro";
const char* password = "a15dd00f424d";
const char* mqttServer = "test.ranye-iot.net";//物联网平台网址

Ticker ticker;
WiFiClient wifiClient;//用于连接WiFi
PubSubClient mqttClient(wifiClient);//用于连接服务器，发布订阅消息
 
int count;    // Ticker计数用变量

void setup() {
  Serial.begin(115200);
  
  //设置ESP8266工作模式为无线终端模式
  WiFi.mode(WIFI_STA);
  
  // 连接WiFi
  connectWifi();
  
  // 设置MQTT服务器和端口号
  mqttClient.setServer(mqttServer, 1883);
 
  // 连接MQTT服务器
  connectMQTTServer();
 
  // Ticker定时对象
  ticker.attach(1, tickerCount);  
}
 
void loop() { 
  if (mqttClient.connected()) { // 如果开发板成功连接服务器
    // 每隔3秒钟发布一次信息
    if (count >= 3){
      pubMQTTmsg();
      count = 0;
    }    
    // 保持心跳
    mqttClient.loop();
  } else {                  // 如果开发板未能成功连接服务器
    connectMQTTServer();    // 则尝试连接服务器
  }
}
 
void tickerCount(){
  count++;
}
 
void connectMQTTServer(){
  String clientId = "esp8266-" + WiFi.macAddress();//获取MAC地址，命名客户端
 
  // 连接MQTT服务器
  if (mqttClient.connect(clientId.c_str())) { 
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.println("ClientId:");
    Serial.println(clientId);
  } else {
    Serial.print("Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }   
}
 
// 发布信息
void pubMQTTmsg(){
  static int value; // 客户端发布信息用数字
  String payload = "payload:";

  // 建立发布主题。主题名称以Esp8266/QRcode/为前缀，后面添加设备的MAC地址。
  String topicString = "Esp8266/QRcode/" + WiFi.macAddress();
  char publishTopic[topicString.length() + 1];
  strcpy(publishTopic, topicString.c_str());

  while(Serial.available()>0){
    delay(100);//确保数据传输完毕
    payload += String(Serial.read());
    delay(10);
  }

 
  // 建立发布信息。后面添加发布次数。
  //字符串转义
  String messageString = payload + String(value++); 
  char publishMsg[messageString.length() + 1];   
  strcpy(publishMsg, messageString.c_str());
  
  // 实现ESP8266向主题发布信息
  if(mqttClient.publish(publishTopic, publishMsg)){
    Serial.println("Publish Topic:");
    Serial.println(publishTopic);
    Serial.println("Publish message:");
    Serial.println(publishMsg);    
  } else {
    Serial.println("Message Publish Failed."); 
  }
}
 
// ESP8266连接wifi
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
