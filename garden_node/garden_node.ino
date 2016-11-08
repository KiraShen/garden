
/**************基本配置****************/
#include "node_usr.h"
//节点地址
String node_address;
//盆栽-土壤湿度
int pot_port[5]={POT_PORT1,POT_PORT2,POT_PORT3,POT_PORT4,POT_PORT5};//盆栽土壤湿度接口
int pot_humi[5]={-1,-1,-1,-1,-1};//盆栽湿度
int pot_ID[5]={1,2,3,4,5};//盆栽ID
//水泵开启/关闭
#define PUMP_ENABLE digitalWrite(PUMP_PORT,HIGH)
#define PUMP_DISABLE digitalWrite(PUMP_PORT,LOW)
//电磁阀
int solenoid_port[5]={SOLENOID_PORT1,SOLENOID_PORT2,SOLENOID_PORT3,SOLENOID_PORT4,SOLENOID_PORT5};//电磁阀接口
//int solenoid_sta[5]={0,0,0,0,0};//默认电磁阀关闭
//盆栽状态
int water_sta[5]={0,0,0,0,0};//盆栽浇水状态
int hungry_count[5]={0,0,0,0,0};//盆栽口渴次数，
int water_count[5]={0,0,0,0,0};//盆栽浇水次数
int urgent_sta[5]={0,0,0,0,0};//干燥紧急状态
int have_water_sta[5]={0,0,0,0,0};//浇水完成状态
/**************信号灯配置****************/
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
#define LED_RED strip.setPixelColor(0, strip.Color(255,0,0))
#define LED_GRE strip.setPixelColor(0, strip.Color(0,255,0))
#define LED_YEL strip.setPixelColor(0, strip.Color(255,255,0))
#define LED_BLU strip.setPixelColor(0, strip.Color(0,0,255))
#define LED_CLO strip.setPixelColor(0, strip.Color(0,0,0))
/**************时间设定****************/
unsigned long lastTime = millis();
unsigned long water_time[5]={0,0,0,0,0};
unsigned long off_time[5]={0,0,0,0,0};
/**************OLED配置****************/
#include <U8glib.h>
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
#define setFont_L u8g.setFont(u8g_font_fur20)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
#define setFont_M u8g.setFont(u8g_font_9x18)
/**************环境参数****************/
float in_temp,in_humi,in_ligh=0.0;
int in_sta=0;
String zig_rec_str="";
/************电磁阀开启/关闭**************/
void solenoid_on(int n){
  digitalWrite(solenoid_port[n],HIGH);
  //solenoid_sta[n]=1;
}
void solenoid_off(int n){
  digitalWrite(solenoid_port[n],LOW);
  //solenoid_sta[n]=0;
}

/************浇水开启/关闭**************/
void water_on(int i){
  solenoid_on(i);
  delay(50);
  PUMP_ENABLE;
  LED_BLU;
  strip.show();
  Serial.print(pot_ID[i]);
  Serial.println("--pot water on...");
  water_sta[i]=1;
  //记录触发时间
  if(lastTime > millis())
    water_time[i]=0;
  else
    water_time[i]=millis();
}
void water_off(int i){
    PUMP_DISABLE;
    delay(50);
    solenoid_off(i);
  	LED_CLO;
  	strip.show();
  	Serial.print(pot_ID[i]);
    Serial.println("--pot water off...");
  	//water_sta[i]=0;
    //记录触发时间
    if(lastTime > millis())
      off_time[i]=0;
    else
      off_time[i]=millis();
    have_water_sta[i]=1;
  	//清0浇水触发时间
  	water_time[i]=0;
}

/**************限幅滤波****************/
int filter(int now_data, int prev_data,int n){

    if(prev_data >= 0){
      if(((now_data - prev_data) < DEVIATION) || ((now_data - prev_data) < DEVIATION)){
        return now_data;
      }
      else{
        Serial.print(pot_ID[n]);
        Serial.print("--WARNING: pot humidty abnormal, data:");
        Serial.println(now_data);
        Serial1.println("{"+String(pot_ID[n])+"21}");
        return prev_data; 
      }
    }else{
      return now_data;
    }
}

/************获取盆栽土壤湿度**************/
int get_pot_humi(int i){
    int temp;
    //temp = map(analogRead(pot_port[i]),0,1023,0,99);
    temp = random(8,41); 
    return temp;
}

/************设置盆栽土壤湿度**************/
void set_pot_humi(){
  int prev_humi,now_humi;
  for(int i=0; i<5; i++){
    prev_humi = pot_humi[i];
    now_humi = filter(get_pot_humi(i),prev_humi,i);
    pot_humi[i] = now_humi;
    //Serial.print(pot_ID[i]);
    Serial.print(String(pot_ID[i])+"--humidty:");
    Serial.println(pot_humi[i]);
    if(pot_humi[i]<=30){
      hungry_count[i]++;
      //Serial.print(pot_ID[i]);
      Serial.print("------hungry:");
      Serial.println(hungry_count[i]);
      if(pot_humi[i]<=10){
        urgent_sta[i]=1;
        LED_YEL;
        strip.show();
        Serial.print(pot_ID[i]);
        Serial.println("--WARNING: pot is very dry.");
      }
    }
  }
}

/**************OLED显示*********************/
void draw(){
    setFont_S;
    for(int x=10,y=10,i=0;i<5;i++,y+=10){
      u8g.setPrintPos(x, y);
      u8g.print("humidty-");
      u8g.print(pot_ID[i]);
      u8g.print(":");
      u8g.print(pot_humi[i]);
    }
    u8g.setPrintPos(10, 60);
    u8g.print("Microduino SmartGarden node");
  
}

//保存环境温湿度
void save_in(String str)
{
  char tmp[20];
  char *p;
  String temp;
  str.toCharArray(tmp,sizeof(tmp));
  //Serial.println(tmp);
  p=strtok(tmp,",");
  if(p) {
    temp=p;
    in_temp=temp.toFloat();
  }
  Serial.print("ENVIRONMENT=");
  Serial.print("T:");
  Serial.print(in_temp);
  temp=strtok(NULL,",");
  in_humi=temp.toFloat();
  Serial.print(";H:");
  Serial.print(in_humi);
  temp=strtok(NULL,"|");
  in_ligh=temp.toFloat();
  Serial.print(";L:");
  Serial.println(in_ligh);
  in_sta=1;
}

/**************接受数据*********************/

void zigbee_rec() {
  String temp;
  char data[30];
  char *p;
  while (Serial1.available() > 0)  
  {
    zig_rec_str += char(Serial1.read());
    delay(2);
  }

  if (zig_rec_str.length() > 10 && zig_rec_str[0] == '+' && zig_rec_str[9] == ',')
  {   
    LED_GRE;
    strip.show();
    String str;
    Serial.print("RECEIVE= ");
    Serial.println(zig_rec_str);
    node_address=zig_rec_str.substring(5,9);
    str=zig_rec_str.substring(20+ADD,zig_rec_str.length()-1);
    zig_rec_str=zig_rec_str.substring(11+ADD,18+ADD);//除去字符串最后的/t /0 ‘}’zig_rec_str.length()-22
    //zig_rec_str=zig_rec_str.substring(15,22);//除去字符串最后的/t /0 ‘}’zig_rec_str.length()-22
    //Serial.println(zig_rec_str);
    if(zig_rec_str=="REQDATA"||zig_rec_str=="REQDAT"||zig_rec_str=="REQDATA}"){
        Serial.println("cmd:REQDATA");
        Serial1.println("{"+String(pot_humi[0])+","+String(pot_humi[1])+","+String(pot_humi[2])+","+String(pot_humi[3])+","+String(pot_humi[4])+"}");
    }  
    else{
      Serial.println("----ERROR: receive data error.");
      LED_RED;
      strip.show();
    }
    save_in(str);
    zig_rec_str="";
  }
  

}

/**************主程序*********************/
void setup()
{
  for(int i=0;i<5;i++){
    pinMode(pot_port[i],INPUT_PULLUP);
    pinMode(solenoid_port[i], OUTPUT);
    digitalWrite(solenoid_port[i],LOW);
  }
  pinMode(PUMP_PORT,OUTPUT);
  digitalWrite(PUMP_PORT,LOW);

  strip.begin();
  strip.show();
  Serial.begin(BAUD0);
  Serial1.begin(BAUD1);
  delay(500);
}


void loop()
{
      //等待接收数据并处理数据
    zigbee_rec();

  //每一段时间记录盆栽数据
    if (lastTime > millis())
        lastTime = millis();
    if (millis() - lastTime > SET_INTERVAL_TIME){
        lastTime = millis();
        set_pot_humi();
    }

    //条件达到浇水开启/时间达到浇水关闭
    for(int i=0;i<5;i++){
      if((urgent_sta[i])&&(!water_sta[i])){//紧急触发浇水
        Serial.print(pot_ID[i]);
        Serial.print("--URGENT: more dry, water...");
        water_on(i);
        urgent_sta[i]=0;
        hungry_count[i]=0;
        Serial1.println("{"+String(pot_ID[i])+"12}");
      }
        //查询是否需要浇水
      if((!water_sta[i])&&(hungry_count[i]>=HUNGRY)&&(in_sta)){
          //环境条件
        if((in_temp<=30)&&(in_ligh<=30)){
          Serial.print(pot_ID[i]);
          Serial.print("--HUNGRY: dry, water...");
          water_on(i);
          hungry_count[i]=0;
          Serial1.println("{"+String(pot_ID[i])+"11}");
        }else{
          LED_YEL;
          strip.show();
          Serial.print(pot_ID[i]);
          Serial.println("--WARNING: dry but inappropriate.");
          Serial1.println("{"+String(pot_ID[i])+"22}");
        }
        in_sta=0;
      }
      if(water_sta[i]&&(!have_water_sta[i])){//到时间关闭浇水
          if(millis() - water_time[i] > WATER_INTERVAL_TIME) {
            water_off(i); 
            //清除本次湿度，直接获取湿度
            pot_humi[i]=get_pot_humi(i);
          }
      }
      if(have_water_sta[i]){//刚刚浇水，一段时间再浇水
          if(millis() - off_time[i] > OFF_INTERVAL_TIME) {
            water_sta[i]=0;
            have_water_sta[i]=0; 
            off_time[i]=0;
            //清除本次湿度，直接获取湿度
            pot_humi[i]=get_pot_humi(i);
          }
      }
    }

    //OLED显示
    u8g.undoRotation();
    u8g.firstPage();
    do{
      draw();
    } while(u8g.nextPage());
}

