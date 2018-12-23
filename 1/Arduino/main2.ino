#include <FastLED.h>
#define LED_PIN  7
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define BRIGHTNESS 20
#include <IRremote.h>
int tiaojian=0;
#define SSID        "HUAWEI P20" //改为你的Wi-Fi名称
#define PASSWORD    "swj369718"//Wi-Fi密码
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID    "503429306" //OneNet上的设备ID
#define PROJECTID   "184572" //OneNet上的产品ID
#define HOST_PORT   (80)
String apiKey="GklTbjI7xqYMU86NgW6rksC2wmI=";//与你的设备绑定的APIKey
#include <Wire.h>                                  //调用库  
#include <ESP8266.h>
//#include <I2Cdev.h>                                //调用库 
String mCottenData;
String jsonToSend;
#include <SoftwareSerial.h>
#define EspSerial mySerial
#define UARTSPEED  9600
SoftwareSerial Serial3; /* RX:D3, TX:D2 */
ESP8266 wifi(&EspSerial);
unsigned long net_time1 = millis();                          //数据上传服务器时间
String postString;                                //用于存储发送数据的字符串
//String jsonToSend;                                //用于存储发送的json格式参数

int RECV_PIN = 10;   //红外线接收器OUTPUT端接在pin 10
IRrecv irrecv(RECV_PIN);  //定义IRrecv对象来接收红外线信号
decode_results results;   //解码结果放在decode_results构造的对象results里

const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

int speedS = 8; //初始速度
int score = 0; //分数
int FX, FY; //食物坐标
int SX, SY; //蛇头坐标
long KEY, K; //按键
char s[7]; //蛇身

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;

long ms;
int dl = 400;
void RANDOM()
{
  FX = random(1, 14);
  FY = random(1, 14);
}

void Delay(int timeF)
{
    if(s[6]!=FX&&s[5]!=FY)
    {
      matrix_drawPixel(FX, FY, 0);
      matrix_drawPixel(FX, FY, 1);
      FastLED.show();
    }
    else {
      RANDOM();
      FastLED.show();
    }
    dl = 50 * timeF;
}
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;

  if ( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if ( kMatrixSerpentineLayout == true) {
    if ( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }

  return i;
}

void matrix_drawPixel(int x, int y, int c) {
  if (c == 2) {
    leds[XY(x, y)] = CRGB::Red;
  }
  if (c == 1) {
    leds[XY(x, y)] = CRGB::Green;
  }
  if (c == 0) {
    leds[XY(x, y)] = CRGB::Black;
  }
  if (c == 3) {
    leds[XY(x, y)] = CRGB::Blue;
  }
  if (c == 4) {
    leds[XY(x, y)] = CRGB::Yellow;
  }
}
void Wall() {
  for (int x = 0; x < 16; x++) {
    matrix_drawPixel(0, x, 4);
    matrix_drawPixel(15, x, 4);
    matrix_drawPixel(x, 0, 4);
    matrix_drawPixel(x, 15, 4);
  }


}
void Logo()
{
   FastLED.clear(); 
     for(int whiteLed = 0; whiteLed < 256; whiteLed = whiteLed + 1) {
      leds[whiteLed] = CRGB::White;
      FastLED.show();
      leds[whiteLed] = CRGB::Black;
   }
  KEY = 4;
  K=0;
  Wall();
    s[1] = random(1,8); s[2] = random(1,8); s[3] = s[1]+1; s[4] = s[2]; s[5] = s[1]+2; s[6] = s[2];
  for (int i = 1; i <= 6; i += 2)
  {
    if (i == 5) matrix_drawPixel(s[i + 1], s[i], 3); else
      matrix_drawPixel(s[i + 1], s[i], 2);
  }
  RANDOM();
  FastLED.show();
}
void gameover()
{
  delay(500);
  Logo();
  FastLED.clear();
  delay(500);
  tiaojian=1;
}


void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear();
  Logo();
  Serial.begin(9600);
  irrecv.enableIRIn(); // 启动红外解码
  ms = millis();

  //初始化串口波特率  
    Wire.begin();
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    Serial.print(F("setup begin\r\n"));
    delay(100);

  WifiInit(EspSerial, UARTSPEED);

  Serial.print(F("FW Version:"));
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print(F("to station + softap ok\r\n"));
  } else {
    Serial.print(F("to station + softap err\r\n"));
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print(F("Join AP success\r\n"));

    Serial.print(F("IP:"));
    Serial.println( wifi.getLocalIP().c_str());
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }

  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }

  Serial.print(F("setup end\r\n"));
  
}
void function();
void loop() {
    if (irrecv.decode(&results)) {      //解码成功，收到一组红外线信号
    //Serial.println(results.value);//// 输出红外线解码结果（十六进制）
    if(results.value != 0 && results.value != -1)
      K = results.value;
    Serial.println(K);
    Serial.println(score);
    if(tiaojian==1)
    {
      updateSensorData();
     }
    irrecv.resume(); //  接收下一个值
    }
    if(millis() - ms >= dl) {
      function();
      ms = millis();
    }
}
void function() {
  
   if(K==0){
    if(KEY==1)KEY=1;
    if(KEY==2)KEY=2;
    if(KEY==3)KEY=3;
    if(KEY==4)KEY=4;
  }
  if(K==33480735){
    if(KEY==1)KEY=3;//左转
    if(KEY==2)KEY=2;
    if(KEY==3)KEY=1;
    if(KEY==4)KEY=1;
    K=0;
  }
  if(K==33460335){
    if(KEY==1)KEY=1;//右转
    if(KEY==2)KEY=3;
    if(KEY==3)KEY=2;
    if(KEY==4)KEY=2;
    K=0;
  }
   if(K==33464415){
    if(KEY==1)KEY=4;//向上
    if(KEY==2)KEY=4;
    if(KEY==3)KEY=3;
    if(KEY==4)KEY=4;
    K=0;
  }
   if(K==33478695){
    if(KEY==1)KEY=3;//向下
    if(KEY==2)KEY=3;
    if(KEY==3)KEY=3;
    if(KEY==4)KEY=4;
    K=0;
  }
  FastLED.clear();
    Wall();
  if (KEY == 1) {
    s[1] = s[3]; s[2] = s[4]; s[3] = s[5]; s[4] = s[6]; s[6] = s[6] - 1;//右转
    K=0;
  }
  if (KEY == 2) {
    s[1] = s[3]; s[2] = s[4]; s[3] = s[5]; s[4] = s[6]; s[6] = s[6] + 1;//左转
    K=0;
  }
  if (KEY == 3) {
    s[1] = s[3]; s[2] = s[4]; s[3] = s[5]; s[4] = s[6]; s[5] = s[5] - 1;//后退
    K=0;
  }
  if (KEY == 4) {
    s[1] = s[3]; s[2] = s[4]; s[3] = s[5]; s[4] = s[6]; s[5] = s[5] + 1;//直行
    K=0;
  }
  for (int i = 1; i <= 6; i += 2)
  {
    matrix_drawPixel(s[i + 1], s[i], 2);
    if(i==5) matrix_drawPixel(s[i+1], s[i], 3); else  matrix_drawPixel(s[i+1], s[i], 2);
  }
  //matrix_drawPixel(s[6], s[5], 2);

  FastLED.show();
 // Delay(speedS);
    if(s[6]!=FX||s[5]!=FY)
    {
      matrix_drawPixel(FX, FY, 1);
      FastLED.show();
    }
  SX = s[6]; SY = s[5];
  
  if (SY == FY && SX == FX) {
    score += 1;
    RANDOM();
    FastLED.show();
  }
  speedS = speedS + score*500;
  if (SY<=0 or SY>=15 or SX<=0 or SX>=15) {
    delay(1000);
    gameover();
  }
}

void updateSensorData() {
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");

    jsonToSend="{\"score\":";
    //dtostrf(score,1,2,buf);
    jsonToSend+="\"";
    jsonToSend += score;
    jsonToSend += "\"";
    jsonToSend+="}";



    postString="POST /devices/";
    postString+=DEVICEID;
    postString+="/datapoints?type=3 HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="Content-Length:";
    postString+=jsonToSend.length();
    postString+="\r\n";
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";

  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("create tcp err\r\n");
  }
}
