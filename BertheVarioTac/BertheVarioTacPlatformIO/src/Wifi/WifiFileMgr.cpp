////////////////////////////////////////////////////////////////////////////////
/// \file WifiFileMgr.cpp
///
/// \brief
///
/// \date creation     : 14/10/2024
/// \date modification : 16/09/2025
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief init du Wifi du file manager
void WifiInitFileMgr()
{
// arret des autres taches
g_GlobalVar.StopAll() ;

// pour gain memoire xctrack
//esp_bt_controller_disable();
//esp_bt_controller_deinit();

// delay de 0.5s
delay( 500 ) ;

// raz des boutons
g_GlobalVar.m_Screen.RazButtons() ;

// creation fichier de validation zones
g_GlobalVar.m_ZonesAerAll.Valid() ;

// liberation memoire
g_GlobalVar.m_ZonesAerAll.DeleteAll() ;

// affichage
g_GlobalVar.m_Screen.ScreenRaz() ;
g_tft.setCursor( 10 , 50 ) ;
g_tft.print( "Connect to wifi" ) ;

// connection wifi
WiFi.begin( (const char*)g_GlobalVar.m_Config.m_Ssid, (const char*)g_GlobalVar.m_Config.m_Passwd );
while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    }

#ifdef HTTP_DEBUG
Serial.println("");
Serial.print("Connected to ");
Serial.println(WIFI_SSID);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
#endif

// adresse wifi
char buf[50];
sprintf(buf, "FileManager IP:\nhttp://%d.%d.%d.%d:8080", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
g_GlobalVar.m_Screen.ScreenRaz() ;
g_tft.setCursor( 0 , 50 ) ;
g_tft.print( buf ) ;
g_tft.setCursor( 50 , 100 ) ;
g_tft.print( "touch/reboot" ) ;

// creation init file manager
g_pfilemgr = new ESPFMfGK( 8080 ) ;

addFileSystems();
setupFilemanager();
}
