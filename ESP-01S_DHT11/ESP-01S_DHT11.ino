/******************************************************************
1.本项目使用的开发版是ESP-01S，传感器是ESP-01S DHT11温度湿度模块
2.下载Blinker库和SimpleDHT库，通过arduino的项目→加载库，导入下载的库
    Blinker库下载地址：https://github.com/blinker-iot/blinker-library/archive/master.zip
    SimpleDHT下载地址：https://downloads.arduino.cc/libraries/github.com/winlinvip/SimpleDHT-1.0.15.zip
3.手机上下载Blinker软件，点击添加设备→独立设备→网络接入，这个案例是选择点灯科技，然后复制key放入下面的代码
 ******************************************************************/

#define BLINKER_WIFI
#include <Blinker.h>

#include <SimpleDHT.h>

char auth[] = "********";               //你手机上创建独立设备后的密钥
char ssid[] = "********";               //你的wifi名称
char pswd[] = "********";               //你的wifi密码
 
// 新建组件对象
BlinkerButton Button1("btn-001");     //手机软件的新增一个按键，数据键名改为btn-001，这样就将两者连接起来通过手机软件来操控固件
BlinkerText Text1("tex-data");        //新增一个文字模块，此项目用来一个文字模块来显示温度和湿度，将获取的数据在文本1和文本2显示


// 初始化dht11的角位
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

// 使用dht11读取温度湿度数据并将其显示在Blinker上
void read_data()
{
    // read without samples.
    byte temperature = 0;
    byte humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
        Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
        //return;  //若不注释return在串口测试时候不会打印下面信息，因为没有连接dht11，肯定会因为读取信息失败而退出函数
    }
    
    /*串口调试时显示如下数据温度湿度均为0，将esp01s插回温度湿度模块能正常工作即可正确的温度湿度值
    Read DHT11 failed, err=5136
    Sample OK: 0 *C, 0 H
    [168914] autoFormatData key: tex-data, json: "tex-data":{"tex":"温度: 0℃","tex1":"湿度: 0％"}
    [168914] new.
    */
    
    // 用于串口调试时显示数据
    Serial.print("Sample OK: ");
    Serial.print((int)temperature); Serial.print(" *C, ");
    Serial.print((int)humidity); Serial.println(" H");

    // 将读取的数据发送到Blinker客户端
    String T =  "温度: ";
    String H = "湿度: ";
    String T_unit =  "℃";
    String H_unit = "％";
    String showT = T + temperature + T_unit;  //直接写 "温度:" + temperature + "℃" 会报错
    String showH = H + humidity + H_unit;
    Text1.print(showT, showH); //Blinker的text组件使用方式，第一个值为文本1，第二个值为文本2
}

// 按下按键执行的函数，此项目用于手动刷新数据
void button1_callback(const String & state)
{
    BLINKER_LOG("Blinker readString: ", state);
    // 调用温度湿度函数读取并显示的函数
    read_data();
}

void setup()
{
    // 初始化串口
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();
    
    // 初始化有LED的IO
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    // 启用Button1组件，并给它绑定button1_callback函数
    Button1.attach(button1_callback);
}

void loop() {
    Blinker.run();
    delay(5000);
    read_data();//间隔5秒刷新一次数据
}
