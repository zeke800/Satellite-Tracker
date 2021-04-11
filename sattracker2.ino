#include <brent.h>
#include <Sgp4.h>
#include <sgp4coord.h>
#include <sgp4ext.h>
#include <sgp4io.h>
#include <sgp4pred.h>
#include <sgp4unit.h>
#include <visible.h>

#include <WiFiNINA.h>
#include <Stepper.h>
#include "secrets.h"
#include <SPI.h>
#include "wififunc.h"

#if defined(ADAFRUIT_FEATHER_M4_EXPRESS) || \
  defined(ADAFRUIT_FEATHER_M0_EXPRESS) || \
  defined(ADAFRUIT_FEATHER_M0) || \
  defined(ARDUINO_AVR_FEATHER32U4) || \
  defined(ARDUINO_NRF52840_FEATHER) || \
  defined(ADAFRUIT_ITSYBITSY_M0) || \
  defined(ADAFRUIT_ITSYBITSY_M4_EXPRESS) || \
  defined(ARDUINO_AVR_ITSYBITSY32U4_3V) || \
  defined(ARDUINO_NRF52_ITSYBITSY)
  // Configure the pins used for the ESP32 connection
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS    13   // Chip select pin
  #define ESP32_RESETN  12   // Reset pin
  #define SPIWIFI_ACK   11   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif defined(ARDUINO_AVR_FEATHER328P)
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS     4   // Chip select pin
  #define ESP32_RESETN   3   // Reset pin
  #define SPIWIFI_ACK    2   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif defined(TEENSYDUINO)
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS     5   // Chip select pin
  #define ESP32_RESETN   6   // Reset pin
  #define SPIWIFI_ACK    9   // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif defined(ARDUINO_NRF52832_FEATHER )
  #define SPIWIFI       SPI  // The SPI port
  #define SPIWIFI_SS    16  // Chip select pin
  #define ESP32_RESETN  15  // Reset pin
  #define SPIWIFI_ACK    7  // a.k.a BUSY or READY pin
  #define ESP32_GPIO0   -1
#elif !defined(SPIWIFI_SS)  // if the wifi definition isnt in the board variant
  // Don't change the names of these #define's! they match the variant ones
  #define SPIWIFI       SPI
  #define SPIWIFI_SS    10   // Chip select pin
  #define SPIWIFI_ACK    7   // a.k.a BUSY or READY pin
  #define ESP32_RESETN   5   // Reset pin
  #define ESP32_GPIO0   -1   // Not connected
#endif
char TLE[500];                //Variable to store satellite TLEs.
int i; int k;
char TLE1[4][70]; char TLE2[4][70];
int SAT; 
#define SERVER "https://celestrak.com"
#define PATH   "/satcat/tle.php?CATNR=33591" //Example for NOAA 19. Replace with your own sat id. You can find it at n2yo.com.
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;
WiFiSSLClient client; //SECURE!
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }
  // check for the WiFi module:
  WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
  while (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    delay(1000);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware.");
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  do {
    status = WiFi.begin(ssid, pass);
    delay(100); // wait until connected
  } while (status != WL_CONNECTED);
  Serial.println("Connected to wifi");
  printWifiStatus();
  Serial.println("Getting TLE. Please wait. (Note: Restart arduino for new tle every day.");
  Serial.println("\nStarting connection to server...");
  if (client.connect(SERVER, 443)) { //Port 443 default for https.
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET " PATH " HTTP/1.1");
    client.println("Host: " SERVER);
    client.println("Connection: close");
    client.println();
  }
  int connectLoop = 0;
  while(client.connected()){
    int j = 0;
    while (client.available()) {
      char c = client.read(); 
      TLE[j] = c;   //store characters to string
      j++;
      connectLoop = 0;
    }
    k = 0;
    for (j=26; j<96; j++){  //TLE line 1 spans characters 26 - 96
      TLE1[SAT][k] = TLE[j];
      k++;
    }
    k = 0;
    for (j=97; j< 167; j++){  //TLE line 2 spans characters 97 - 167
      TLE2[SAT][k]= TLE[j];
      k++;
    }
    connectLoop++;
    delay(100);
    if(connectLoop > 100){
      client.stop();
      break;
    }
  }
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    while(1); //No point to continue.
    
  }
  
 
 
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
