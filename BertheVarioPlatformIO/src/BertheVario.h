////////////////////////////////////////////////////////////////////////////////
/// \file BertheVario.h
///
/// \brief Include global du projet
///
/// \date creation     : 03/03/2024
/// \date modification : 27/03/2025
///

//////////////////
// include systeme
#include <esp32-hal-i2c.h>
#include <esp32-hal-dac.h>
#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <FS.h>
#include <MS5611.h>
#include <MPU9250.h>
#include <ESPFMfGK.h>

#include <math.h>
#include <map>
#include <vector>
#include <algorithm>

//////////////////////////
// definitions materielles

//#define GNU_VARIO_GRIS        ///< decommenter pour ecran original 1.54
#define TYPE_SCREEN 154         ///< si ecran 154
//#define TYPE_SCREEN 290       ///< si ecran 290 a implementer
//#define VARIO_CAP_MAG_A_PLAT  2 ///< si vario cap magnetique a plat 1 , ou sur suspente droite 2 , ou sur suspente gauche 3
#define VARIO_CAP_MAG_A_PLAT  1
//#define XC_TRACK                ///< si besoin de memoire et pas de xc_track desactiver (40ko de gagner)

#ifdef XC_TRACK
 #include <NimBLEDevice.h>
#endif

////////////////////////////////////
// platform de la version precedente
// platform = https://github.com/tasmota/platform-espressif32/releases/download/2024.01.01/platform-espressif32.zip
// platform = https://github.com/tasmota/platform-espressif32/releases/download/2024.12.30/platform-espressif32.zip
// platform = https://github.com/tasmota/platform-espressif32/releases/download/2024.04.11/platform-espressif32.zip
// platform = espressif32@6.10.0

////////////////
// si mode debug
//#define _LG_DEBUG_
//#define GPS_DEBUG
//#define IGC_DEBUG
//#define HTTP_DEBUG
//#define MS5611_DEBUG
//#define SOUND_DEBUG
//#define TERMIC_DEBUG
//#define MPU9250_DEBUG
//#define SMALL_VZ_DEBUG
//#define TMA_DEBUG
//#define REBOOT_DEBUG
//#define DEBUG_RANDO_VOl
//#define DEBUG_512

//#define SIMU_VOL      ///< attention pour le progamme validation simulateur de vol

////////////////////////
// definition des pin //
////////////////////////
#define POWER_PIN           12
#define POWER_PIN_STATE     HIGH
#define SPEAKER_PIN         25
#define VOLTAGE_DIVISOR_PIN 35

//#define VARIO_TW_FREQ   400000UL
//#define VARIOMETER_POWER_ON_DELAY   2000

// Sound
//#define PIN_AUDIO_AMP_ENA     12                        //Enabled ampli class D

// GPS
#define pinGpsRXD  (33)
#define pinGpsTXD  (-1)

// MPU 9250 / MS5611
#define VARIO_SDA_PIN 27
#define VARIO_SCL_PIN 32

#define MS5611_ADDRESS          (0x77)

#define MPU_STATIC_ADDRESS      (0x68)

// boutons
#define BUTTON_A_PIN 38
#define BUTTON_B_PIN 37
#define BUTTON_C_PIN 39

// SDCARD
#define SDCARD_CS_PIN   13
#define SDCARD_MOSI_PIN 15
#define SDCARD_MISO_PIN 2
#define SDCARD_SCK_PIN  14

////////////////
// constantes //
////////////////

#define MilesParDegres         (60.)
#define UnMileEnMetres         (1852.)
#define T_PI                   (3.1415926535)

///////////////////
// compatibilite //
///////////////////

#define FABS(x)     fabsf(x)
#define SQRT(x)     sqrtf(x)
#define SIN(x)      sinf(x)
#define COS(x)      cosf(x)
#define ACOS(x)     acosf(x)
#define POW(x,y)    powf(x,y)
#define FMOD(x,y)   fmodf(x,y)
#define FLOOR(x)    floorf(x)
#define MODF(x,y)   modff(x,y)

////////////////////
// include locaux //
////////////////////
#include "GlobalVar/CMutex.h"
#include "Gps/CTrame.h"
#include "Gps/CIgc.h"
#include "Gps/CStabGps.h"
#include "Gps/CFinDeVol.h"
#include "Gps/CGps.h"
#include "Gps/CHgt2Agl.h"
#include "Screen/CBoutons.h"
#include "Screen/CGestEcrans.h"
#include "Screen/CScreen154.h"
#include "Screen/CScreen290.h"
#include "Screen/CLocTermic.h"
#include "VarioBeep/CSoundSvr.h"
#include "VarioBeep/CVarioBeep.h"
#include "SDCard/CSDCard.h"
#include "GlobalVar/CNbSatDelay.h"
#include "GlobalVar/CConfigFile.h"
#include "Gps/CPileVit.h"
#include "MS5611/CMS5611.h"
#include "MPU9250/CMpu9250.h"
//#include "WebBrowser/CWebBrowser.h"
/*extern "C" {
#include "Neural/SNNS_NET.h"
}
#include "Neural/CNeuralTermic.h"*/
//#include "Calculateur/CThermicMap.h"
#include "Calculateur/CSortArray.h"
#include "Calculateur/CTerrainsConnu.h"
#include "GlobalVar/CFileHistoVol.h"
#include "Calculateur/CFinSol.h"
#include "Geometrie/CVecteur2D.h"
#include "Geometrie/CDroite2D.h"
#include "Geometrie/CVecteur3D.h"
#include "Geometrie/CDroite3D.h"
#include "Geometrie/CMatrice3D.h"
#include "Geometrie/Segments.h"
#include "ZonesAeriennes/CDate.h"
#include "ZonesAeriennes/CZoneAer.h"
#include "ZonesAeriennes/CDistFront.h"
#include "ZonesAeriennes/CPolygone.h"
#include "ZonesAeriennes/CZonesAerAll.h"
#include "ZonesAeriennes/CZonesFchValid.h"
#include "Perfmon/esp32_perfmon.h"
#ifdef SIMU_VOL
 #include "Gps/CSimuVol.h"
#endif
#include "FileMgr/FileMgr.h"
#include "RandoVol/CFileGpx.h"
#include "RandoVol/CRandoVol.h"
#ifdef XC_TRACK
 #include "Bluetooth/CBleXct.h"
#endif

///////////////////////////
// taches plus affichage //
///////////////////////////

// son vario priorite tres haute non interruptible (bloque/bug les capteurs sans mutex)
#define VARIOBEEP_NUM_TASK   0
#define VARIOBEEP_STACK_SIZE 1000
#define VARIOBEEP_PRIORITY   19
#define VARIOBEEP_CORE       0

// calcul general basse priorite
#define CALCUL_NUM_TASK     1
#define CALCUL_STACK_SIZE   2000
#define CALCUL_PRIORITY     0
#define CALCUL_CORE         1

// simuvol basse priorite
#define SIMUVOL_NUM_TASK     2
#define SIMUVOL_STACK_SIZE   3000
#define SIMUVOL_PRIORITY     0
#define SIMUVOL_CORE         1

/////////////////////////////////////////////
// taches plus temps reel                  //
// FreeRtOs utilise le core 0 a 75% a vide //
/////////////////////////////////////////////

// pression calcul Vz, acquisition cap magnetique, priorite tres haute non interruptible
#define VZ_MAG_NUM_TASK   3
#define VZ_MAG_STACK_SIZE 2000
#define VZ_MAG_PRIORITY   20
#define VZ_MAG_CORE       1

// acquisition gps, priorite haute non interruptible
#define SERIAL_NUM_TASK         4
#define SERIAL_GPS_STACK_SIZE   3000
#define SERIAL_GPS_PRIORITY     19
#define SERIAL_GPS_CORE         1

// ecriture igc, priorite tres haute non interruptible
#define IGC_NUM_TASK    5
#define IGC_STACK_SIZE  4000
#define IGC_PRIORITY    19
#define IGC_CORE        1

// temps de vol / histo, priorite tres haute non interruptible
#define TEMPS_NUM_TASK   6
#define TEMPS_STACK_SIZE 4000
#define TEMPS_PRIORITY   19
#define TEMPS_CORE       1

// scan des boutons, basse priorite
#define SCAN_BUTON_NUM_TASK     7
#define SCAN_BUTTON_STACK_SIZE  1000
#define SCAN_BUTTON_PRIORITY    0
#define SCAN_BUTTON_CORE        1

// relance pour cause faux depart de vol, tache fugitive.
#define RELANCE_IGC_NUM_TASK   8
#define RELANCE_IGC_STACK_SIZE 3000
#define RELANCE_IGC_PRIORITY   0
#define RELANCE_IGC_CORE       1

// serveur de son, priorite moyenne
#define SOUNDSVR_NUM_TASK   9
#define SOUNDSVR_STACK_SIZE 1500
#define SOUNDSVR_PRIORITY   9
#define SOUNDSVR_CORE       1

// nombre total de taches
#define SIZE_TASK 10

#include "GlobalVar/CNumTaskArr.h"
#include "GlobalVar/CGlobalVar.h"

//////////////////////
// variable globale //
//////////////////////
extern CGlobalVar g_GlobalVar ;

extern char NumVer[] ;
