////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 08/07/2025
///

#include "BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief setup ESP32-2432S028
void setup()
{
//Serial.begin(115200);
Serial.begin(9600);
#ifdef NO_GPS_DEBUG
 Serial.begin(9600);
 Serial.println("Debugage on" ) ;
#endif
//goto fin ;

// lancement tache son
g_GlobalVar.LanceTacheSound() ;
delay(500) ;
g_GlobalVar.BeepOk() ;

// init pin d'entree de tension
g_GlobalVar.InitBattery() ;

// initialisation de l'ecran tactile
g_tft.InitScreen() ;

// affaichage vbat
AfficheEcranDebut() ;

// init sdcard
g_GlobalVar.InitSDCard() ;

// test pdd hgt
g_GlobalVar.TestPddGroundZ() ;

// Init BUS I2C
g_GlobalVar.InitI2C() ;

// lecture fichier terrains
g_GlobalVar.m_TerrainArr.LireFichierTerrains(TERRAIN_FCH) ;

// lecture fichier zones aeriennes
g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;

// lecture fichier de configuration
g_GlobalVar.m_Config.LectureFichier() ;
g_GlobalVar.m_Config.FreeVect() ;

// init capteur de pression
g_GlobalVar.m_pCapteurPression->InitCapteur() ;

// init capteur magnetique
g_GlobalVar.m_QMC5883Mag.InitMagnetique() ;

#ifndef NO_GPS_DEBUG
 // init port serie GPS
 g_GlobalVar.InitGps() ;

 // lancement tache gps
 g_GlobalVar.LanceTacheGps(true) ;
#else
 g_GlobalVar.m_DureeVolMin = ATTENTE_MESSAGE_GPS ;
#endif // NO_GPS_DEBUG

// lancement tache beep
g_GlobalVar.LanceTacheVarioCapBeep() ;

// raz de l'ecran
g_GlobalVar.m_Screen.ScreenRaz(false) ;

// desactivation du wifi pour autonomie batterie
esp_wifi_stop() ;
esp_wifi_deinit() ;

/*
// mode economie d'energie
// -DCONFIG_PM_ENABLE
esp_pm_config_t pm_config ;
pm_config.max_freq_mhz = 100; // 160 240
pm_config.min_freq_mhz = 40;
pm_config.light_sleep_enable = true ;
ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
*/
}


////////////////////////////////////////////////////////////////////////////////
/// \brief boucle sans fin
void loop()
{
// variables
static int count_10hz = 0 ;
static bool WifiSetupFileMgr = true ;
static bool WifiSetupOta = true ;

////////////////////////
// si mode wifi file mgr
if ( g_GlobalVar.m_ModeHttp )
    {
    // si init wifi
    if ( WifiSetupFileMgr )
        {
        // init file manager
        WifiSetupFileMgr = false ;
        WifiInitFileMgr() ;
        }

    g_pfilemgr->handleClient();

    // si ecran pressé on reboot
    if ( !(count_10hz++%100) )
        {
        // traitement de touch pad
        g_GlobalVar.m_Screen.HandleTouchScreen() ;

        // scan les boutons tactiles
        g_GlobalVar.m_Screen.HandleButtons() ;

        if ( g_GlobalVar.m_Screen.IsCenterPressed() )
            g_GlobalVar.Reboot() ;
        }

    return ;
    }

////////////////////
// si mode wifi ota
if ( g_GlobalVar.m_ModeHttpOta )
    {
    // si init wifi
    if ( WifiSetupOta )
        {
        WifiSetupOta = false ;
        WifiInitOta() ;
        }

    WifiOtaHandle() ;

    // si ecran pressé on reboot
    if ( !(count_10hz++%100) )
        {
        // traitement de touch pad
        g_GlobalVar.m_Screen.HandleTouchScreen() ;

        // scan les boutons tactiles
        g_GlobalVar.m_Screen.HandleButtons() ;

        if ( g_GlobalVar.m_Screen.IsCenterPressed() )
            g_GlobalVar.Reboot() ;
        }

    return ;
    }

/////////////////
// ecran de debut
if( (millis()-g_GlobalVar.m_temps_debut)/1000 < 5 )
    {
    g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
     AfficheEcranDebut() ;
    g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
    delay( 1000 ) ;
    return ;
    }

static bool once = true ;
if ( once )
    {
    once = false ;
    g_GlobalVar.BeepOk() ;
    delay( 200 ) ;
    g_GlobalVar.BeepOk() ;
    }

////////
// 10 hz
unsigned long time = millis() ;

// traitement de touch pad
//g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
 g_GlobalVar.m_Screen.HandleTouchScreen() ;
//g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;

// scan les boutons tactiles
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
 bool Action = g_GlobalVar.m_Screen.HandleButtons() ;
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;

// affichage des boutons
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
g_tft.startWrite();
 g_GlobalVar.m_Screen.AfficheButtons() ;
g_tft.endWrite();
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;

// compteur 10 hz
count_10hz++ ;

// 10 hz
while ( (millis() - time) < 100 )
    delay( DELAY_ATTENTE_IDLE ) ;

///////
// 2 hz
if ( (count_10hz%5) && !Action )
    return ;

Serial.println( "*** loop ***" ) ;

// si page Vz en cours
bool SpeedScreen = g_GlobalVar.m_Screen.GetEtatAuto() == CAutoPages::ECRAN_0_Vz ;

// affichage des pages
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
if ( SpeedScreen )
    g_tft.startWrite();
g_GlobalVar.m_Screen.SequencementPages() ;
if ( SpeedScreen )
    g_tft.endWrite();
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;

// pour eviter mutex accet carte sd/ecran
if ( g_GlobalVar.m_Screen.GetEtatAuto() == CAutoPages::ECRAN_9c_RandoVolInfo )
    g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = g_GlobalVar.m_Hgt2Agl.GetGroundZ( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat ) ;

#ifdef NO_GPS_DEBUG
 static int count_debug = 0 ;
 Serial.print( count_debug++) ;
 Serial.println(" mode debug") ;
#endif

// mode economie d'energie
//esp_light_sleep_start() ;

//tft.sleep() ;
//tft.powerSaveOn() ;
//g_tft.startWrite(); // bug avec carte SD si utilisation
//tft.beginTransaction();

// sequencement des pages
//g_GlobalVar.m_Screen.SequencementPages() ;

// raz de l'etat des boutons tactiles
//g_GlobalVar.m_Screen.RazButtons() ;

//g_tft.endWrite(); // bug avec carte SD si utilisation

//tft.clearClipRect()  ;
//tft.fillRect(0, 0,240,320,TFT_WHITE);
//tft.clearDisplay() ;
//tft.powerSaveOff() ;
//tft.wakeup() ;
//tft.endTransaction();

//uxTaskGetStackHighWaterMark(NULL) ;
}
