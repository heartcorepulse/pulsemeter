#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
#include <SoftwareSerial.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define WHITE     0x0000 
#define YELLOW    0x001F 
#define CYAN      0xF800 
#define MAGENTA   0x07E0 
#define RED       0x07FF 
#define GREEN     0xF81F 
#define BLUE      0xFFE0 
#define BLACK     0xFFFF 

#define blinkPin A11
#define fadePin 21
#define pulsePin A10

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//  VARIABLES
int     fadeRate = 0;                 // used to fade LED on with PWM on fadePin
int     oldvalue = 0;
String  oldlevel = "";

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

void setup() {
  pinMode(blinkPin, OUTPUT);        // pin that will blink to your heartbeat!
  pinMode(fadePin, OUTPUT);         // pin that will fade to your heartbeat!
  Serial.begin(115200);             // we agree to talk fast!
  Serial2.begin(115200);
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS

  tft.reset();

  tft.begin(0x9341); // SDFP5408
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  drawTextToDisp();
}

void loop() {
  sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data
  if (QS == true) {                      // Quantified Self flag is true when arduino finds a heartbeat
    fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
    sendDataToProcessing('B', BPM);  // send heart rate with a 'B' prefix
    sendDataToProcessing('Q', IBI);  // send time between beats with a 'Q' prefix
    QS = false;                      // reset the Quantified Self flag for next time
  }
  SoundToBeat();  
  Serial2.println(BPM);
}

void SoundToBeat() {
  fadeRate -= 15;                        
  fadeRate = constrain(fadeRate, 0, 255); 
  analogWrite(fadePin, fadeRate);         
}

void sendDataToProcessing(char symbol, int data ) {
  Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
  Serial.println(data);                // the data to send culminating in a carriage return
}

void drawTextPulse (int value)
{
  
  int x = tft.width() /5,
      y = tft.height() /2;
  tft.setCursor(x, y);
  tft.setTextColor(BLACK);
  tft.setTextSize(9);
  tft.println(oldvalue);

  tft.setCursor(x, y);
  if (value > 60 && value < 90)
  {
    tft.setTextColor(GREEN);
  }
  else if(value < 60 && value > 40)
  {
    tft.setTextColor(YELLOW);
  }
  else if(value > 90 && value < 200)
  {
    tft.setTextColor(RED);
  }
  tft.println(value);
  oldvalue = value; 
}

void drawTextToDisp()
{
  int x = tft.width() /10,
      y = tft.height() /20;
  tft.setCursor(x, y);
  tft.setTextColor(CYAN);
  tft.setTextSize(4);
  tft.println("HeartCore");

  x = 0;
  y = tft.height() /5;
  tft.setCursor(x, y);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Your pulse is");

  y = tft.height() * 6/7;
  tft.setCursor(x, y);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.println("BPM");

  drawHeart();
}

void drawHeart()
{
  int x2,xa,xb,xc,ya,yb,yc; //the heart is made with 2 circues and 1 triangle
  int x1 = tft.width() * 16/20;
  int y1 = tft.height() * 17/20;
  int r = 10; //radius of the circles
  
  x2=x1+2*r;
  
  xa = x1 - r;
  ya = y1 + 2;
  xb = x2 + r;
  yb = y1 + 2;
  xc = x1 + r;
  yc = tft.height() * 19/20;
  
  tft.fillCircle(x1, y1, r, MAGENTA);
  tft.fillCircle(x2, y1, r, MAGENTA);
  tft.fillTriangle(xa, ya, xb, yb, xc, yc, MAGENTA);
}
