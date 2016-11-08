/*
 a new dev :node. address:0014
1
+ZBD=0014,{-1,-1,-1,-1,-1}|25.00,45.00,43.00|
+ZBD=0014,2:{12}

temp:0.0-99.0
humi:0.0-99.0
light:0.0-99.0
 */
#define GET_INTERVAL_TIME  20000
#define SEND_INTERVAL_TIME 30000 
#define max_rout_num 2
#define max_node_num 5

//温度湿度IIC

//光照
#define LIGHT_PORT A0

/**************串口配置****************/
#define BAUD0 115200
#define BAUD1 9600

/**************接受数据样式****************/
#define ADD 0
//#define ADD 4
