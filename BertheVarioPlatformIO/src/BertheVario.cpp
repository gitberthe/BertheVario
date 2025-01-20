////////////////////////////////////////////////////////////////////////////////
/// \file BerheVario.cpp
///
/// \brief Fichier principal du projet GNU-Vario de Berthe
///
/// \date creation     : 02/03/2024
/// \date modification : 20/01/2025
///

char NumVer[] = "20250120b" ;

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
//g_GlobalVar.InitSpeaker() ;
g_GlobalVar.LanceTacheSound() ;
delay(300) ;
CGlobalVar::BeepOk() ;

// init alim
g_GlobalVar.InitAlim() ;

// init ecran
g_GlobalVar.InitScreen() ;

// affichage boutons
g_GlobalVar.AfficheBoutons() ;

// init boutons
g_GlobalVar.InitButton() ;

// init sdcard
g_GlobalVar.InitSDCard() ;

// boutons
bool BoutonGaucheAppuye = false ;  // calibration
bool BoutonCentreAppuye = false ;  // mode wifi
bool BoutonDroitAppuye = false ;   // mode rando-vol
unsigned long time = millis() ;
while ( (millis() - time) < 400 )
    {
    if ( g_GlobalVar.BoutonDroit() )
        BoutonDroitAppuye = true ;
    if ( g_GlobalVar.BoutonGauche() )
        BoutonGaucheAppuye = true ;
    if ( g_GlobalVar.BoutonCentre() )
        BoutonCentreAppuye = true ;
    }
g_GlobalVar.BootEffectue() ;

// affichage voltage
g_GlobalVar.AfficheVoltage() ;

// si un bouton appuye
if ( BoutonCentreAppuye || BoutonDroitAppuye || BoutonGaucheAppuye )
    {
    // lecture fichier de configuration
    g_GlobalVar.m_Config.LectureFichier() ;
    }
else
    {
    // lecture fichier de configuration
    g_GlobalVar.m_Config.LectureFichier() ;

    // lecture fichier terrains
    g_GlobalVar.m_TerrainArr.LireFichierTerrains(TERRAIN_FCH) ;

    // lecture fichier zones aeriennes
    g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;
    }

// Init BUS I2C
g_GlobalVar.InitI2C() ;

////////////////////
// si mode http wifi
if ( BoutonCentreAppuye )
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

////////////////////////////////////
// si calibration capteur magnetique
if ( BoutonGaucheAppuye )
    {
    g_GlobalVar.AfficheCalibreMag() ;
    g_GlobalVar.m_Mpu9250.Calibration() ;
    CGlobalVar::Reboot() ;
    }

// lancement tache de calcul de la Vz et acquisition cap magnetique
g_GlobalVar.m_MS5611.LancerTacheCalculVzCapMag() ;


/////////////////////
// si mode vol-rando
#ifdef DEBUG_RANDO_VOl
 g_GlobalVar.m_ModeRandoVol = true ;
 return ;
#endif
if ( BoutonDroitAppuye )
    {
    g_GlobalVar.m_Config.LectureFichier() ;
    g_GlobalVar.m_ModeRandoVol = true ;
    return ;
    }
else
    g_GlobalVar.m_ModeRandoVol = false ;

// lancement tache gps
g_GlobalVar.LanceTacheGps(true) ;

// lancement tache beep
g_GlobalVar.LanceTacheVarioBeep() ;

#ifdef _LG_DEBUG_
  Serial.println("setup done");
#endif
}

bool once = true ;

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction loop toujours appelée.
void loop()
{
// si boucle a ne plus faire
if ( g_GlobalVar.m_StopLoop )
    {
    delay( 100 ) ;
    return ;
    }

////////////////////
// si mode http wifi
if ( g_GlobalVar.m_ModeHttp )
    {
    pfilemgr->handleClient();
    return ;
    }

////////////////////
// si mode rando-vol
if ( g_GlobalVar.m_ModeRandoVol )
    {
    if ( once )
        {
        once = false ;
        g_GlobalVar.InitRandoVol() ;
        }
    g_GlobalVar.AfficheRandoVol() ;
    delay( 300 ) ;
    return ;
    }

// bip demarrage premiere boucle
if ( once )
    {
    once = false ;
    CGlobalVar::BeepOk() ;
    CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 );
    CGlobalVar::BeepOk() ;

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
unsigned long TimeAvAff = millis() ;
g_GlobalVar.AfficheAll();

// attente 0.5s, l'affichage n'est pas prioritaire
while ( (millis() - TimeAvAff) < 500 )
    delay( 10 ) ;

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
