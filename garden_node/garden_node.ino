#define WATER_INTERVAL_TIME 5000
#define INTERVAL_TIME 30000
#define THRESHOLD_DRY 500

#define PUMP_PORT 13
#define PUMP_ENABLE digitalWrite(PUMP_PORT,HIGH)
#define PUMP_DISABLE digitalWrite(PUMP_PORT,LOW)

String zig_rec_str="";

unsigned long lastTime = millis();
unsigned long water_time[5];

//node zigbee UART is D2,D3
int solenoid_port[5]={7,8,9,10,11};//电磁阀接口
int solenoid_sta[5]={0,0,0,0,0};//默认电磁阀关闭
int pot_PORT[5]={A0,A1,A2,A3,A6};//盆栽土壤湿度接口
int pot_NO[5]={0,0,0,0,0};//默认节点没有接入盆栽
float pot_humi[5]={0.0,0.0,0.0,0.0,0.0};//盆栽湿度
unsigned int pot_ID[5]={1,2,3,4,5};

float h_temp[5]={0.0,0.0,0.0,0.0,0.0};
//环境参数
float in_temp,in_humi,in_ligh;
//环境参数全部保存完毕状态
//每次获取以后置1
//每次判断以后清0
int in_sta=0;//环境参数收集完成
int water[5]={0,0,0,0,0};//某一盆栽浇水状态

//使能电磁阀
void solenoid_on(int n){
  digitalWrite(solenoid_port[n],HIGH);
  solenoid_sta[n]=1;
}
//关闭电磁阀
void solenoid_off(int n){
  digitalWrite(solenoid_port[n],LOW);
  solenoid_sta[n]=0;
}

//启动浇水
void water_on(int i){
  solenoid_on(i);
  PUMP_ENABLE;
  //water_sta=1;
  water[i]=1;
  Serial.println();
  Serial.print(pot_ID[i]);
  Serial.println("--------pot water on...");
}
//关闭浇水
void water_off(int i){
  solenoid_off(i);
  PUMP_DISABLE;
  //water_sta=0;
  water[i]=0;
  Serial.print(pot_ID[i]);
  Serial.println("--------pot water off...");
}

//获取盆栽土壤湿度
float get_pot_humi(int i){
  float temp;
  //temp = analogRead(pot_PORT[i])/1.0;//一位小数
  temp = random(99)/1.00;//test
  return temp;
}

//查询接入盆栽状态

//判断电磁阀开启时机

//节点盆栽湿度数据发送
void  send_pot_humi(){
  //Serial.println("node send data to coor:");
   for(int i=0;i<5;i++){
        pot_humi[i]=get_pot_humi(i);
        h_temp[i]=pot_humi[i];
        Serial.print(pot_ID[i]);
        Serial.print(",data:");
        Serial.println(pot_humi[i]);
    }
    Serial1.println("{"+String(pot_humi[0])+","+String(pot_humi[1])+","+String(pot_humi[2])+","+String(pot_humi[3])+","+String(pot_humi[4])+"}");

}
void setup()
{
  for(int i=0;i<5;i++){
    pinMode(solenoid_port[i], OUTPUT);
    digitalWrite(solenoid_port[i],LOW);
  }
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(500);
}

void loop()
{
  zigbee_rec();
//////////判断是否浇水////////////
  if(in_sta&&in_temp)//满足什么温度、湿度、光照条件???
  {
    for(int pot_i=0;pot_i<5;pot_i++)
    {
      //Serial.println(h_temp[pot_i]);
      if(!water[pot_i])
      {
      if(h_temp[pot_i]>90.0)//满足什么土壤湿度条件???
      {
        //Serial.print(h_temp[pot_i]);
        water_on(pot_i);
        if(millis()+WATER_INTERVAL_TIME<WATER_INTERVAL_TIME)//overflow
          {water_time[pot_i]=1000;}
        else
          {
            water_time[pot_i]=millis();
            //Serial.println(water_time[pot_i]);
          }
      }
      
      }
    }
    delete_in();
    in_sta=0;
  }
 //////////////////////////////////////////
  ////每隔着一段时间关闭浇水//////
  for(int j=0;j<5;j++)
  {
    if(water[j])
       if (millis() - water_time[j] > WATER_INTERVAL_TIME) {
          water_off(j);
          water_time[j]=0;
       }
  }
//////////////////////////////////////
}

void zigbee_rec() {
  String temp;
  char data[30];
  char *p;
  //char *f=",}";
  //float s[4];
  while (Serial1.available() > 0)  
  {
    zig_rec_str += char(Serial1.read());
    delay(2);
  }
/*  if (zig_rec_str.length() > 10)
  {
    Serial.println(zig_rec_str);
      zig_rec_str="";
  }  */

//命令格式<20 
  if (zig_rec_str.length() > 10 && zig_rec_str[0] == '+' && zig_rec_str[9] == ',')
  { 
    String str;
    str=zig_rec_str.substring(20,zig_rec_str.length()-1);
    zig_rec_str=zig_rec_str.substring(11,zig_rec_str.length()-22);//除去字符串最后的/t /0 ‘}’
    //Serial.println(zig_rec_str);
    if(zig_rec_str=="REQDATA"||zig_rec_str=="REQDAT"){
      Serial.println("cmd:REQDATA");
      send_pot_humi();
    }  
    else
      Serial.println("cmd ERROR;");
    Serial.println("RECEIVE:");
    save_in(str);
    zig_rec_str="";
    
  }
  

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
  Serial.print("temp:");
  Serial.print(in_temp);
   
  temp=strtok(NULL,",");
  in_humi=temp.toFloat();
  Serial.print(";humi:");
  Serial.print(in_humi);
   
  temp=strtok(NULL,"|");
  in_ligh=temp.toFloat();
  Serial.print(";ligh:");
  Serial.println(in_ligh);

  for(int i=0;i<5;i++){
        pot_humi[i]=get_pot_humi(i);
    }
  in_sta=1;
  /*
  if(in_temp&&in_ligh&&in_humi)
  {
        for(int pot_i=0;pot_i<5;pot_i++)
        {
          Serial.print(h_temp[pot_i]);
          if(!water[pot_i])
          {
          if(h_temp[pot_i]>90.0)
          {
            //Serial.print(h_temp[pot_i]);
            water_on(pot_i);
            if(millis()+WATER_INTERVAL_TIME<WATER_INTERVAL_TIME)//overflow
              {water_time[pot_i]=1000;}
            else
              {
                water_time[pot_i]=millis();
                //Serial.println(water_time[pot_i]);
              }
          }
          
          }
        }
        delete_in();
        //in_sta=0;
  }
  */
}

//清楚环境参数
void delete_in()
{
  in_temp=0;
  in_humi=0;
  in_ligh=0;
}

