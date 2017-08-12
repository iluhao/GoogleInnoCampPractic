/*
  本代码在使用时需要修改若干信息，详见手册
*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <ArduinoJson.h>
int pinDHT11 = D2;
SimpleDHT11 dht11;


int Humidity = 0;
int Temperature = 0;

const char* ssid = "iPhone";//连接的路由器的名字
const char* password = "wwp123456";//连接的路由器的密码
const char* mqtt_server = "123.206.127.199";//服务器的地址 iot.eclipse.org是开源mqttbroker
long lastMsg = 0;//存放时间的变量
char msg[100];//存放要发的数据

WiFiClient espClient; //定义一个esp8266wifi对象
PubSubClient client(espClient);//定义一个mqttpubsub方法的客户端对象


//LingShun Lab

int input1 = D1; // 定义uno的pin 5 向 input1 输出
int input2 = D2; // 定义uno的pin 6 向 input2 输出
int input3 = D3; // 定义uno的pin 9 向 input3 输出
int input4 = D4; // 定义uno的pin 10 向 input4 输出
int flag = 0;


void setup_wifi() {//自动连WIFI接入网络
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}



void callback(char* topic, byte* payload, unsigned int length) {//用于接收数据
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);//串口打印接收到的数据
  }
  if (!flag && (char)payload[0] == 'o') {
    flag = 1;
    for (int i = 0; i <= 1023; i++) {
      analogWrite(D1, i);
      delay(3);
    }
  }
  if (flag && (char)payload[0] == 'c') {
    flag = 0;
    //    digitalWrite(input1, LOW);


    for (int i = 1023; i >= 0; i--) {
      analogWrite(D1, i);
      delay(3);
    }
  }

  Serial.println();//换行
}



void reconnect() {//等待，直到连接上服务器
  while (!client.connected()) {//如果没有连接上
    if (client.connect("test") + random(999999999)) { //接入时的用户名，尽量取一个很不常用的用户名
      client.subscribe("command1");//订阅接收外来的数据时的topic
    } else {
      Serial.print("failed, rc=");//连接失败
      Serial.print(client.state());//返回目前状态
      Serial.println(" try again in 5 seconds");//延时5秒后重新连接
      delay(5000);//保存此状态延时5秒
    }
  }
}




void setup() {//初始化程序，只运行一遍
  Serial.begin(9600);//设置串口波特率（与烧写用波特率不是一个概念）
  //初始化各IO,模式为OUTPUT 输出模式
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(input3, OUTPUT);
  pinMode(input4, OUTPUT);

  /*此处进行传感器或者动作初始化设置*/
  setup_wifi();//自动连WIFI接入网络
  client.setServer(mqtt_server, 1883);//1883为端口号
  client.setCallback(callback); //用于接收服务器接收的数据
}



void loop() {//主循环
  //  DhT11();
  //  encodeJson();
  //  show();
  reconnect();//确保连上服务器，否则一直等待。
  client.loop();//MUC接收数据的主循环函数。
  /*此处调用传感器函数或者动作控制函数*/
  long now = millis();//记录当前时间
  if (now - lastMsg > 1000) {//每隔1秒进行if内部语句
    //client.publish("dht11", msg);//发送数据，其中temperature是发出去的topic
    lastMsg = now;//刷新上一次发送数据的时间
  }
}




/*
  此处添加传感器函数或者动作控制函数
*/

//void show() {
//  Serial.print(Temperature); Serial.print(" *C, ");
//  Serial.print(Humidity); Serial.println(" H");
//  Serial.println("json");
//  Serial.println(msg);
//}

//void DhT11() {
//  byte temperature = 0;
//  byte humidity = 0;
//  int err = SimpleDHTErrSuccess;
//  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
//    //  Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
//    return;
//  }
//  Humidity = (int)humidity;
//  Temperature = (int)temperature;
//  delay(1000);
//}
//
//
//
//void encodeJson() {
//  DynamicJsonBuffer jsonBuffer;
//  JsonObject& root1 = jsonBuffer.createObject();
//  root1["Humidity"] = Humidity;
//  root1["Temperature"] = Temperature;
//  root1.printTo(msg);
//}




