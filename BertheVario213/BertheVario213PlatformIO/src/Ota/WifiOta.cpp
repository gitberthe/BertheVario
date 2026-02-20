////////////////////////////////////////////////////////////////////////////////
/// \file WifiOta.cpp
///
/// \brief
///
/// \date creation     : 14/10/2024
/// \date 23/10/2025 : mise a jour affichage moins long
///

#include "../BertheVario213.h"

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
  g_GlobalVar.DoChar( 0 , 150 , "OTA start!" );
  // <Add your own code here>
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAProgress(size_t current, size_t final)
{
int static count = 0 ;
  // Log every 1 second
  /*if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }*/
if ( !(count%500) )
    {
    static int x_char = 0 ;
    g_GlobalVar.DoChar( x_char , 40 , "." );
    x_char += 10 ;
    }
count++ ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    g_GlobalVar.DoChar( 0 , 200 , "OTA successfull!" ) ;
  } else {
    g_GlobalVar.DoChar( 0 , 200 , "There was an error during OTA update!"  );
  }
  // <Add your own code here>
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation du mote telechargement firmware.
void WifiInitOta()
{
// arret des autres taches
g_GlobalVar.StopAll() ;

//delay( 500 ) ;

// raz des boutons
//g_GlobalVar.RazButtons() ;

// creation fichier de validation zones
//g_GlobalVar.m_ZonesAerAll.Valid() ;

// liberation memoire
g_GlobalVar.m_ZonesAerAll.DeleteAll() ;

// affichage
g_GlobalVar.ScreenRaz() ;
//g_tft.setCursor( 0 , 50 ) ;
//g_tft.println( "Ota\nTouch/reboot" ) ;
//g_tft.println( "Connect to wifi\n" ) ;

g_GlobalVar.DoChar( 0 , 50 , "Ota init" ) ;


WiFi.mode(WIFI_STA);
WiFi.begin( g_GlobalVar.m_Config.m_Ssid , g_GlobalVar.m_Config.m_Passwd );

// Wait for connection
while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    }

// adresse wifi
char buf[50];
sprintf(buf, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
g_GlobalVar.AfficheWifi( buf ) ;

g_GlobalVar.m_Config.FreeVect() ;

g_pserver_ota = new WebServer(80) ;

g_pserver_ota->on("/", []() {
    g_pserver_ota->send(200, "text/plain", "ElegantOTA BertVario213.");});

ElegantOTA.begin(g_pserver_ota);    // Start ElegantOTA

// ElegantOTA callbacks
ElegantOTA.onStart(onOTAStart);
ElegantOTA.onProgress(onOTAProgress);
ElegantOTA.onEnd(onOTAEnd);

g_pserver_ota->begin();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dans platformio.ini mettre :
/// extra_scripts = platformio_upload.py
/// upload_protocol = custom
/// upload_url = http://192.168.148.237
/// ou
/// ~/.platformio/penv/bin/pio run -t upload --upload-port 10.62.22.186
/// ~/.platformio/penv/bin/pio run -t upload --upload-port /dev/ttyACM0
/// http://10.62.22.186/update pour interface web
void WifiOtaHandle()
{
if ( g_pserver_ota != NULL )
    {
    g_pserver_ota->handleClient();
    ElegantOTA.loop();
    }
}

#endif
