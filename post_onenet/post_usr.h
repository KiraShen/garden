#define ADD 0

#define BAUD_RATE 115200
//#define INTERVAL_TIME 10000
//#define SERBER "api.heclouds.com"
//#define SERBER_PORT 80
 #define DEVICE_ID "3497676"
 #define API_KEY "3ebQkaP3cH4gZ3QCVt7WFg5p0R8="

char *ssid     = "Angelababy";//这里是我的wifi，你使用时修改为你要连接的wifi ssid
char *password = "00000000";//你要连接的wifi密码
char *host = "api.heclouds.com";//修改为你建立的Server服务端的IP地址
//char *host = 183.230.40.33
//port 80
unsigned int tcpPort = 80;//修改为你建立的Server服务端的端口号


 //#define SENSOR_ID "temp"
char *sensor_id[5]={"h1","h2","h3","h4","h5"};
char *enviro_id[3]={"T","H","L"};
char *status_id[3]={"S","S","S"};