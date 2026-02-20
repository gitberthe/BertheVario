////////////////////////////////////////////////////////////////////////////////
/// \file BertheVarioTac.h
///
/// \brief Include global du projet de vario tactile
///
/// \date creation     : 20/09/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 09/07/2025 modification beep vario
/// \date modification : 21/10/2025
///

#define NO_OTA

//////////////////////
// includes globaux //
//////////////////////
#include <esp32-hal-i2c.h>
#include <esp32-hal-dac.h>
#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#ifndef NO_OTA
 #include <WiFiClient.h>
 //#include <WiFiClientSecure.h>
 //#include <WebServer.h>
 //#include <HTTPClient.h>
 //#include "esp_tls.h"
 //#include "esp_http_client.h"
 //#include "mbedtls/ssl.h"
 //#include "ssl_client.h"
 //#include "mqtt_client.h"
 //#include "esp_tls.h"
 //#include "esp_sntp.h"
#endif
#include <FS.h>
#include <ESPFMfGK.h>
 // #include <SFE_BMP180.h> // sparkfun/Sparkfun BMP180@^1.1.2
#include <MS5611.h>
#include <freertos/queue.h>
#include <soc/dac_channel.h>
#include <driver/dac.h>
//#include <driver/dac_continuous.h>
#include <DFRobot_QMC5883.h>
#include <esp_wifi.h>
//#include <esp_bt.h>
//#include <esp_pm.h>
//#include <lz4.h> // -DLZ4_MEMORY_USAGE=15
#include <Update.h>

#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>

////////////////////////////////////////////////////////////////////////////////
// platform precedente
// bad : platform = espressif32
// bad : platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
// bad : platform = https://github.com/platformio/platform-espressif32.git
// platform = https://github.com/tasmota/platform-espressif32/releases/download/2024.01.01/platform-espressif32.zip
// platform ok :
// platform = https://github.com/tasmota/platform-espressif32/releases/download/2024.12.30/platform-espressif32.zip
//
// board_build.partitions = no_ota.csv
//
// extra_scripts = platformio_upload.py
// upload_protocol = custom
// upload_url = http://192.168.148.237

// ~/.platformio/packages/framework-arduinoespressif32/tools/espota.py -i 10.174.96.237 -f '/home/internet/Documents/PlatformIO/Projects/BertheVarioTac/BertheVarioTacPlatformIO/p_pio/build/esp32dev/firmware.bin'

// dans /home/internet/Documents/PlatformIO/Projects/BertheVarioTac/BertheVarioTacPlatformIO/.pio/libdeps/esp32dev/ESP32 File Manager for Generation Klick ESPFMfGK/src/ESPFMfGK.h
// // #define fileManagerServerStaticsInternally
// // #define ZipDownloadAll

//////////////////////////
// define pour le debug //
//////////////////////////
//#define GPS_DEBUG
//#define PERFMON_DEBUG
//#define NO_GPS_DEBUG
//#define MAG_DEBUG
//#define BRIGHT_DEBUG
//#define DEBUG_RANDO_VOL
//#define STACK_DEBUG

/////////////////////////////
// define pour le materiel //
/////////////////////////////
//#define BMP180_PRESS
#define MS5611_PRESS

//////////////////////////
// includes du logiciel //
//////////////////////////
#include "LibGeneral/LibGeneral.h"
#include "GlobalVar/CMutex.h"
#include "Screen/CBrightness.h"
#include "Screen/CTouchButtons.h"
#include "Screen/CLGFX.h"
#include "Screen/CAutoPages.h"
#include "Screen/CText2Screen.h"
#include "Screen/CScreen.h"
#include "Gps/CPileVit.h"
#include "GlobalVar/CFileHistoVol.h"
#include "GlobalVar/CConfigFile.h"
#include "Calculateur/CTerrainsConnu.h"
#include "SDCard/CSDCard.h"
#include "Perfmon/esp32_perfmon.h"
#include "Geometrie/CVecteur2D.h"
#include "Geometrie/CDroite2D.h"
#include "Geometrie/CVecteur3D.h"
#include "Geometrie/CDroite3D.h"
#include "Geometrie/CMatrice3D.h"
#include "ZonesAeriennes/CDate.h"
#include "ZonesAeriennes/CRectIn.h"
#include "ZonesAeriennes/CZoneAer.h"
#include "Geometrie/Segments.h"
#include "ZonesAeriennes/CDistFront.h"
#include "ZonesAeriennes/CPolygone.h"
#include "ZonesAeriennes/CZonesAerAll.h"
#include "ZonesAeriennes/CZonesFchValid.h"
#include "Gps/CFinDeVol.h"
#include "Gps/CTrame.h"
#include "Gps/CIgc.h"
#include "Gps/CGps.h"
#include "Gps/CHgt2Agl.h"
 // #include "BMP180/CBMP180Pression.h"  // sparkfun/Sparkfun BMP180@^1.1.2
#include "MS5611/CMS5611Pression.h"
#include "FileMgr/FileMgr.h"
#include "VarioCapBeep/CVarioCapBeep.h"
#include "VarioCapBeep/CSoundSvr.h"
#include "QMC5883L/CQMC5883Mag.h"
#include "RandoVol/CFileGpx.h"
#include "RandoVol/CRandoVol.h"
#include "Bluetooth/CBleXct.h"

//#include "Wifi/NetworkClientSecure.h"
//#include "Wifi/CWifiNotam.h"

////////////////////////
// definition des pin //
////////////////////////

// GPS
//#define pinGpsRXD  (27)
//#define pinGpsRXD  (21)
//#define pinGpsRXD  (35)
#define pinGpsRXD  (1)
#define pinGpsTXD  (-1)

#define VoltageInPin  (35)
#define BrightnessPin (34)

// SDCARD
#define SDCARD_CS_PIN   5
#define SDCARD_MOSI_PIN 23
#define SDCARD_MISO_PIN 19
#define SDCARD_SCK_PIN  18

// SDA SCL
#define VARIO_SDA_PIN 27
//#define VARIO_SDA_PIN 21
//#define VARIO_SDA_PIN 35
#define VARIO_SCL_PIN 22

// haut parleur
#define SPEAKER_PIN 26

////////////////
// constantes //
////////////////

#define T_PI                   (3.1415926535)
#define UnMileEnMetres         (1852.)
#define MilesParDegres         (60.)

///////////
// macro //
///////////

#define FABS(x)     fabsf(x)
#define SQRT(x)     sqrtf(x)
#define SIN(x)      sinf(x)
#define COS(x)      cosf(x)
#define ACOS(x)     acosf(x)
#define POW(x,y)    powf(x,y)
#define FMOD(x,y)   fmodf(x,y)
#define FLOOR(x)    floorf(x)
#define MODF(x,y)   modff(x,y)

//#define MIN( a , b )   (((a)<(b)) ? (a) : (b))
//#define MAX( a , b )   (((a)>(b)) ? (a) : (b))

///////////////////////////
// taches plus affichage //
///////////////////////////

// tache touch, basse priorite core 0 necessaire ou plantage
/*#define TOUCH_NUM_TASK       0
#define TOUCH_STACK_SIZE     3000
#define TOUCH_PRIORITY       5
#define TOUCH_CORE           0*/

/////////////////////////////////////////////
// taches plus temps reel                  //
// FreeRtOs utilise le core 0 a 75% a vide //
/////////////////////////////////////////////

// scan capteur et calcul Vz priorite tres haute non interruptible
#define VARIOCAP_NUM_TASK   0
#define VARIOCAP_STACK_SIZE 2400
#define VARIOCAP_PRIORITY   20
#define VARIOCAP_CORE       1

// son vario fonction Vz priorite moyenne
#define VARIOBEEP_NUM_TASK   1
#define VARIOBEEP_STACK_SIZE 2100
#define VARIOBEEP_PRIORITY   20
#define VARIOBEEP_CORE       1

// serveur de son, priorite moyenne
#define SOUNDSVR_NUM_TASK   2
#define SOUNDSVR_STACK_SIZE 2500
#define SOUNDSVR_PRIORITY   19
#define SOUNDSVR_CORE       0

// acquisition gps, priorite haute non interruptible
#define SERIAL_GPS_NUM_TASK     3
#define SERIAL_GPS_STACK_SIZE   2700
#define SERIAL_GPS_PRIORITY     19
#define SERIAL_GPS_CORE         1

// ecriture igc, priorite moyenne
#define IGC_NUM_TASK        4
#define IGC_STACK_SIZE      3400
#define IGC_PRIORITY        20
#define IGC_CORE            1

// temps de vol / histo, basse priorite
#define ATTENTE_VOL_NUM_TASK   5
#define ATTENTE_VOL_STACK_SIZE 3200
#define ATTENTE_VOL_PRIORITY   20
#define ATTENTE_VOL_CORE       1

// relance pour cause faux depart de vol, tache fugitive.
#define RELANCE_IGC_NUM_TASK   6
#define RELANCE_IGC_STACK_SIZE 3600
#define RELANCE_IGC_PRIORITY   0
#define RELANCE_IGC_CORE       1

// tache de mesure d'occupation des cores 0 et 1
#define PERF_MON_NUM_TASK   7
#define PERF_MON_STACK_SIZE 1000
#define PERF_MON_PRIORITY   1
#define PERF_MON_CORE       1

// nombre total de taches
#define SIZE_TASK 8


//////////////////////////
// includes du logiciel //
//////////////////////////
#include "GlobalVar/CNumTaskArr.h"
#include "GlobalVar/CGlobalVar.h"


////////////////////////////
// fonctions et variables //
////////////////////////////
void AfficheEcranDebut() ;
void WifiInitFileMgr() ;
void WifiInitOta() ;
void WifiOtaHandle() ;

#ifndef NO_OTA
 extern WebServer * g_pserver_ota ;
#endif
extern char g_NumVersion[] ;
