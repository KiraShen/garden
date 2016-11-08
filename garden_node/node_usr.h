
//node use core+ & set zigbee UART is D2,D3
/*
1,正常
11-正常浇水
12-紧急浇水

2,警告
21-湿度数据超幅
22-土壤干燥不适合浇水

3,错误
31-接受数据错误


LED状态表示：
红色 异常
绿色 通信
黄色 干燥
蓝色 浇水
黑色 正常

浇水状态：
1.正常浇水－需要环境参数
2.紧急浇水－不需要环境参数
3.每次正常浇水后下一次浇水时间>=INTERVAL_TIME＊HUNGRY

coor->node
style1: 发送命令格式：+ZBD=0014,{REQDATA}|TEMP,HUMI,LIGH|
style2: 发送命令格式：+ZBD=0014,123,{REQDATA}|TEMP,HUMI,LIGH|

*/

#define WATER_INTERVAL_TIME 10000 //浇水时间
#define OFF_INTERVAL_TIME 300000 //浇水后多少时间不浇水
#define SET_INTERVAL_TIME 60000	//循环时间>180000

/**************引脚配置****************/
#define PUMP_PORT 12//水泵

#define POT_PORT1 A1//盆栽－土壤湿度
#define POT_PORT2 A2
#define POT_PORT3 A3
#define POT_PORT4 A6
#define POT_PORT5 A7

#define SOLENOID_PORT1 7//电磁阀
#define SOLENOID_PORT2 8
#define SOLENOID_PORT3 9
#define SOLENOID_PORT4 10
#define SOLENOID_PORT5 11

#define LED_PIN A0
#define LED_NUM 1

/**************盆栽干燥上限****************/
#define HUNGRY 5 

/**************串口配置****************/
#define BAUD0 9600
#define BAUD1 9600

/**************限幅滤波偏差****************/
#define DEVIATION 10

/**************接受数据样式****************/
#define ADD 0
//#define ADD 4
