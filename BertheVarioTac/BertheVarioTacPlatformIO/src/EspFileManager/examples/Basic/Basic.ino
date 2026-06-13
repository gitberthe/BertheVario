#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>


#include <EspFileManager.h>

#define MICROSD_SPI_SS_PIN 5
#define ST_SSID "XXXXXXXXXXX"
#define ST_PASS "XXXXXXXXXXX"

AsyncWebServer server(80);
EspFileManager FileManager;

void setup() 
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
	WiFi.begin(ST_SSID, ST_PASS);
	while (WiFi.status() != WL_CONNECTED);
	{
		Serial.print(".");
		delay(50);
	}
  Serial.print("Connected to wifi... \nIP: ");
  Serial.println(WiFi.localIP());

  FileManager.initSDCard(&SD, MICROSD_SPI_SS_PIN);
  FileManager.setServer(&server);

  server.begin();
}

void loop() 
{
  
}