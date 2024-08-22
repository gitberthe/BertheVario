////////////////////////////////////////////////////////////////////////////////
/// \file BerheVario.cpp
///
/// \brief Fichier principal du projet GNU-Vario de Berthe
///
/// \date creation     : 02/03/2024
/// \date modification : 16/08/2024
///

char NumVer[] = "20240822b" ;

// uncomment next line to use HSPI for EPD (and e.g VSPI for SD), e.g. with Waveshare ESP32 Driver Board
//#define USE_HSPI_FOR_EPD

#include "BertheVario.h"

// variable globale du programme
CGlobalVar g_GlobalVar ;

// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction setup de demmarrage.
void setup()
{

// init port serie de console
Serial.begin(115200);
Serial.println("Setup") ;
#ifdef _LG_DEBUG_
 Serial.println();
 Serial.println("fonction setup");
 delay(100);
#endif

//#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
  //hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
  //display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
//#endif

// beeper init
g_GlobalVar.InitSpeaker() ;
CGlobalVar::BeepOk() ;

// init alim
g_GlobalVar.InitAlim() ;

// init ecran
g_GlobalVar.InitScreen() ;
g_GlobalVar.AfficheVoltage() ;

// init boutons
g_GlobalVar.InitButton() ;

// attente
//delay(1000);

// init sdcard
g_GlobalVar.InitSDCard() ;

// lecture histo vol
g_GlobalVar.m_HistoVol.LectureFichiers() ;

// lecture fichier de configuration
g_GlobalVar.m_Config.LectureFichier() ;

// lecture fichier terrains
g_GlobalVar.m_TerrainArr.LireFichierTerrains() ;

// lecture fichier zones aeriennes
g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;

// Init BUS I2C
g_GlobalVar.InitI2C() ;

////////////////////
// si mode http wifi
if ( g_GlobalVar.BoutonCentre() )
    {
    // mode ftp pour loop
    g_GlobalVar.m_ModeHttp = true ;

    // creation fichier de validation zones
    g_GlobalVar.m_ZonesAerAll.Valid() ;
    // liberation memoire
    g_GlobalVar.m_ZonesAerAll.DeleteAll() ;

    // affichage
    g_GlobalVar.AfficheConnectWifi() ;

    // connection wifi
    WiFi.begin( g_GlobalVar.m_Config.m_Ssid.c_str(), g_GlobalVar.m_Config.m_Passwd.c_str() );
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
    sprintf(buf, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
    g_GlobalVar.AfficheWifi( buf ) ;

    // creation init file manager
    pfilemgr = new ESPFMfGK( 8080 ) ;

    addFileSystems();
    setupFilemanager();

    return ;
    }
else
    g_GlobalVar.m_ModeHttp = false ;


// capteur de pression temperature
g_GlobalVar.m_MS5611.InitMs5611() ;

// capteur accelerometrique/cap magnetique
g_GlobalVar.m_Mpu9250.Init() ;

// init port serie GPS
g_GlobalVar.InitGps() ;

/////////////////
// si calibration capteur magnetique
if ( g_GlobalVar.BoutonGauche() )
    {
    g_GlobalVar.AfficheCalibreMag() ;

    g_GlobalVar.m_Mpu9250.Calibration() ;

    CGlobalVar::Reboot() ;
    }

#ifndef MPU9250_DEBUG
// lancement tache de calcul du cap magnetique
g_GlobalVar.m_Mpu9250.LancerTacheCalculCapMag() ;

// lancement tache de calcul de la Vz
g_GlobalVar.m_MS5611.LancerTacheCalculVz() ;

// lancement tache gps
g_GlobalVar.LanceTacheGps(true) ;

// lancement tache beep
g_GlobalVar.LanceTacheVarioBeep() ;
#endif // MPU9250_DEBUG

#ifdef _LG_DEBUG_
  Serial.println("setup done");
#endif
}

bool once = true ;

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction loop toujours appelée.
void loop()
{
// temps de watchdog
g_GlobalVar.m_MillisWatchDog = millis() ;

/*
// test Hgt2Agl
// pdd
float PddLat = 45.772396 ;
float PddLon = 2.964061  ;
// vichy
float ViLat = 46.1239268 ;
float ViLon = 3.4203712  ;

// aiguille du midi
float MiLat = 45. + 52./60 + 43./3600. ;
float MiLon = 6. + 53./60 + 14./3600. ;

// aiguille du midi
float MbLat = 45.878723 ;
float MbLon = 6.887614 ;

// aiguille du midi
float PtLat = 45.885989 ;
float PtLon = 2.727933 ;

CHgt2Agl Hgt2Agl ;
int PddAlti = Hgt2Agl.GetGroundZ( PddLon , PddLat ) ;
int ViAlti = Hgt2Agl.GetGroundZ( ViLon , ViLat ) ;
int MiAlti = Hgt2Agl.GetGroundZ( MiLon , MiLat ) ;
int MbAlti = Hgt2Agl.GetGroundZ( MbLon , MbLat ) ;
int PtAlti = Hgt2Agl.GetGroundZ( PtLon , PtLat ) ;

Serial.println("--------------") ;
Serial.print("pdd(1465) alti:") ; Serial.println( PddAlti ) ;
Serial.print("vichy(255) alti:");    Serial.println( ViAlti ) ;
Serial.print("midi(3842) alti:") ; Serial.println( MiAlti ) ;
Serial.print("mb(4809) alti:") ; Serial.println( MbAlti ) ;
Serial.print("pt(686) alti:") ;  Serial.println( PtAlti ) ;

return ; // */

//Serial.println( xPortGetCoreID() ) ;

//Serial.print("Altibaro:");
//Serial.println( g_GlobalVar.m_MS5611.GetAltiMetres() ) ;

//////////////
// si mode ftp
if ( g_GlobalVar.m_ModeHttp )
    {
    pfilemgr->handleClient();
    return ;
    }

// bip demarrage premiere boucle
if ( once )
    {
    once = false ;
    delay(200);
    CGlobalVar::BeepOk() ;
    delay(200);
    CGlobalVar::BeepOk() ;
    delay(200);

    // tache watch dog
    xTaskCreatePinnedToCore(CGlobalVar::TacheWatchDog, "WatchDogTask", WATCH_DOG_STACK_SIZE, & g_GlobalVar , WATCH_DOG_PRIORITY , NULL, WATCH_DOG_CORE);
    // tache acquisition termic
    //g_TermicMap.LancerTacheTermic() ;
    // screen tache de fond calcul
    g_GlobalVar.LancerTacheCalcul() ;
    // tache de mesure % utilisation cpu
    perfmon_start() ;

    // puy de dome pour test fichier hgt
    float Lat = 45.772396 ;
    float Lon = 2.964061  ;
    g_GlobalVar.m_Hgt2Agl.GetGroundZ( Lon , Lat ) ;

    // simu vol
    #ifdef SIMU_VOL
     g_SimuVol.LancerTacheSimuVol() ;
    #endif
    }

// message de debugage
#ifdef _LG_DEBUG_
 Serial.println("fonction loop");
#endif

// fonction d'affichage
g_GlobalVar.AfficheAll();

// attente 0.05s, l'affichage n'est pas prioritaire
delay(50) ;

/*// animation attente gps
if ( g_GlobalVar.m_AltiGps == 0 )
    g_GlobalVar.m_AltiGps = -1 ;
else if ( g_GlobalVar.m_AltiGps == -1 )
    g_GlobalVar.m_AltiGps = 0 ; */

/*// augmentation valeur vitesse verticale
static float AngleDeg = 0. ;
g_GlobalVar.m_VitVertMS = 9 * cos( AngleDeg / 180. * 3.1416 ) ;
AngleDeg += 40. ;

g_GlobalVar.m_dureeVolMin+= 5 ;
g_GlobalVar.m_CapGpsDeg  += 10 ;
g_GlobalVar.m_AltiGps    += 100 ;
g_GlobalVar.m_VitesseKmh += 0.5 ;*/

// beep
//g_GlobalVar.beeper( 5000 , 200 ) ;
}
