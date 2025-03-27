////////////////////////////////////////////////////////////////////////////////
/// \file BerheVario.cpp
///
/// \brief Fichier principal du projet GNU-Vario de Berthe
///
/// \date creation     : 02/03/2024
/// \date modification : 27/03/2025
///

char NumVer[] = "20250327a" ;

// uncomment next line to use HSPI for EPD (and e.g VSPI for SD), e.g. with Waveshare ESP32 Driver Board
//#define USE_HSPI_FOR_EPD

#include "BertheVario.h"

// variable globale du programme
CGlobalVar g_GlobalVar ;

// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

//VarioBle VBle ;

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
g_GlobalVar.LanceTacheSound() ;
delay( 300 ) ;
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

    // lecture fichier zones aeriennes
    if ( BoutonCentreAppuye )
        g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;
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
    #ifdef XC_TRACK
     // desactive ble
     esp_bt_controller_disable();
     esp_bt_controller_deinit();
    #endif

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
    #ifdef XC_TRACK
     // desactive ble
     esp_bt_controller_disable();
     esp_bt_controller_deinit();
    #endif

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

// screen tache de fond calcul
g_GlobalVar.LancerTacheCalcul() ;
// tache de mesure % utilisation cpu
perfmon_start() ;

#ifdef XC_TRACK
// blue tooth
 g_GlobalVar.m_BleXct.Init( BLE_NAME ) ;
#endif

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
    delay( 200 ) ;
    return ;
    }

// bip demarrage premiere boucle
if ( once )
    {
    once = false ;
    CGlobalVar::BeepOk() ;
    CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 );
    CGlobalVar::BeepOk() ;

    // puy de dome pour test fichier hgt
    float Lat = 45.772396 ;
    float Lon = 2.964061  ;
    //Lon = 0 ;  Lat = 39 ; // pour test
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

/*
// pression hPascal, altitude meters ,vario cm/s, temperature C, battery voltage, *checksum
char Sentence[500] ;
sprintf( Sentence , "LK8EX1,999999,%f,99,99,999",g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;
int Len = strlen( Sentence ) ;
unsigned int checksum , ai, bi;                                               // Calculating checksum for data string
for (checksum = 0, ai = 0; ai < Len ; ai++)
    {
    bi = (unsigned char)Sentence[ai];
    checksum ^= bi;
    }

char Sentence2[500] ; // = "$LK8EX1,98974,197,0,99,999*32\r\n" ;
sprintf( Sentence2 , "$%s*%X\r\n" , Sentence , checksum ) ;
int Len2 = strlen( Sentence2 ) ;
for ( int ic = 0 ; ic < Len2 ; ic++ )
    VBle.write( Sentence2[ic] ) ;
VBle.flush() ;
*/
}
