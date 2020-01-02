//ベルトコンベア制御用のプログラムです。
//Adafruit 2.8インチ TFTタッチシールド v2(静電容量方式)専用です。
//https://www.switch-science.com/catalog/1864/
//抵抗膜方式との違いに注意してください。
//
//[ライブラリ情報]
//Adafruit_GFX
//https://github.com/adafruit/Adafruit-GFX-Library
//Adafruit_ILI9341
//https://github.com/adafruit/Adafruit_ILI9341
//Adafruit_FT6206
//https://github.com/adafruit/Adafruit_FT6206_Library
//MsTimer2
//http://www.arduino.cc/playground/uploads/Main/MsTimer2.zip

#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <MsTimer2.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <EEPROM.h>

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ts = Adafruit_FT6206();

#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

float interval = 0.00;
float Record = 0.00;
int flag = 0;
String menu = "";
int inPin = 6;
int val = 0;
int outPin = 4;
int revPin = 5;
int AWO = 8;
int MO = 7;
unsigned long time01 = millis();
float time02 = 0;

#define MAIN_X 80
#define MAIN_Y 10
#define MODE_AUTO_X 110
#define MODE_AUTO_Y 13
#define MODE_MANUAL_X 110
#define MODE_MANUAL_Y 13

#define AUTO_X 20
#define AUTO_Y 70
#define AUTO_W 120
#define AUTO_H 120

#define MANUAL_X 180
#define MANUAL_Y 70
#define MANUAL_W 120
#define MANUAL_H 120

#define TEXT01_X 10
#define TEXT01_Y 215
#define TEXT01_SIZE 1
#define TEXT01_COLOR ILI9341_BLACK

#define FRAME_X 10
#define FRAME_Y 70
#define FRAME_W tft.width()-FRAME_X*2
#define FRAME_H 70

#define START_X 230
#define START_Y FRAME_Y+9
#define START_W 70
#define START_H 50
#define START_COLOR_A ILI9341_LIGHTGREY
#define START_TEXTCOLOR_A ILI9341_WHITE
#define START_CURSOR_X 5
#define START_CURSOR_Y 17

#define DECREASE_X FRAME_X
#define DECREASE_Y FRAME_Y+FRAME_H+15
#define DECREASE_W 140
#define DECREASE_H 70
#define DECREASE_COLOR_A 421
#define DECREASE_COLOR_B 4910
#define DECREASE_TEXTCOLOR ILI9341_WHITE
#define DECREASE_TEXTSIZE 5
#define DECREASE_CURSOR_X 55
#define DECREASE_CURSOR_Y 16

#define INCREASE_X DECREASE_X*3+DECREASE_W
#define INCREASE_Y FRAME_Y+FRAME_H+15
#define INCREASE_W DECREASE_W
#define INCREASE_H DECREASE_H
#define INCREASE_COLOR_A 149
#define INCREASE_COLOR_B 4510
#define INCREASE_TEXTCOLOR ILI9341_WHITE
#define INCREASE_TEXTSIZE 5
#define INCREASE_CURSOR_X 55
#define INCREASE_CURSOR_Y 16

#define BACK_X 5
#define BACK_Y 5
#define BACK_W 90
#define BACK_H 40
#define BACK_CURSOR_X 10
#define BACK_CURSOR_Y 8
#define BACK_COLOR ILI9341_LIGHTGREY

#define STOP_X 20
#define STOP_Y 80
#define STOP_W 120
#define STOP_H 120
#define STOP_COLOR_A ILI9341_WHITE
#define STOP_COLOR_B ILI9341_GREEN
#define STOP_TEXTCOLOR ILI9341_WHITE
#define STOP_CURSOR_X 30
#define STOP_CURSOR_Y 50

#define MOVE_X STOP_X+STOP_W+20+20
#define MOVE_Y STOP_Y
#define MOVE_W STOP_W
#define MOVE_H STOP_H
#define MOVE_COLOR_A ILI9341_WHITE
#define MOVE_COLOR_B ILI9341_BLUE
#define MOVE_TEXTCOLOR ILI9341_WHITE
#define MOVE_CURSOR_X 30
#define MOVE_CURSOR_Y 50

#define CD_CURSOR_X 160
#define CD_CURSOR_Y 110
#define CD_COLOR ILI9341_RED

#define close_X 30
#define close_Y 30
#define close_W 40
#define close_H 40

#define TOUCHDELAY 120

void makeBtn(int x1, int y1, int w, int h, uint16_t rectcolor , int x2, int y2, uint16_t textcolor, int textsize, String pr)
{
  tft.fillRect(x1, y1, w, h, rectcolor);
  tft.setCursor(x1 + x2 , y1 + y2);
  tft.setTextColor(textcolor);
  tft.setTextSize(textsize);
  tft.println(pr);
}
void makeTextBox(int x, int y, uint16_t textcolor, int textsize, String pr)
{
  tft.setCursor(x , y);
  tft.setTextColor(textcolor);
  tft.setTextSize(textsize);
  tft.println(pr);
}

void intervalPrint(float i)
{
  EEPROM.put(0,i);
  makeBtn(FRAME_X + 121, FRAME_Y + 19, 90, 30, ILI9341_WHITE, 0, 0, ILI9341_BLACK, 3, (String)i);
}

void countdownCalc(float t)
{
  makeBtn(CD_CURSOR_X, CD_CURSOR_Y, 90, 30, ILI9341_WHITE, 0, 0, CD_COLOR, 3, (String)t);
}

void decreaseBtn()
{
  if ((interval - 0.10) < 0) {
  }
  else
  {
    MsTimer2::set(TOUCHDELAY, BtnOff);
    MsTimer2::start();
    if (flag != 1) {
      makeBtn(DECREASE_X, DECREASE_Y, DECREASE_W, DECREASE_H, DECREASE_COLOR_B, DECREASE_CURSOR_X, DECREASE_CURSOR_Y,
        DECREASE_TEXTCOLOR, DECREASE_TEXTSIZE, "-");
      flag = 1;
    }
    interval = interval - 0.10;
    intervalPrint(interval);
    //タイマーボタン操作時の反応速度を変更する場合はここ
    delay(90);
  }
}

void increaseBtn()
{
  if (20.00 > interval) {
    MsTimer2::set(TOUCHDELAY, BtnOff);
    MsTimer2::start();
    if ( flag != 2) {
      makeBtn(INCREASE_X, INCREASE_Y, INCREASE_W, INCREASE_H, INCREASE_COLOR_B, INCREASE_CURSOR_X, INCREASE_CURSOR_Y,
        INCREASE_TEXTCOLOR, INCREASE_TEXTSIZE, "+");
      flag = 2;
    }
    interval = interval + 0.10;
    intervalPrint(interval);
    //タイマーボタン操作時の反応速度を変更する場合はここ    
    delay(90);
  }
}

void startBtn()
{
  if (flag != 3) {
    flag = 3;
    Record = interval;
    makeBtn(START_X, START_Y, START_W, START_H, START_COLOR_A, START_CURSOR_X, START_CURSOR_Y, START_TEXTCOLOR_A, 2, "START");
    mode(menu = "CountDown");
  }
}

void moveBtn()
{
  if ( flag != 4) {
    flag = 4;
    digitalWrite(outPin, HIGH);
    makeBtn(MOVE_X, MOVE_Y, MOVE_W, MOVE_H, MOVE_COLOR_B, MOVE_CURSOR_X, MOVE_CURSOR_Y, MOVE_TEXTCOLOR, 3, "MOVE");
    ManualBtnOff("stopBtnOff");
  }
}

void stopBtn()
{
  if ( flag != 5) {
    flag = 5;
    digitalWrite(outPin, LOW);
    makeBtn(STOP_X, STOP_Y, STOP_W, STOP_H, STOP_COLOR_B, STOP_CURSOR_X, STOP_CURSOR_Y, STOP_TEXTCOLOR, 3, "STOP");
    ManualBtnOff("moveBtnOff");
  }
}


void BtnOff(void)
{
  if (flag == 1) {
    makeBtn(DECREASE_X, DECREASE_Y, DECREASE_W, DECREASE_H, DECREASE_COLOR_A, DECREASE_CURSOR_X, DECREASE_CURSOR_Y,
      ILI9341_WHITE, DECREASE_TEXTSIZE, "-");
    flag = 0;
  }
  else if (flag == 2) {
    makeBtn(INCREASE_X, INCREASE_Y, INCREASE_W, INCREASE_H, INCREASE_COLOR_A, INCREASE_CURSOR_X, INCREASE_CURSOR_Y,
      ILI9341_WHITE, INCREASE_TEXTSIZE, "+");
    flag = 0;
  }
}

void ManualBtnOff(String a)
{
  if (a == "stopBtnOff")
  {
    makeBtn(STOP_X, STOP_Y, STOP_W, STOP_H, STOP_COLOR_A, STOP_CURSOR_X, STOP_CURSOR_Y, ILI9341_DARKGREY, 3, "STOP");
    tft.drawRect(STOP_X, STOP_Y, STOP_W, STOP_H, ILI9341_BLACK);
  }
  else if (a == "moveBtnOff")
  {
    makeBtn(MOVE_X, MOVE_Y, MOVE_W, MOVE_H, MOVE_COLOR_A, MOVE_CURSOR_X, MOVE_CURSOR_Y, ILI9341_DARKGREY, 3, "MOVE");
    tft.drawRect(MOVE_X, MOVE_Y, MOVE_W, MOVE_H, ILI9341_BLACK);
  }
}

void mode(String a)
{
  if (a == "Main") {

    tft.fillScreen(ILI9341_WHITE);

    makeBtn(AUTO_X, AUTO_Y, AUTO_W, AUTO_H, ILI9341_DARKGREY, 27, 50, ILI9341_WHITE, 3, "AUTO");
    makeBtn(MANUAL_X, MANUAL_Y, MANUAL_W, MANUAL_H, ILI9341_DARKGREY, 10, 50, ILI9341_WHITE, 3, "MANUAL");

    makeTextBox(TEXT01_X, TEXT01_Y, TEXT01_COLOR, TEXT01_SIZE, "Pin7=INPUT,Pin13=OUTPUT");
    makeTextBox(TEXT01_X, TEXT01_Y + 10, TEXT01_COLOR, TEXT01_SIZE, "MATSUYAMA Inc. All right reserved.");
    makeTextBox(MAIN_X, MAIN_Y, ILI9341_BLACK, 3, "MAIN MENU");

    flag = 0;
  }
  else if (a == "Auto")
  {
    tft.fillScreen(ILI9341_WHITE);

    interval = Record;

    tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);
    tft.setCursor(FRAME_X + 6, FRAME_Y + 26);
    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(2);
    tft.println("INTERVAL:");

    makeTextBox(MODE_AUTO_X, MODE_AUTO_Y, ILI9341_BLACK, 3, "AUTO MODE");
    makeBtn(DECREASE_X, DECREASE_Y, DECREASE_W, DECREASE_H, DECREASE_COLOR_A, DECREASE_CURSOR_X, DECREASE_CURSOR_Y,
      DECREASE_TEXTCOLOR, DECREASE_TEXTSIZE, "-");
    makeBtn(INCREASE_X, INCREASE_Y, INCREASE_W, INCREASE_H, INCREASE_COLOR_A, INCREASE_CURSOR_X, INCREASE_CURSOR_Y,
      INCREASE_TEXTCOLOR, INCREASE_TEXTSIZE, "+");
    makeBtn(BACK_X, BACK_Y, BACK_W, BACK_H, BACK_COLOR, BACK_CURSOR_X, BACK_CURSOR_Y, ILI9341_WHITE, 3, "BACK");
    //makeBtn(START_X,START_Y,START_W,START_H,START_COLOR_A,START_CURSOR_X,START_CURSOR_Y,START_TEXTCOLOR_A,2,"START");
    EEPROM.get(0, interval);
    intervalPrint(interval);
  }
  else if (a == "Manual")
  {
    tft.fillScreen(ILI9341_WHITE);

    makeTextBox(MODE_MANUAL_X, MODE_MANUAL_Y, ILI9341_BLACK, 3, "MANUAL MODE");
    makeBtn(STOP_X, STOP_Y, STOP_W, STOP_H, STOP_COLOR_B, STOP_CURSOR_X, STOP_CURSOR_Y, STOP_TEXTCOLOR, 3, "STOP");
    makeBtn(MOVE_X, MOVE_Y, MOVE_W, MOVE_H, MOVE_COLOR_A, MOVE_CURSOR_X, MOVE_CURSOR_Y, ILI9341_DARKGREY, 3, "MOVE");
    makeBtn(BACK_X, BACK_Y, BACK_W, BACK_H, BACK_COLOR, BACK_CURSOR_X, BACK_CURSOR_Y, ILI9341_WHITE, 3, "BACK");
    tft.drawRect(MOVE_X, MOVE_Y, MOVE_W, MOVE_H, ILI9341_BLACK);
  }
  else if (a == "CountDown")
  {
    tft.fillRect(20, 20, 280, 200, ILI9341_WHITE);
    //makeBtn(close_X,close_Y,close_W,close_H,ILI9341_BLACK,15,11,ILI9341_WHITE,2,"x");
    makeTextBox(50, CD_CURSOR_Y, ILI9341_BLACK, 3, "Timer:");
    makeTextBox(CD_CURSOR_X, CD_CURSOR_Y, ILI9341_BLACK, 3, (String)Record);
    makeTextBox(50, CD_CURSOR_Y + 40, ILI9341_BLACK, 2, "Stand-by");
  }
}

void processing01()
{
  MsTimer2::set(1000, processing02);
  MsTimer2::start();
  makeBtn(50, CD_CURSOR_Y + 40, 220, 50, ILI9341_WHITE, 0, 0, CD_COLOR, 2, "Processing.");
}
void processing02()
{
  MsTimer2::set(1000, processing03);
  MsTimer2::start();
  makeBtn(50, CD_CURSOR_Y + 40, 220, 50, ILI9341_WHITE, 0, 0, CD_COLOR, 2, "Processing..");
}
void processing03()
{
  MsTimer2::set(1000, processing01);
  MsTimer2::start();
  makeBtn(50, CD_CURSOR_Y + 40, 220, 50, ILI9341_WHITE, 0, 0, CD_COLOR, 2, "Processing...");
}

void setup(void)
{
  Serial.begin(9600);
  tft.begin();
  if (!ts.begin(40)) {
    Serial.println("Unable to start touchscreen.");
  }
  else {
    Serial.println("Touchscreen started.");
  }
  // origin = left,top landscape (USB left upper)
  tft.setRotation(1);
  pinMode(inPin, INPUT);
  pinMode(outPin, OUTPUT);
  pinMode(revPin, OUTPUT);
  pinMode(AWO, OUTPUT);
  pinMode(MO, OUTPUT);

  mode(menu = "Main");
}

void loop()
{
  //digitalWrite(outPin, HIGH);
  //digitalWrite(revPin, HIGH);
  digitalWrite(AWO, LOW);
  digitalWrite(MO, LOW);

  if (menu == "Auto")
  {
    if (val = digitalRead(inPin))
    {
      MsTimer2::stop();
      time01 = millis();
      if (flag != 3) {
        flag = 3;
        Record = interval;
        mode(menu = "CountDown");
      }
      processing01();
      flag = 5;
    }
    else if (flag == 3)
    {
      // standby01();
      flag = 0;
    }
    if (flag == 5)
    {
      digitalWrite(outPin, HIGH);
      tft.fillRect(close_X, close_Y, close_W, close_H, ILI9341_WHITE);
      while (0.00 <= (time02 = (((Record * 1000) - (millis() - time01)))))
      {
        time02 = time02 / 1000;
        countdownCalc(time02);
        delay(20);
      }
      MsTimer2::stop();
      makeBtn(CD_CURSOR_X, CD_CURSOR_Y, 90, 30, ILI9341_WHITE, 0, 0, CD_COLOR, 3, "0.00");
      flag = 0;
      digitalWrite(outPin, LOW);
      mode(menu = "Auto");
    }
  }


  // See if there's any  touch data for us
  if (ts.touched())
  {
    // Retrieve a point
    TS_Point p = ts.getPoint();
    // rotate coordinate system
    // flip it around to match the screen.
    p.x = map(p.x, 0, tft.height(), tft.height(), 0);
    p.y = map(p.y, 0, tft.width(), tft.width(), 0);
    int y = tft.height() - p.x;
    int x = p.y;

    if (menu == "Main")
    {
      if (((x > AUTO_X) && (x < (AUTO_X + AUTO_W)))
        && ((y > AUTO_Y) && (y < AUTO_Y + AUTO_H)))
      {
        mode(menu = "Auto");
      }
      else if (((x > MANUAL_X) && (x < (MANUAL_X + MANUAL_W)))
        && ((y > MANUAL_Y) && (y < MANUAL_Y + MANUAL_H)))
      {
        mode(menu = "Manual");
      }
    }
    else if (menu == "Auto")
    {
      if (((x > DECREASE_X) && (x < (DECREASE_X + DECREASE_W)))
        && ((y > DECREASE_Y) && (y < DECREASE_Y + DECREASE_H)))
      {
        decreaseBtn();
      }
      else if (((x > INCREASE_X) && (x < (INCREASE_X + INCREASE_W)))
        && ((y > INCREASE_Y) && (y < INCREASE_Y + INCREASE_H)))
      {
        increaseBtn();
      }
      else if (((x > BACK_X) && (x < (BACK_X + BACK_W)))
        && ((y > BACK_Y) && (y < BACK_Y + BACK_H)))
      {
        mode(menu = "Main");
      }
    }
    else if (menu == "Manual")
    {
      if (((x > STOP_X) && (x < (STOP_X + STOP_W)))
        && ((y > STOP_Y) && (y < STOP_Y + STOP_H)))
      {
        stopBtn();
      }
      else if (((x > MOVE_X) && (x < (MOVE_X + MOVE_W)))
        && ((y > MOVE_Y) && (y < MOVE_Y + MOVE_H)))
      {
        moveBtn();
      }
      else if (((x > BACK_X) && (x < (BACK_X + BACK_W)))
        && ((y > BACK_Y) && (y < BACK_Y + BACK_H)))
      {
        mode(menu = "Main");
      }
    }
  }
}