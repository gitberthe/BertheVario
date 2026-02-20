////////////////////////////////////////////////////////////////////////////////
/// \file WifiOta.cpp
///
/// \brief
///
/// \date creation     : 14/10/2024
/// \date modification : 16/09/2025
///

#include "BertheVarioTac.h"
#include <ArduinoOTA.h>
CGlobalVar g_GlobalVar ;

/*
#ifndef NO_OTA
#include <ElegantOTA.h>

WebServer * g_pserver_ota = NULL ; // (80);

//unsigned long ota_progress_millis = 0;

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAStart()
{
  // Log when OTA has started
  //Serial.println("OTA update started!");
  g_tft.println(" OTA start!");
  // <Add your own code here>
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAProgress(size_t current, size_t final)
{
int static count = 0 ;
  // Log every 1 second
if ( !(count%20) )
    g_tft.print(".");
count++ ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    g_tft.println("\nOTA successfull!");
  } else {
    g_tft.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}
#endif // NO_OTA
*/

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation du mote telechargement firmware.
void WifiInitOta()
{
ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      /*Serial.printf("Progress: %u%%\r", (progress / (total / 100)))*/;
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

// arret des autres taches
g_GlobalVar.StopAll() ;

delay( 500 ) ;

// raz des boutons
g_GlobalVar.m_Screen.RazButtons() ;

// creation fichier de validation zones
//g_GlobalVar.m_ZonesAerAll.Valid() ;

// liberation memoire
g_GlobalVar.m_ZonesAerAll.DeleteAll() ;

// affichage
g_GlobalVar.m_Screen.ScreenRaz() ;
g_tft.setCursor( 0 , 50 ) ;
g_tft.println( "Ota\nTouch/reboot" ) ;
g_tft.println( "Connect to wifi\n" ) ;

WiFi.mode(WIFI_STA);
WiFi.begin( (const char*)g_GlobalVar.m_Config.m_Ssid , (const char*)g_GlobalVar.m_Config.m_Passwd );

// Wait for connection
while (WiFi.status() != WL_CONNECTED)
    {
    delay(1000);
    Serial.print(".");
    }

g_tft.print( " " ) ;
g_tft.println( WiFi.localIP() ) ;

ArduinoOTA.begin();

/*
#ifndef NO_OTA
g_pserver_ota = new WebServer(80) ;

g_pserver_ota->on("/", []() {
    g_pserver_ota->send(200, "text/plain", "ElegantOTA BertVarioTac.");});

ElegantOTA.begin(g_pserver_ota);    // Start ElegantOTA

// ElegantOTA callbacks
ElegantOTA.onStart(onOTAStart);
ElegantOTA.onProgress(onOTAProgress);
ElegantOTA.onEnd(onOTAEnd);

g_pserver_ota->begin();
#endif
*/
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dans platformio.ini mettre :
/// extra_scripts = platformio_upload.py
/// upload_protocol = custom
/// upload_url = http://192.168.148.237
void WifiOtaHandle()
{
ArduinoOTA.handle();
/*
#ifndef NO_OTA
g_pserver_ota->handleClient();
ElegantOTA.loop();
#endif */
}
