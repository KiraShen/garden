#include "coor_usr.h"
unsigned long lastTime0 = millis();
unsigned long lastTime = millis();
unsigned long send_time;

String rout_dev[max_rout_num] = {};
unsigned int rout_dev_num;
String node_dev[max_node_num] = {};
unsigned int node_dev_num;

int rec_sta = 0;
unsigned int dev_i=0;

#include <Wire.h>
#include <AM2321.h>
float readByAM2321(int num) {
  AM2321 am2321;
  am2321.read();
  float sensor_tem=am2321.temperature/10.0;
  float sensor_hum=am2321.humidity/10.0;
  if(num==1) {
    return sensor_tem;
  }
  else if(num==2) {
    return sensor_hum;
  }else {
    return 0.0;
  }
}

//获取环境温度、湿度、光照
float in_temp,in_humi,in_ligh=0.0;
void get_in(){
  //in_temp = readByAM2321(1);
  //in_humi = readByAM2321(2);
  //in_ligh =analogRead(LIGHT_PORT)/1.0; //光照
  //in_ligh  = map(in_ligh, 0.0, 1023.0, 0.0, 99.0);
  in_temp = random(20,51);
  in_humi = random(20,51);
  in_ligh = random(20,51);
}

//OLED显示
#include <U8glib.h>
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
#define setFont_L u8g.setFont(u8g_font_fur20)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
#define setFont_M u8g.setFont(u8g_font_9x18)
void draw(){
    setFont_S;
    u8g.setPrintPos(0, 15);
    u8g.print("hum:");
    u8g.setPrintPos(27, 15);
    u8g.print(in_humi);
    u8g.setPrintPos(65, 15);
    u8g.print("lux:");
    u8g.setPrintPos(90, 15);
    u8g.print(in_ligh);
    u8g.setPrintPos(0, 30);
    u8g.print("temp:");
    u8g.setPrintPos(28, 30);
    u8g.print(in_temp);
    u8g.setPrintPos(10, 60);
    u8g.print("Microduino SmartGarden coor");
  
}

String zig_rec_str;


//发送请求数据的命令
void send_req_data_cmd(String dev_str){
  Serial.print("cmd to:");
  Serial.println(dev_str);
  //get_in();
  Serial1.println("+ZBD="+dev_str+","+"{REQDATA}"+"|"+in_temp+","+in_humi+","+in_ligh+"|");
}

//接受数据
void zigbee_rec() {
  if (Serial1.available() > 0) {
      zig_rec_str = Serial1.readStringUntil('\n');
  }

  //get zigbee dev id
  if (zig_rec_str.length() > 20 && zig_rec_str[0] == '+' && zig_rec_str[7] == ',' && zig_rec_str[12] == ',')
  {
    Serial.print("a new dev :");
    zigbee_get_id(zig_rec_str);
  }
  
  //收到节点数据
  if(zig_rec_str.length() > 20 && zig_rec_str[0] == '+' && zig_rec_str[9] == ',')
  {
    //Serial.print("receive data:");
    Serial.println("H,"+zig_rec_str);//数据转发给wifi
    //Serial.println("receive over.");
    zig_rec_str = "";
  }
  else if(zig_rec_str.length() < 18 && zig_rec_str[0] == '+' && zig_rec_str[9] == ','){
    Serial.println("S,"+zig_rec_str);
    zig_rec_str = "";
  }
  else
    zig_rec_str = "";
    //Serial.println(zig_rec_str);
}

//获取加入设备的ID
void zigbee_get_id(String str)
{
    int add_sta=1;
    String MacString = "";
    String StyleString = "";

    //get zigbee dev type
    for (int i = 5; i < 7; i++)
      StyleString += zig_rec_str[i];
    //get zigbee dev mac
    for (int i = 8; i < 12; i++)
      MacString += zig_rec_str[i];

    for(int a=0;a<max_rout_num;a++)
    {
      for(int b=0;b<max_node_num;b++)
      {
        if((MacString==rout_dev[a])||(MacString==node_dev[b]))
        {
          Serial.println("old dev.");
          add_sta=0;
          break;
        }
      }
    }
    if(add_sta)
    {
    if (StyleString == "01")
    {
      rout_dev_num++;
      rout_dev[rout_dev_num-1] = MacString;
      if(rout_dev_num >= max_rout_num)
      {
        Serial.println("rout dev overflow.");
      }
      Serial.println("rounter. address:" + rout_dev[rout_dev_num-1]);
      Serial.println(rout_dev_num);
    }
    else if (StyleString == "02")
    {
      node_dev_num++;
      node_dev[node_dev_num-1] = MacString;
      if(node_dev_num >= max_node_num)
      {
        Serial.println("node dev overflow.");
      }
      Serial.println("node. address:" + node_dev[node_dev_num-1]);
      Serial.println(node_dev_num);
    }
    else
    {
      Serial.println("unkown zigbee dev.");
    }
    }
    //Serial.println(zig_rec_str);
    zig_rec_str = "";
}



void setup(void) {
  //初始化串口波特率
  Serial.begin(BAUD0);
  Serial1.begin(BAUD1);
}
void loop(){
    zigbee_rec();

    if (lastTime0 > millis())
        lastTime0 = millis();
    if (millis() - lastTime0 > GET_INTERVAL_TIME){
        lastTime0 = millis();
        get_in();
        Serial.println("E,{"+String(in_temp)+','+String(in_humi)+','+String(in_ligh)+"}");
    }

  //如果有节点加入网络，每一段时间发送请求节点数据命令，请求数据
  if(node_dev_num){
    if (lastTime > millis())
      lastTime = millis();
    if ((millis() - lastTime > SEND_INTERVAL_TIME)&&(dev_i<node_dev_num)) {
      //get_in();
      send_req_data_cmd(node_dev[dev_i]);
      lastTime = millis();
      dev_i++;
      if(dev_i>=node_dev_num)
        {dev_i=0;}
    }

  }

  //OLED显示
    u8g.undoRotation();
    u8g.firstPage();
    do{
      draw();
    } while(u8g.nextPage());

}

