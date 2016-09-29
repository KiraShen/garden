//协调器 代码



#define SEND_INTERVAL_TIME 15000 
#define max_rout_num 2
#define max_node_num 5

unsigned long lastTime = millis();
unsigned long send_time;
float sen_data[5]={0.0,0.0,0.0,0.0,0.0};
int sen_sta[5]={0,0,0,0,0};
String zig_rec_str = "";

String rout_dev[max_rout_num] = {};
unsigned int rout_dev_num;
String node_dev[max_node_num] = {};
unsigned int node_dev_num;

int rec_sta = 0;
unsigned int dev_i=0;

float in_temp=0.0;
float in_humi=0.0;
float in_ligh=0.0;
//获取环境温度、湿度、光照
void get_temp_humi(){
  //test
  in_temp=random(99)/1.00;//test
  in_humi=random(99)/1.00;//test
  in_ligh=random(99)/1.00;//test
}

void setup(void) {
  //初始化串口波特率
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop(){
  zigbee_rec();

  //如果有节点加入网络，每一段时间发送请求节点数据命令，请求数据
  if(node_dev_num){
    if (lastTime > millis())
      lastTime = millis();
    if ((millis() - lastTime > SEND_INTERVAL_TIME)&&(dev_i<node_dev_num)) {
      send_req_data_cmd(node_dev[dev_i]);
      lastTime = millis();
      dev_i++;
      if(dev_i>=node_dev_num)
        {dev_i=0;}
    }

  }
  
}

//发送请求数据的命令
void send_req_data_cmd(String dev_str){
  Serial.print("send require data cmd to ");
  Serial.println(dev_str);
  get_temp_humi();
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
    Serial.print("receive data:");
    Serial.print(zig_rec_str+"|"+in_temp+','+in_humi+','+in_ligh+"|");//数据转发给wifi
    Serial.println("receive over.");
    zig_rec_str = "";
  }
  //else
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






