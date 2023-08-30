#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <u-blox_config_keys.h>
#include <u-blox_structs.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2C_ADDRESS 0x42  // I2C address of ZOE-M8Q GPS module

SFE_UBLOX_GNSS gps;

#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET      -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS  0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
  display.println(F("SSD1306"));
  display.display();      // Show initial text
  delay(100);

  //
  // Initialize the GNSS unit
  //
  if (gps.begin() == false) //Connect to the u-blox module using Wire port
  {
    display.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    display.display();

    while (1);
  }

  printf("Factory reset starting...\n");
  gps.factoryReset();

  gps.setI2COutput(COM_TYPE_UBX); 
  gps.setNavigationFrequency(10); 
  gps.setAutoPVT(true); 
}

void loop() {

  if (gps.getPVT() && (gps.getInvalidLlh() == false))
  {
    display.setCursor(0, 0);
    display.clearDisplay();
    display.printf("Prot: %d\n", gps.getProtocolVersion());
    display.printf("Mod: %d.%d\n", gps.moduleSWVersion->versionHigh, gps.moduleSWVersion->versionLow);
    display.printf("Sats: %d\n", gps.getSIV());
    display.display();
  }

  delay(100);
}
