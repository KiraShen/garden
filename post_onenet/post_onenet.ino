/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

 String s="+ZBD=0014,{37.44,55.00,60.09,55.50,88.88}|37.50,66.21,244.93|";
//保存环境温湿度

float in_data[8];//接受解析得到的数据
String data_str;
void process_str(String str)
{
  char tmp[60];
  char *p;
  String temp;
  char *delim=",}|";
  str.toCharArray(tmp,sizeof(tmp));
  //Serial.println(tmp);
  p=strtok(tmp,",");
  if(p) {
    temp=p;
    in_data[0]=temp.toFloat();
  }
  //Serial.print("pot0");
  Serial.println(in_data[0]);
  
  for(int i=1;i<8;i++)
  {
    temp=strtok(NULL,delim);
    in_data[i]=temp.toFloat();
    Serial.println(in_data[i]);
  }
}

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

//#include <ESP8266HTTPClient.h>

#include <HttpPacket.h>//onenet http arduino lib
#include <ArduinoJson.h>

#define BAUD_RATE 115200
#define INTERVAL_TIME 10000
//#define SERBER "api.heclouds.com"
//#define SERBER_PORT 80
 #define DEVICE_ID "3497676"
 #define API_KEY "3ebQkaP3cH4gZ3QCVt7WFg5p0R8="
 //#define SENSOR_ID "temp"
char *sensor_id[8]={"humidity0","humidity1","humidity2","humidity3","humidity4","temp","humidity","light"};
String wifi_rec_str="";
int rec_sta=0;

WiFiClient client;
ESP8266WiFiMulti WiFiMulti;
HttpPacketHead packetHead;

char *ssid     = "Angelababy";//这里是我的wifi，你使用时修改为你要连接的wifi ssid
char *password = "00000000";//你要连接的wifi密码
char *host = "api.heclouds.com";//修改为你建立的Server服务端的IP地址
//char *host = 183.230.40.33
//port 80
unsigned int tcpPort = 80;//修改为你建立的Server服务端的端口号
//test
//char *host = "192.168.1.160";
//unsigned int tcpPort = 1000;

unsigned long lastTime = millis(); 
//StaticJsonBuffer<200> jsonBuffer;
//char jsonstr[200] ={0};
//serial ready...
void initSerial() {
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    ;  // wait for serial port to initialize
  }
  Serial.println("Serial ready");
}


void initWifi(){
    Serial.print("Connecting to ");//写几句提示，哈哈
    Serial.println(ssid);
 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)                                    
    {
        delay(500);
        Serial.print(".");
    }//如果没有连通向串口发送.....
    delay(3000);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址

}

void addJsonDataRecord(char id[], float value, JsonArray& array) {
  JsonObject &myJson_Json = array.createNestedObject();//myJson数据流中增加一个内置json对象
  myJson_Json["id"] = id; //为内置json对象加入键id
  JsonArray &myJson_Jsondata = myJson_Json.createNestedArray("datapoints");//内置json加入数据流
  JsonObject &myJson_Json_Json = myJson_Jsondata.createNestedObject();//内置json数据流中新增一个内置json对象
  myJson_Json_Json["value"] = value;//为内置json对象加入键value

  /*
  jsondata:
  
  {
      “datastreams”:[
      {
          “id”:”key1”,
          “datapoints”:
          {
            “value”:”value1”
          }
      }
      {
          “id”:”key2”,
          “datapoints”:
          {
            “value”:”value2”
          }
      }
      ]
  }
  */
}
void postDataToOneNet(char *id,float data)
{
        //合成POST请求
    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& myJson = jsonBuffer.createObject();//新建一个Json对象
    //myJson["id"]="id_key";
    JsonArray& myJsondata = myJson.createNestedArray("datastreams");//myJson中增加数据流
    addJsonDataRecord(id, data, myJsondata);//数据流中添加一个数据记录

    char jsonstr[200];
    int num = myJson.printTo(jsonstr, sizeof(jsonstr));
    Serial.print("jsonstr:");
    Serial.println(jsonstr);
    //char *p = "{\"datastreams\":[{\"id\":\"temp\",\"datapoints\":[{\"value\":50}]}]}";
    packetHead.setHostAddress(host); 
    packetHead.setDevId(DEVICE_ID); 
    packetHead.setAccessKey(API_KEY); 
    // packetHead.setDataStreamId("<datastream_id>");    //datastream_id
    // packetHead.setTriggerId("<trigger_id>");
    // packetHead.setBinIdx("<bin_index>");

    /*create the http message about add datapoint */
    packetHead.createCmdPacket(POST, TYPE_DATAPOINT, jsonstr);

    
    //packetHead.createCmdPacket(POST, TYPE_DATAPOINT, p);
    //int httpLength = strlen(packetHead.content) + num;
    if (strlen(packetHead.content)){
      //Serial.printf("packetHead:" );
      //Serial.println(packetHead.content);
    }
    else{
      Serial.println("post packet none");
    }
    if(jsonstr!=NULL)
    {
       //Serial.println(jsonstr);
    }
    char buf[300];
    memset(buf, 0, sizeof(buf));
    strcat(buf,packetHead.content);
    strcat(buf,jsonstr);
    Serial.println(buf);
    client.print(buf);     
   
  }

//接受数据
void wifi_rec() {
  if (Serial.available() > 0) {
      wifi_rec_str = Serial.readStringUntil('\n');
  }
  
  //收到数据
  if(wifi_rec_str.length() > 20 && wifi_rec_str[0] == '+' && wifi_rec_str[9] == ',')
  {
    Serial.print("receive data:");
    //+ZBD=0014,{0.00,55.00,60.00,55.00,88.00}|TEMP,HUMI,LIGH|
    Serial.println(wifi_rec_str);
    data_str=wifi_rec_str.substring(11,wifi_rec_str.length()-1);
    process_str(data_str);
    rec_sta=1;
    wifi_rec_str = "";
  }
  else
  {
    //Serial.println(wifi_rec_str);
    wifi_rec_str = "";
  }
    
}


void setup() {
    initSerial();
    initWifi();
}

void loop() {
  
  wifi_rec();
  
  if (lastTime > millis())
    lastTime = millis();
  if (millis() - lastTime > INTERVAL_TIME) {
    //Serial.println("new post...");
    if(rec_sta){
    while (!client.connected())
    {
        if (!client.connect(host, tcpPort))
        {
            //Serial.println("connect server...");
            //client.stop();
            delay(100);
        }
        
    }
    //int sensor_data = random(99);
    //postDataToOneNet(sensor_data);
    //client.print("Send this data to server");

    //data_str=s.substring(11,s.length()-1);
    //process_str(data_str);
    
    //update pot humidity
    for(int i=0;i<5;i++)
    {
      while (!client.connected())
      {
          if (!client.connect(host, tcpPort))
          {
              //Serial.println("connect server...");
              //client.stop();
              delay(100);
          }
          
      }
      //in_data[i] = random(99);
      postDataToOneNet(sensor_id[i], in_data[i]);
      delay(200);
    }
    client.stop();
    //updata temp humidity light
    for(int i=5;i<8;i++)
    {
      while (!client.connected())
      {
          if (!client.connect(host, tcpPort))
          {
              //Serial.println("connect server...");
              //client.stop();
              delay(100);
          }
          
      }
      //in_data[i] = random(99);
      postDataToOneNet(sensor_id[i], in_data[i]);
      delay(200);
    }
    client.stop();
    rec_sta=0;
    }
    lastTime=millis();
    
  }
  
}

