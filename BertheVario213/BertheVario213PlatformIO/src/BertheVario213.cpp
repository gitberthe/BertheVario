////////////////////////////////////////////////////////////////////////////////
/// \file BerheVario.cpp
///
/// \brief Fichier principal du projet GNU-Vario de Berthe
///
/// \date creation     : 02/03/2024
/// \date modification : 09/07/2025 priorite des taches, suppression XC_TRACK, distance zone aerienne
/// \date modification : 18/10/2025
///

char NumVer[] = "20260219a" ;

// uncomment next line to use HSPI for EPD (and e.g VSPI for SD), e.g. with Waveshare ESP32 Driver Board
//#define USE_HSPI_FOR_EPD

#include "BertheVario213.h"

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
//VBle.init( "BerteVario" ) ;

// init port serie de console
Serial.begin(115200);
Serial.println("Setup") ;
#ifdef G_DEBUG
 Serial.println();
 Serial.println("fonction setup");
 delay(100);
#endif

// beeper init
g_GlobalVar.LanceTacheSound() ;
delay( 300 ) ;
CGlobalVar::BeepOk() ;

// init ecran
g_GlobalVar.InitScreen() ;

// pour la lecture de la batterie
g_GlobalVar.InitAlim() ;

// affichage boutons
g_GlobalVar.AfficheBoutons() ;

// init boutons
g_GlobalVar.InitButton() ;

// init sdcard
g_GlobalVar.InitSDCard() ;

// test pdd fichier hgt
g_GlobalVar.TestPddGroundZ() ;

// boutons
bool BoutonGaucheAppuye = false ;  // calibration
bool BoutonCentreAppuye = false ;  // mode wifi
bool BoutonDroitAppuye = false ;   // mode rando-vol
unsigned long time = millis() ;
while ( (millis() - time) < 2000 && !BoutonGaucheAppuye && !BoutonCentreAppuye && !BoutonDroitAppuye )
    {
    if ( !g_GlobalVar.TestBouton(BUTTON_C_PIN) )
        BoutonDroitAppuye = true ;
    if ( !g_GlobalVar.TestBouton(BUTTON_A_PIN) )
        BoutonGaucheAppuye = true ;
    if ( !g_GlobalVar.TestBouton(BUTTON_B_PIN) )
        BoutonCentreAppuye = true ;
    }
//g_GlobalVar.BootEffectue() ;

// affichage voltage
g_GlobalVar.AfficheVoltage() ;

// lecture fichier de configuration
g_GlobalVar.m_Config.LectureFichier() ;

// Init BUS I2C
g_GlobalVar.InitI2C() ;

////////////////////
// si mode http wifi
if ( BoutonCentreAppuye )
    {
    // mode ftp pour loop
    g_GlobalVar.m_ModeHttp = true ;

    // lecture fichier zones aeriennes
    g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;
    // creation fichier de validation zones
    g_GlobalVar.m_ZonesAerAll.Valid() ;
    // liberation memoire
    g_GlobalVar.m_ZonesAerAll.DeleteAll() ;

    // affichage
    g_GlobalVar.AfficheConnectWifi() ;

    // connection wifi
    WiFi.begin( g_GlobalVar.m_Config.m_Ssid, g_GlobalVar.m_Config.m_Passwd );
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
g_GlobalVar.m_MS5611Press.InitMs5611() ;

// capteur accelerometrique/cap magnetique
g_GlobalVar.m_QMC5883Mag.InitMagnetique() ;

// init port serie GPS
g_GlobalVar.InitGps() ;

////////////////////////////////////
// si calibration capteur magnetique
if ( BoutonGaucheAppuye )
    {
    g_GlobalVar.AfficheCalibreMag() ;
    g_GlobalVar.m_QMC5883Mag.CalibrationMagnetique() ;
    CGlobalVar::Reboot() ;
    }

// lancement tache de calcul de la Vz et acquisition cap magnetique
g_GlobalVar.m_MS5611Press.LancerTacheCalculVzCapMag() ;

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

// lecture fichier terrains
g_GlobalVar.m_TerrainArr.LireFichierTerrains(TERRAIN_FCH) ;

// lecture fichier zones aeriennes
g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;

// lancement tache gps
g_GlobalVar.LanceTacheGps(true) ;

// lancement tache beep
g_GlobalVar.LanceTacheVarioBeep() ;

#ifdef G_DEBUG
  Serial.println("setup done");
#endif

#ifdef TIME_GROUND_Z
 //while ( g_GlobalVar.m_Hgt2Agl.GetCacheAlti().GetSize() < MAX_ELEMENT_ALTI - 20 )
 //    g_GlobalVar.m_Hgt2Agl.GetCacheAlti().Add( new CCacheAlti ) ;
#endif

#ifdef GROUND_Z_DEBUG
for ( int ipa = 0 ; ipa < 3 ; ipa++ )
    {
    unsigned long time_gz = millis() ;
    Serial.print( "\ncache size : " ) ; Serial.println( g_GlobalVar.m_Hgt2Agl.GetCacheSize() ) ;

    // 512
    float SsdLat = 45.631652 ;
    float SsdLon =  3.116147 ;
    // pdd
    float PddLat = 45.772396 ;
    float PddLon = 2.964061  ;
    // vichy
    float ViLat = 46.1239268 ;
    float ViLon = 3.4203712  ;
    // aiguille du midi
    float MiLat = 42.843134 ;
    float MiLon = -0.438166 ;
    // mont blanc
    float MbLat = 45.832526 ;
    float MbLon = 6.864775 ;
    // pt
    float PtLat = 45.885989 ;
    float PtLon = 2.727933 ;
    // corent
    float CoLat = 45.660355 ;
    float CoLon = 3.179634 ;
    // zp sancy
    float ZpLat = 45.491927 ;
    float ZpLon = 2.740171 ;
    // dune
    float DuLat = 44.529188 ;
    float DuLon = -1.257285;

    int SsdAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( SsdLon , SsdLat ) ;
    int PddAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( PddLon , PddLat ) ;
    int ViAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( ViLon , ViLat ) ;
    int MiAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( MiLon , MiLat ) ;
    int MbAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( MbLon , MbLat ) ;
    int PtAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( PtLon , PtLat ) ;
    int CoAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( CoLon , CoLat ) ;
    int ZpAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( ZpLon , ZpLat ) ;
    int DuAlti = g_GlobalVar.m_Hgt2Agl.GetGroundZ( DuLon , DuLat ) ;

    Serial.print( "\n512(790) alti:" ) ; Serial.println( SsdAlti ) ;
    Serial.print( "pdd(1465) alti:" ) ; Serial.println( PddAlti ) ;
    Serial.print( "vichy() alti:" ) ;   Serial.println( ViAlti ) ;
    Serial.print( "midi(2847) alti:" ) ;Serial.println( MiAlti ) ;
    Serial.print( "mb(4768) alti:" ) ;  Serial.println( MbAlti ) ;
    Serial.print( "pt(686) alti:" ) ;   Serial.println( PtAlti ) ;
    Serial.print( "co(616) alti:" ) ;   Serial.println( CoAlti ) ;
    Serial.print( "Zp(1141) alti:" ) ;  Serial.println( ZpAlti ) ;
    Serial.print( "Du(2) alti:" ) ;  Serial.println( DuAlti ) ;
    unsigned long ecart = millis() - time_gz ;
    Serial.print( "time calc hgt ms : " ) ;
    Serial.println( ecart ) ;
    Serial.println( "************" ) ;
    }
#endif
}

bool once = true ;
bool once_ota = true ;

/* AsyncClient* client = new AsyncClient;
#define HOST "pdd.dprslt.fr"
#define PORT 80 */

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction loop toujours appelée.
void loop()
{
if ( g_GlobalVar.GetWant2Reboot() )
    CGlobalVar::Reboot() ;

/*
//client->write("GET /README.md HTTP/1.1\r\nHost: " HOST "\r\nUser-Agent: ESP\r\nConnection: close\r\n\r\n");
  client->onError([](void* arg, AsyncClient* client, int8_t error) {
    Serial.printf("** error occurred %s \n", client->errorToString(error));
    client->close(true);
    //delete client;
  });

  client->onDisconnect([](void* arg, AsyncClient* client) {
      Serial.printf("** client has been disconnected: %" PRIu16 "\n", client->localPort());
      //client->close(true);
      //delete client;

      //permits++;
      //makeRequest();
    });

    client->onData([](void* arg, AsyncClient* client, void* data, size_t len) {
      //Serial.printf("** data received by client: %" PRIu16 ": len=%u\n", client->localPort(), len);
      Serial.printf( (char*)data) ;
    });

  client->onConnect([](void* arg, AsyncClient* client) {
    //pekhoih-prog/AsyncTCP_SSL@^1.3.1
	armmbed/mbedtls@^2.23.0rmits--;
    Serial.printf("** client has been connected: %" PRIu16 "\n", client->localPort());

    //client->connect(HOST,client->localPort());

    //client->write("GET /README.md HTTP/1.1\r\nHost: " HOST "\r\nUser-Agent: ESP\r\nConnection: close\r\n\r\n");
    client->write("GET /space/notams HTTP/1.0\r\nHost: " HOST "\r\nUser-Agent: ESP\r\nConnection: open\n\r\n");
    }) ;

  if (once && client->connect(HOST, PORT))
    {
    once = false ;
    Serial.println( "connect ok" ) ;
    }
  //else
  //    client->write("GET /README.md HTTP/1.1\r\nHost: " HOST "\r\nUser-Agent: ESP\r\nConnection: close\r\n\r\n");

return ;
*/

/*g_GlobalVar.m_VitVertMS += 0.1 ;
if ( g_GlobalVar.m_VitVertMS >= 0.3 )
    g_GlobalVar.m_VitVertMS *= -1 ;*/

////////////////////
// si mode http wifi
if ( g_GlobalVar.m_ModeHttp )
    {
    // pour le test reboot
    g_GlobalVar.m_DureeVolMin = ATTENTE_MESSAGE_GPS ;
    // http client
    pfilemgr->handleClient();
    return ;
    }

// si raz ecran
if ( g_GlobalVar.BoutonCentreLong() )
    {
    g_GlobalVar.PurgeBoutons( 3000 ) ;
    g_GlobalVar.ScreenRaz() ;
    }

////////////////////
// si mode rando-vol
if ( g_GlobalVar.m_ModeRandoVol )
    {
    if ( once )
        {
        unsigned int time = millis() ;
        while ( (millis()-time) < 2000 )
            {
            // mode verification checksum
            if ( g_GlobalVar.BoutonDroit() )
                {
                g_GlobalVar.m_ModeChecksum = true ;
                g_GlobalVar.m_ModeRandoVol = false ;
                g_GlobalVar.DoChar( 30 , 30 , "Mode chks" ) ;
                once = false ;
                return ;
                }
            }
        once = false ;
        g_GlobalVar.InitRandoVol() ;
        }
    g_GlobalVar.AfficheRandoVol() ;
    delay( 200 ) ;
    return ;
    }

////////////////////
// si mode checksum
if ( g_GlobalVar.m_ModeChecksum )
    {
    // si cheksum ok
    if ( g_GlobalVar.VerifyAllChecksumGood() )
        {
        Serial.println( "good all cheksum" ) ;
        g_GlobalVar.DoChar( 0 , 120 , "chks all ok" ) ;
        }
    else
        g_GlobalVar.DoChar( 0 , 120 , "wait bad chk" ) ;
    // atente 1 jour
    if ( ! g_GlobalVar.GetWant2Reboot() )
        delay( 1000*60*60*24 ) ;
    return ;
    }

#ifndef NO_OTA
//////////////////////////////////
// si mode telechargement firmware
if ( g_GlobalVar.m_ModeHttpOta )
    {
    if ( once_ota )
        {
        once_ota = false ;
        WifiInitOta() ;
        return ;
        }

    WifiOtaHandle() ;

    return ;
    }
#endif

// bip demarrage premiere boucle
if ( once )
    {
    once = false ;
    CGlobalVar::BeepOk() ;
    delay( 200 );
    CGlobalVar::BeepOk() ;

    // simu vol
    #ifdef SIMU_VOL
     g_SimuVol.LancerTacheSimuVol() ;
    #endif
    }

// message de debugage
#ifdef G_DEBUG
 Serial.println("fonction loop");
#endif

// fonction d'affichage
//unsigned long TimeAvAff = millis() ;
g_GlobalVar.AfficheAll();

// attente 0.5s, l'affichage n'est pas prioritaire
//while ( (millis() - TimeAvAff) < 500 )
//    delay( 10 ) ;

/*
// pression hPascal, altitude meters ,vario cm/s, temperature C, battery voltage, *checksum
char Sentence[500] ;
sprintf( Sentence , "LK8EX1,999999,%f,99,99,999",g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ) ;
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
