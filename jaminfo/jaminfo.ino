#include <Adafruit_PN532.h>

#include <Wire.h> //Needed for I2C to GNSS
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_PN532 nfc(1, -1, &Wire);

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

void setup()
{

  Serial.begin(115200);
  Wire.begin();
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    display.print("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);


  if (myGNSS.begin() == false)
  {
    display.print("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing.");
    while (1)
      ;
  }

  // Enable the jamming / interference monitor
  UBX_CFG_ITFM_data_t jammingConfig; // Create storage for the jamming configuration
  if (myGNSS.getJammingConfiguration(&jammingConfig)) // Read the jamming configuration
  {
    if (jammingConfig.config.bits.enable == 0) // Check if the monitor is already enabled
    {
      (jammingConfig.config.bits.enable = 1); // Enable the monitor
      if (myGNSS.setJammingConfiguration(&jammingConfig)) // Set the jamming configuration
        display.println("Jamming monitor on");
      else
        display.println("Jamming monitor failed!");
    }
  }
  display.display();

}


void readCardData(uint16_t timeout) {
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;
  String cardData;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, timeout);

  if (success) {
    //Nothing
  }
}


void loop()
{
  // Create storage to hold the hardware status
  // See the definition of UBX_MON_HW_data_t in u-blox_structs.h for more details
  UBX_MON_HW_data_t hwStatus;

  if (myGNSS.getHWstatus(&hwStatus)) // Read the hardware status
  {
    display.setCursor(0, 0);
    display.clearDisplay();
    display.display();

    display.printf("Jamming State: %d\n", hwStatus.flags.bits.jammingState);

    display.print("Noise level: ");
    display.println(hwStatus.noisePerMS);
    
    display.print("AGC monitor: ");
    display.println(hwStatus.agcCnt);
    
    display.print("CW jamming indicator: ");
    display.println(hwStatus.jamInd);

    readCardData(50);

    display.printf("Sats: %d", myGNSS.getSIV());

    display.println();
    display.display();
  }

  delay(500);
}
