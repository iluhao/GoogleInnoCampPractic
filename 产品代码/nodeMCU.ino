#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <ArduinoJson.h>


SoftwareSerial mySerial(4, 5); // RX, TX
const char* ssid = "iPhone";
const char* password ="wwp123456";
const char* mqtt_server = "123.206.127.199";

WiFiClient espClient;    //定义对象
PubSubClient client(espClient);


char msg[50];   //信息
char msg0[50];
char sensor[100]="123";
int value;

//继电器
int Appkey=-1;
int border=30;
int jidianqi=D2;



float temperature = 0;
int pinDHT11 = D7;//温度传感器
SimpleDHT11 dht11;


void setup() {
  pinMode(jidianqi,OUTPUT);
  digitalWrite(jidianqi,HIGH);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  Serial.begin(9600);
  mySerial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(500);
    Serial.print(".");
    digitalWrite(BUILTIN_LED, LOW);
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}




void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else { 
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client2")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("HIGH", "success");
      // ... and resubscribe
//      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  int changemod;
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
temperature=getT();  //实时采集温度
Serial.print("Temperature:");
Serial.println(temperature);
check();
changemod=changemode();//模式选择
if(changemod==1)
    LIGHT();     //光感控制模式
else
  {
    phone();     //手机远程控制
    }
encodeJson();
delay(200);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void check() 
{
     //当继电器打开时，检测温度。（温度高于25度灯亮；反之亦然）
 
  if(temperature>border)
  {
    Serial.println("Alarm!!!!!");
    digitalWrite(D1,HIGH);
    digitalWrite(jidianqi,LOW);
  }
  
  if(temperature<=border)
  {
    //Serial.println("Normal");
    digitalWrite(D1,LOW);
    if(value<500||Appkey==1)
        digitalWrite(jidianqi,HIGH);
    else
        digitalWrite(jidianqi,LOW);
  }
 
}
  
int changemode()     //模式切换 
{ 
  int mod=digitalRead(D6);
  return mod;
  }


void LIGHT(){          //采集光强
  value = analogRead(A0);
 // Serial.println(value);
  Serial.println();
  sensor[0]=value/100+'0';
  sensor[1]=value/10%10+'0';
  sensor[2]=value%100%10+'0';
  sensor[3]='\0';
 // Serial.println(sensor);
  }

void encodeJson(){       //转成json数据
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root1 = jsonBuffer.createObject();
  root1["CLOSE"] = "oo";

  JsonObject& root2 = jsonBuffer.createObject();
  root2["CLOSE"] = "cc";
  
  root1.printTo(msg);
  root2.printTo(msg0);
  
 // Serial.println(msg);
 // Serial.println(msg0);
  
   if(value>500)
   {
    client.publish("chuanglian",msg);
    //digitalWrite(jidianqi,LOW);
   }
  else {
    client.publish("chuanglian",msg0);
    //digitalWrite(jidianqi,HIGH);
  }
  }





 

void phone()    //手机模式下收到的数据
{
   while(Serial.available())
  {
    char c=Serial.read();
    if(c=='a')
    {
      digitalWrite(jidianqi,HIGH);
      Appkey=1;
      Serial.println("i have TurnOn AppKeyOn");
    }
    else if(c=='b')
    {
      digitalWrite(jidianqi,LOW);
      Serial.println("i have TurnDown AppKeyOn");
      Appkey=0;
    }
  }

  if(Appkey==1)       //手机发送，接收到a打开继电器
  {
    digitalWrite(jidianqi,HIGH);
  }
   if(Appkey==0)
  {
    digitalWrite(jidianqi,LOW);
  }
  
}


double getT()      //采集温度
{
  byte temperature=0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return temperature;
  }
  
 // Serial.print("Sample OK: ");
  //Serial.print((int)temperature); Serial.print(" *C, "); 
  //Serial.print((int)humidity); Serial.println(" H");
  return temperature;
  }


