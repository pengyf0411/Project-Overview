

/*#define cmd_nooperation "_T"    空操作
#define cmd_changeMode "_aT"    改变运动状态
#define cmd_forward "_bT"     前进
#define cmd_back "_cT"    no use
#define cmd_left "_dT"        左转
#define cmd_right "_eT"       右转
#define cmd_forwardLeft "_fT"   no use
#define cmd__forwardRight "_gT"   no use
#define cmd_spin "_hT"    no use
#define cmd_sleep "_iT"   no use
#define cmd_start "_jT"       开始
#define cmd_gimbal_rst "_kT"    云台-重置
#define cmd_gimbal_left "_lT"   云台-左转
#define cmd_gimbal_right "_mT"    云台-右转
#define cmd_gimbal_up "_nT"     云台-上转
#define cmd_gimbal_down "_oT"   云台-下转
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

const char* ssid = "pengyf";
const char* password = "12345678";
const char* mqttServer = "test.ranye-iot.net";//本项目使用然也服务器

Ticker ticker;
WiFiClient wifiClient;//用于连接WiFi
PubSubClient mqttClient(wifiClient);//用于连接服务器，发布订阅消息
//SoftwareSerial mySerial(28,29);//用于软串口通讯，RX/TX

int counter;//定义计时器

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);//无线终端模式
  connectWifi();//连接Wifi

  mqttClient.setServer(mqttServer, 1883);//设置TCP端口号
  mqttClient.setCallback(receiveCallback);//设置调回函数

  connectMQTTServer();//连接mqtt服务器，并订阅主题。
  
  ticker.attach(1,tickerCounter);//每隔一秒，计时器++
  /*mySerial.begin(9600);//打开软串口
  while (!Serial) {

  }//等待软串口开启

  while(mySerial.read()>=0){
    
  }//清除串口缓存

  while (mySerial.overflow()== 1) {

  }//检查缓冲区是否有溢出
  
  */
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
    /*Serial.print(".");
    */
  }
  /*Serial.println("");
  Serial.println("WiFi Connected!");  
  Serial.println(""); 
  Serial.println("macAddress:");
  Serial.println(WiFi.macAddress());
  */
  
}

void connectMQTTServer(){
  String clientId = "esp8266-" + WiFi.macAddress();//获取MAC地址，命名客户端。
  // 连接MQTT服务器
  if (mqttClient.connect(clientId.c_str())) {
    /*Serial.println("MQTT Server Connected.");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.println("ClientId:");
    Serial.println(clientId);
     */
    subscribeTopic();
  } else {
 //   Serial.print("MQTT Server Connect Failed. Client State:");
   // Serial.println(mqttClient.state());
    delay(2000);
  }   
}

void tickerCounter(){
  counter++;
}

//订阅指定主题
void subscribeTopic(){
  String topicString1 = "Esp8266/Walk/" + WiFi.macAddress();
  char topicChar1[topicString1.length()+1];

  String topicString2 = "Esp8266/Drive/" + WiFi.macAddress();
  char topicChar2[topicString2.length() + 1];

  String topicString3 = "Esp8266/FeedBack/" + WiFi.macAddress();
  char topicChar3[topicString3.length() + 1];

  strcpy(topicChar1,topicString1.c_str());
  
  if(mqttClient.subscribe(topicChar1)){

      /*Serial.println("Subscribe Topic:");
    Serial.println(topicChar1);
      */
    
  }else{
   // Serial.print("Subscribe Fail...");
  }

  strcpy(topicChar2, topicString2.c_str());
  if (mqttClient.subscribe(topicChar2)) {

      /*Serial.println("Subscribe Topic:");
      Serial.println(topicChar2);
      */
    
  }
  else {
     // Serial.print("Subscribe Fail...");
  }


  strcpy(topicChar3, topicString3.c_str());
  if (mqttClient.subscribe(topicChar3)) {

      /*Serial.println("Subscribe Topic:");
      Serial.println(topicChar3);
      */
     
  }
  else {
      //Serial.print("Subscribe Fail...");
  }
}

//收到信息后的回调函数，用于返回信息
void receiveCallback(char* topic,byte* payload,unsigned int length){

    /*Serial.print("Received:[");
  Serial.print(topic);
  Serial.println("]");
    */
  
  for(int i = 0;i<length;i++){
    Serial.print((char)payload[i]);
  }
  //Serial.println("");//将信息输出到硬串口（调试用）


  /* for (int i = 0; i < length; i++) {
      mySerial.print((char)payload[i]);
  }
  mySerial.println("");//将信息输出到软串口
  */
  

}



/*void publishMQTT() {
  String topicString1 = "Esp8266/Walk/"+ WiFi.macAddress();//用mac地址命名 行走指令主题,
  char topicChar1[topicString1.length() + 1];//将string转换为char数组
  strcpy(topicChar1,topicString1.c_str());

  String topicString2 = "Esp8266/Drive/"  + WiFi.macAddress();//用mac地址命名主题2
  char topicChar2[topicString2.length() + 1];
  strcpy(topicChar2,topicString2.c_str());

  String topicString3 = "Esp8266/FeedBack/" + WiFi.macAddress();//用mac地址命名主题3
  char topicChar3[wopicString3.length() + 1];
  strcpy(topicChar3, topicString3.c_str());

  String msgString1 = "...";
  char publishMsg1[msgString1.length() + 1];
  strcpy(publishMsg1,msgString1.c_str());
  
  String msgString2 = "...";
  char publishMsg2[msgString2.length() + 1];
  strcpy(publishMsg2,msgString2.c_str());


   
  mqttClient.publish(topicChar1,publishMsg1);
  
  mqttClient.publish(topicChar2,publishMsg2);

}
*/
//Abandon
