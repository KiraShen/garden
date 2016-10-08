
制作一个简易智能花卉管理系统
方案：microduino（核心控制）＋zigbee组网（cc2530）＋WIFI（esp8266）上传数据＋onenet（移动云服务）

流程：
节点node：
1.等待协调器命令；
2.接受环境参数（温湿光）；
3.检测盆栽参数（土壤湿度）；
4.条件对比，触发执行器（浇水），关闭；
----<data>----
RECEIVE:
temp:29.00;humi:34.00;ligh:16.00
cmd:REQDATA
1,data:17.00
2,data:92.00
3,data:52.00
4,data:55.00
5,data:55.00

2--------pot water on...
2--------pot water off...
(如果满足条件出发)
---------

协调器：
1.自动组网，解析节点地址；
2.持续为节点发送命令（请求数据）；
3.收到节点数据；
4.上传给Wi-Fi；

格式（每一盆栽数据）
coor->node
发送命令格式：+ZBD=0014,{REQDATA}|TEMP,HUMI,LIGH|
coor<-node
接受数据格式：+ZBD=0014,{0.00,55.00,60.00,55.00,88.00}
coor->wifi
上传数据wifi：+ZBD=0014,{0.00,55.00,60.00,55.00,88.00}|TEMP,HUMI,LIGH|

-------
Wi-Fi：
1.接受协调器数据(串口通信)；baud:115200
2.解析数据，土壤湿度&环境参数；
3.上传数据到onenet，需要路由器ssid&key，需要在netnet注册账户

格式
coor->wifi:+ZBD=0014,{0.00,55.00,60.00,55.00,88.00}|TEMP,HUMI,LIGH|
解析数据：data0,data1,data2,data3,data4,temp,humi,ligh
逐一上传数据，每一个节点数据上传正常时间小于2秒
