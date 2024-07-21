////////////////////////////////////////////////////////////////////////////////
/// \file CScreen154.cpp
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 21/07/2024
///

#include "../BertheVario.h"

#if ( TYPE_SCREEN == 154 )

// comment out unused bitmaps to reduce code space used
//#include <bitmaps/Bitmaps200x200.h> // 1.54" b/w

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 1

#include <GxEPD2_BW.h>

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>
#include <GxEPD2_GFX.h>
// Note: if you use this with ENABLE_GxEPD2_GFX 1:
//       uncomment it in GxEPD2_GFX.h too, or add #include <GFX.h> before any #include <GxEPD2_GFX.h>

#include <Fonts/FreeMonoBold9pt7b.h> // 12 18
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

// note 16.11.2019: the compiler may exclude code based on constant if statements (display.epd2.panel == constant),
//                  therefore bitmaps may get optimized out by the linker

#ifdef GNU_VARIO_GRIS
 // select the display constructor line in one of the following files (old style):
 #include "Screen/GxEPD2_display_selection.h"
#else
 //#include "Screen/GxEPD2_display_selection_added.h"
 // or select the display class and display driver class in the following file (new style):
 #include "Screen/GxEPD2_display_selection_new_style.h"
#endif

//#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
//SPIClass hspi(HSPI);
//#endif

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialise l'ecran.
void CScreen154::InitScreen()
{
// init ecran
display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02

//hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
//display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));

//display.init(115200, true, 10, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
//display.init(115200, true, 10, false, SPI0, SPISettings(4000000, MSBFIRST, SPI_MODE0)); // extended init method with SPI channel and/or settings selection
display.setTextColor( GxEPD_BLACK );
/*if (display.pages() > 1)
    {
    delay(100);
    Serial.print("pages = ");
    Serial.print(display.pages());
    Serial.print(" page height = ");
    Serial.println(display.pageHeight());
    delay(1000);
    }*/

/*
display.setFullWindow() ;
display.firstPage();
do  {
    display.fillScreen(GxEPD_WHITE);
    }
while (display.nextPage());*/

// mise hors tension ecran
//display.hibernate();
//display.powerOff();
display.setPartialWindow( 0, 0, 200 , 200 );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Raz de l'ecran.
void CScreen154::ScreenRaz()
{
display.powerOff();
/*display.setFullWindow() ;
display.setFont(&FreeMonoBold18pt7b);
display.firstPage();
do  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 0);
    display.printf("RAZ") ;
    }
while (display.nextPage());*/
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CScreen154::ScreenOff()
{
// mise hors tension ecran
display.powerOff();
//display.end();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de "Connect to Wifi".
void CScreen154::AfficheConnectWifi()
{
display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
do
    {
    // connect to wifi
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(10, 100);
    display.print("Connect to Wifi");
    }
while (display.nextPage());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de la calibration magnetique.
void CScreen154::AfficheCalibreMag()
{
display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
do
    {
    // voltage
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 75);
    display.print("Calibre magnitude (8)");
    }
while (display.nextPage());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de l'adresse ip.
void CScreen154::AfficheWifi(char * IpAdress)
{
display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
do
    {
    // voltage
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 75);
    display.print("http://");
    display.print(IpAdress);
    display.print(":8080");
    }
while (display.nextPage());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche le voltage batterie.
void CScreen154::AfficheVoltage()
{
float Voltage = g_GlobalVar.GetVoltage() ;

// affichage
char TmpChar[10] ;
sprintf( TmpChar , "%1.2fv", Voltage ) ;

display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
//display.setFullWindow();
do
    {
    // voltage
    display.setFont(&FreeMonoBold24pt7b);
    display.setCursor(30, 75);
    display.print(TmpChar);

    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(5, 150);
    display.print(NumVer);
    }
while (display.nextPage());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche un rectangle plein
void CScreen154::DoRect(int x, int y, int w, int h, bool Black )
{
auto Color = (Black) ? GxEPD_BLACK : GxEPD_WHITE ;
display.fillRect(x, y, w, h, Color ); // x y w h
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche un rectangle plein
void CScreen154::DoChar(int x, int y, const char * pChar )
{
//auto Color = (Black) ? GxEPD_BLACK : GxEPD_WHITE ;
//display.fillRect(x, y, w, h, Color ); // x y w h
display.setFont(&FreeMonoBold12pt7b);
display.setCursor(x, y);
display.print( pChar );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 0.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::Ecran0Vz()
{
CLocTermic LocTermic ;

#ifdef _LG_DEBUG_
    Serial.println("Affichage de screen");
#endif

// vitesses verticale
float VitVert = g_GlobalVar.m_VitVertMS ;
bool SigneNeg = VitVert < 0. ;

// VZ ou VZ/finesse sol
char TmpCharVz[15] ;
char TmpCharFin[15] ;
if ( VitVert >= 0. )
    m_MillisVzPositive = millis() ;
bool GrosseVz = ((millis() - m_MillisVzPositive)/1000.) < 10;
if ( GrosseVz )
    {
    if ( SigneNeg )
        sprintf( TmpCharVz , "%2.1f-" , VitVert ) ;
    else
        sprintf( TmpCharVz , " %2.1f " , VitVert ) ;
    }
else
    {
    float FinesseSol = g_GlobalVar.m_FinesseSol ;
    FinesseSol = (FinesseSol>99) ? 99 : FinesseSol ;
    sprintf( TmpCharVz , "%4.1f", VitVert ) ;
    if ( FinesseSol >= 0. )
        sprintf( TmpCharFin , "%4.1f", FinesseSol ) ;
    else
        sprintf( TmpCharFin , "  -" ) ;
    }


// altitude barometrique
char TmpCharAlt[15] ;
sprintf( TmpCharAlt , "%4d", (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;

// derive
const float DeriveMilieu = 41. ;
float DeriveAngle = g_GlobalVar.GetDeriveDeg() ;
char TmpCharAngleDerive[15] ;
if ( fabsf(DeriveAngle) >= 90. )
    sprintf( TmpCharAngleDerive , " \\R/" ) ;
else if ( DeriveAngle >= DeriveMilieu )
    sprintf( TmpCharAngleDerive , "  %1d>>", ((int)(fabsf(DeriveAngle)/10)) ) ;
else if ( DeriveAngle <= -DeriveMilieu )
    sprintf( TmpCharAngleDerive , "<<%1d", ((int)(fabsf(DeriveAngle)/10)) ) ;
else
    sprintf( TmpCharAngleDerive , " ^%1d^", ((int)(fabsf(DeriveAngle)/10)) ) ;

// vitesse sol
char TmpCharVitSol[15] ;
sprintf( TmpCharVitSol , "%3.0f", g_GlobalVar.m_VitesseKmh ) ;

// duree du vol
char TmpCharDV[15] ;
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
    sprintf( TmpCharDV , "  G" ) ;
else if ( g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS )
    sprintf( TmpCharDV , "  S" ) ;
else if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL )
    sprintf( TmpCharDV , "  V" ) ;
else
    sprintf( TmpCharDV , "%3d", g_GlobalVar.m_DureeVolMin ) ;

// cap
int Cap = g_GlobalVar.m_CapGpsDeg ;
int CapMarge = 45/2 + 1 ;
char TmpCharCap[15] ;
char TmpCharNomCap[] = "  " ;
if ( Cap < CapMarge || Cap > (360-CapMarge) )
    strcpy( TmpCharNomCap, "N " ) ;
else if ( labs(Cap-45) < CapMarge )
    strcpy( TmpCharNomCap, "NE" ) ;
else if ( labs(Cap-90) < CapMarge )
    strcpy( TmpCharNomCap, "E " ) ;
else if ( labs(Cap-135) < CapMarge )
    strcpy( TmpCharNomCap, "SE" ) ;
else if ( labs(Cap-180) < CapMarge )
    strcpy( TmpCharNomCap, "S " ) ;
else if ( labs(Cap-225) < CapMarge )
    strcpy( TmpCharNomCap, "SW" ) ;
else if ( labs(Cap-270) < CapMarge )
    strcpy( TmpCharNomCap, "W " ) ;
else if ( labs(Cap-315) < CapMarge )
    strcpy( TmpCharNomCap, "NW" ) ;
sprintf( TmpCharCap , "%2d", (int)(Cap/10) ) ;

// nom/finesse du site le plus proche
float FinesseTerrainMinimum = 99. ;
const CLocTerrain * pTerrain = g_GlobalVar.m_TerrainArr.GetTerrainProche( FinesseTerrainMinimum ) ;
char TmpCharNomSite[15] = "----------" ;
if ( pTerrain != NULL )
    {
    int inspp = 0 ;
    // recopie nom de terrain
    for ( ; inspp < 10 && inspp < pTerrain->m_Nom.length() ; inspp++ )
        TmpCharNomSite[inspp] = pTerrain->m_Nom[inspp] ;
    TmpCharNomSite[inspp] = 0 ;
    }
else
    FinesseTerrainMinimum = 99 ;
char TmpCharFinesseSite[5] ;
sprintf( TmpCharFinesseSite , "%2d" , (int)FinesseTerrainMinimum ) ;

// zone aerienne
g_GlobalVar.m_ZonesAerAll.m_Mutex.PrendreMutex() ;
 std::string NomZoneDessous = g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous ;
 int DansUneZone            = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone ;
 std::string NomZoneLimite  = g_GlobalVar.m_ZonesAerAll.m_NomZoneEnLimite ;
 int LimiteZone             = g_GlobalVar.m_ZonesAerAll.m_LimiteZone ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;

// termic/terrain le plus proche
float CapTermic ;
float DistanceTermic ;
/*if ( g_GlobalVar.m_Config.m_Termic )
    {
    CapTermic = CGlobalVar::GetDiffAngle( g_TermicMap.m_GisementDeg , g_GlobalVar.m_Mpu9250.m_CapMagnetique ) ;
    DistanceTermic = g_TermicMap.m_DistanceMetres ;
    }
else*/
    {
    if ( pTerrain == NULL )
        {
        CapTermic = -1 ;
        DistanceTermic = -1 ;
        }
    else
        {
        CapTermic = CGlobalVar::GetDiffAngle( pTerrain->m_GisementDeg , g_GlobalVar.m_Mpu9250.m_CapMagnetique ) ;
        DistanceTermic = pTerrain->m_DistanceMetres ;
        }
    }

// selection fonts et rotation
display.setRotation(0);
display.setFont(&FreeMonoBold24pt7b);

// affichage valeur de VZ
int16_t tbx, tby;
uint16_t tbw, tbh;


display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
do
    {
    display.fillScreen(GxEPD_WHITE);
    //g_GlobalVar.DoRect( 0 , 0 , 200 , 200 , false );

    ///////////////////////////////////////////
    // nom de zone aerienne ou termique/terrain
    if ( g_GlobalVar.m_Hgt2Agl.m_ErreurFichier )
        {
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(0, 15);
        display.print("*** ERREUR *** * FICHIER HGT*");
        if ( g_GlobalVar.m_BeepAttenteGVZone )
            CGlobalVar::BeepError() ;
        }
    else if ( DansUneZone == ZONE_DEDANS )
        {
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(0, 15);
        display.print(NomZoneDessous.c_str());
        if ( g_GlobalVar.m_BeepAttenteGVZone )
            {
            CGlobalVar::BeepOk() ;
            delay( 150 ) ;
            CGlobalVar::BeepOk() ;
            }
        }
    else if ( DansUneZone == ZONE_LIMITE_ALTI )
        {
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(0, 15);
        display.print(NomZoneDessous.c_str());
        if ( g_GlobalVar.m_BeepAttenteGVZone )
            CGlobalVar::BeepOk() ;
        }
    else if ( LimiteZone == ZONE_LIMITE_FRONTIERE )
        {
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(0, 15);
        display.print(NomZoneLimite.c_str());
        if ( g_GlobalVar.m_BeepAttenteGVZone )
            CGlobalVar::BeepOk() ;
        }
    else
        {
        // thermique
        if ( DistanceTermic != -1 )
            LocTermic.Affiche( CapTermic , DistanceTermic ) ;
        else
            LocTermic.Affiche( -180. , DistanceMaxMetres ) ;

        /////////////
        // bandeaux 1
        const int y1 = 46 ;
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(0, y1);
        display.print(TmpCharNomSite);
        display.setFont(&FreeMonoBold18pt7b);
        display.setCursor(155, y1);
        display.print(TmpCharFinesseSite);
        }

    /////////////
    // bandeaux 2
    const int y2 = 78 ;
    display.setFont(&FreeMonoBold18pt7b);
    // duree du vol
    display.setCursor(0, y2+21);
    display.print(TmpCharDV);
    display.setFont(&FreeMonoBold12pt7b);
    display.print("'");
    display.drawLine( 0 , 107 , 200 , 107 , GxEPD_BLACK ) ; // -
    display.drawLine( 90 , 108 , 90 , 85 , GxEPD_BLACK ) ; // |
    display.drawLine( 200 - 70 , 85 , 200 -70, 107 , GxEPD_BLACK ) ; // |
    display.drawLine( 90 , 85 , 200 -70, 85 , GxEPD_BLACK ) ; // -

    // derive
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(59, y2);
    display.print(TmpCharAngleDerive);

    // cap
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(135, y2+21);
    display.print(TmpCharCap);
    display.setFont(&FreeMonoBold18pt7b);
    display.print(TmpCharNomCap);

    /////////////
    // bandeaux 3
    // VZ / Finesse sol
    if ( GrosseVz )
    // ascendance
        {
        // align with centered HelloWorld
        display.getTextBounds( TmpCharVz, 0, 0, &tbx, &tby, &tbw, &tbh);
        uint16_t xv ;
        if ( SigneNeg )
            xv = (display.width() - tbw) / 2 - 20 ;
        else
            xv = (display.width() - tbw) / 2 - 30 ;

        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(xv, 152);
        display.print(TmpCharVz);

        auto Color = (SigneNeg) ? GxEPD_BLACK : GxEPD_WHITE ;
        display.fillRect(0, 108, 200, 8, Color ); // x y w h
        display.fillRect(0, 160, 200, 8, Color );
        display.fillRect(0, 116, 20, 44, Color );
        display.fillRect(180,116, 20, 44, Color );
        }
    else
        {
        display.setFont(&FreeMonoBold18pt7b);
        display.setCursor(0 , 147);
        display.print(TmpCharVz);
        display.setFont(&FreeMonoBold9pt7b);
        display.print("m");
        display.setFont(&FreeMonoBold18pt7b);
        display.setCursor(110 , 147);
        display.print(TmpCharFin);
        display.drawLine( 105 ,110 ,105 , 160 , GxEPD_BLACK ) ;

        auto Color = (SigneNeg) ? GxEPD_BLACK : GxEPD_WHITE ;
        display.fillRect(0, 108, 105, 7, Color ); // x y w h
        display.fillRect(0, 157, 105, 7, Color );
        display.fillRect(0, 108, 7, 49, Color );
        display.fillRect(98,108, 7, 49, Color );
        }

    /////////////
    // bandeaux 4
    // alti
    display.drawLine( 0 , 163 , 200 , 163 , GxEPD_BLACK ) ;
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(0, 195);
    display.print(TmpCharAlt);
    display.setFont(&FreeMonoBold9pt7b);
    display.print("m");

    // vitesse sol
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(120, 195);
    display.print(TmpCharVitSol);
    display.setFont(&FreeMonoBold9pt7b);
    display.print("k");
    display.drawLine( 115 ,200 ,115 , 163 , GxEPD_BLACK ) ;
    }
while (display.nextPage());

// si changement d'ecran si pas en mode V ou G/S
if ( g_GlobalVar.m_DureeVolMin != ATTENTE_VITESSE_VOL &&
     g_GlobalVar.m_DureeVolMin != ATTENTE_STABILITE_GPS &&
     g_GlobalVar.m_DureeVolMin != ATTENTE_MESSAGE_GPS &&
     BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_1_Histo ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_3_Sys ;
    }

// si activation / desactivation beep attente Gps / Vitesse
if ( BoutonCentre() )
    g_GlobalVar.m_BeepAttenteGVZone = ! g_GlobalVar.m_BeepAttenteGVZone ;

return ECRAN_0_Vz ;

//display.hibernate();
//display.powerOff();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 1. Historique de vol.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::Ecran1Histo()
{
char TmpCharAltiDeco[20] ;
sprintf( TmpCharAltiDeco , "%4dm", (int)g_GlobalVar.m_HistoVol.m_ZDeco ) ;

char TmpCharAltiMax[20] ;
sprintf( TmpCharAltiMax , "%4dm", g_GlobalVar.m_HistoVol.m_ZMax ) ;

char TmpCharVzMax[20] ;
sprintf( TmpCharVzMax , "%5.1f", g_GlobalVar.m_HistoVol.m_VzMax ) ;

char TmpCharVzMin[20] ;
sprintf( TmpCharVzMin , "%5.1f", g_GlobalVar.m_HistoVol.m_VzMin ) ;

char TmpCharVsMax[20] ;
sprintf( TmpCharVsMax , "%5.1f", g_GlobalVar.m_HistoVol.m_VsMax ) ;

char TmpCharDistMax[20] ;
sprintf( TmpCharDistMax , "%5.1f", g_GlobalVar.m_HistoVol.m_DistanceKm ) ;

// temps de vol
char TmpCharTV[20] ;
sprintf( TmpCharTV , " %3d'", g_GlobalVar.m_HistoVol.m_TempsDeVol ) ;

// zone au dessus
std::string NomZone = "" ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.PrendreMutex() ;
 if ( g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone )
    NomZone = g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;

display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
    display.fillScreen(GxEPD_WHITE);

    // alti decollage
    display.setCursor(0, 20);
    display.print("Z deco:");
    display.setCursor(110, 20);
    display.print(TmpCharAltiDeco);

    // alti max
    display.setCursor(0, 40);
    display.print("Z max :");
    display.setCursor(110, 40);
    display.print(TmpCharAltiMax);

    // Vz max
    display.setCursor(0, 60);
    display.print("Vz max:");
    display.setCursor(110, 60);
    display.print(TmpCharVzMax);

    // Vz min
    display.setCursor(0, 80);
    display.print("Vz min:");
    display.setCursor(110, 80);
    display.print(TmpCharVzMin);

    // distance max
    display.setCursor(0, 100);
    display.print("Dist. :");
    display.setCursor(110, 100);
    display.print(TmpCharDistMax);

    // Vs max
    display.setCursor(0, 120);
    display.print("Vs max:");
    display.setCursor(110, 120);
    display.print(TmpCharVsMax);

    // Dure vol
    display.setCursor(0,140);
    display.print("t vol :");
    display.setCursor(110, 140);
    display.print(TmpCharTV);

    // nom zone
    display.setCursor(0,160);
    display.print( NomZone.c_str() );
    }
while (display.nextPage());

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_2a_TmaAll ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_0_Vz ;
    }

// si changement d'ecran
if ( BoutonCentre() )
    {
    g_GlobalVar.m_Config.LectureFichier() ;
    return ECRAN_4_CfgFch ;
    }

return ECRAN_1_Histo ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 3, orienté systeme.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::Ecran3Sys()
{
// date
char TmpCharDate[35] ;
int secondes_date = g_GlobalVar.m_HeureSec ;
sprintf( TmpCharDate ,"%02d%02d%02d-%02d:%02d" ,
    (int)(g_GlobalVar.m_Annee - 2000) ,
    g_GlobalVar.m_Mois ,
    g_GlobalVar.m_Jour ,
    (int) (secondes_date/3600) ,   // heure
    (int)((secondes_date/60)%60)   // minutes
    ) ;

// v batterie
char TmpCharVB[20] ;
sprintf( TmpCharVB , "%1.2fv", g_GlobalVar.GetVoltage() ) ;

// cap magnetique
char TmpCharCM[20] ;
sprintf( TmpCharCM , " %3dd", (int)g_GlobalVar.m_Mpu9250.m_CapMagnetique ) ;

// alti baro
char TmpAltiBaro[20] ;
sprintf( TmpAltiBaro , "%4.1fm", g_GlobalVar.m_MS5611.GetAltiMetres() ) ;

// memoire
char TmpCharMem[20] ;

// pourcantage utilisation cpu
g_GlobalVar.m_MutexCore.PrendreMutex() ;
 int cpu0 = g_GlobalVar.m_PercentCore0 ;
 int cpu1 = g_GlobalVar.m_PercentCore1 ;
g_GlobalVar.m_MutexCore.RelacherMutex() ;

display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
    //display.fillScreen(GxEPD_WHITE);
    // date et heure
    display.setCursor(12, 15);
    display.print(TmpCharDate) ;

    // alti baro
    display.setCursor(0, 55);
    display.print("Al bar:");
    display.setCursor(110,55);
    display.print(TmpAltiBaro);

    // Cap Magnetique
    display.setCursor(0, 75);
    display.print("Cap m :");
    display.setCursor(110,75);
    display.print(TmpCharCM);

    // core 0 usage
    display.setCursor(0,95);
    display.print("core 0:   ");
    display.print(cpu0) ;
    display.print("%");

    // core 1 usage
    display.setCursor(0,115);
    display.print("core 1:   ");
    display.print(cpu1) ;
    display.print("%");

    // memory
    display.setCursor(0,135);
    sprintf( TmpCharMem , "f mem:%6db", (int) esp_get_free_heap_size() ) ;
    display.print(TmpCharMem);

    // batterie
    display.setCursor(0,155);
    display.print("V bat :");
    display.setCursor(110,155);
    display.print(TmpCharVB);

    // firmware
    display.setCursor(0, 195);
    display.print("fir:");
    display.print(NumVer);
    }
while (display.nextPage());

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_0_Vz ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_2a_TmaAll ;
    }

return ECRAN_3_Sys ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche/modifie les [variables] du fichier de
/// configuration.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::Ecran4CfgFch()
{
static char TmpMod[100] = {0} ;
std::string Name ;
std::string Value ; ;

static bool Modif = false ;
static int iChamps = -1 ;

// sortie
if ( iChamps == -1 )
    {
    strcpy( TmpMod , "" ) ;
    Name = "Boutons <GCD>" ;
    }
else
    g_GlobalVar.m_Config.GetChar( iChamps , Name , Value ) ;

display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
    display.fillScreen(GxEPD_WHITE);

    // Mod
    display.setCursor(0, 20);
    display.print(TmpMod) ;
    // NomVar
    display.setCursor(0, 60);
    display.print(Name.c_str()) ;
    // ValNomVar
    display.setCursor(0, 140);
    display.print(Value.c_str()) ;
    }
while (display.nextPage());

bool BoutonCent = BoutonCentre() ;
bool BoutonGau  = BoutonGauche() ;
bool BoutonDroi = BoutonDroit() ;
// sortie ecran
if ( BoutonCent && !Modif && iChamps == -1 )
    {
    m_MillisEcran0 = millis() ;
    g_GlobalVar.m_Config.EcritureFichier() ;
    g_GlobalVar.m_Config.LectureFichier() ;
    return ECRAN_1_Histo ;
    }

// modif
if ( BoutonCent && iChamps != -1 )
    {
    Modif = !Modif ;
    if ( Modif )
        strcpy( TmpMod , "Modification" ) ;
    else
        strcpy( TmpMod , "" ) ;
    }

if ( BoutonGau && Modif )
    {
    CConfigFile::st_line * pLine = g_GlobalVar.m_Config.m_LinesVect[iChamps] ;
    if ( pLine->m_Type == TYPE_VAR_FLOAT )
        {
        float * pVal = (float*) pLine->m_pVar ;
        *pVal -= 0.05 ;
        }
    else if ( pLine->m_Type == TYPE_VAR_INT )
        {
        int *pVal = (int*) pLine->m_pVar ;
        int inc = 10 ;
        if ( abs(*pVal) <= 60 )
            inc = 1 ;
        *pVal -= inc ;
        }
    else if ( pLine->m_Type == TYPE_VAR_BOOL )
        {
        bool *pVal = (bool*) pLine->m_pVar ;
        *pVal = !*pVal ;
        }
    }

if ( BoutonDroi && Modif )
    {
    CConfigFile::st_line * pLine = g_GlobalVar.m_Config.m_LinesVect[iChamps] ;
    if ( pLine->m_Type == TYPE_VAR_FLOAT )
        {
        float * pVal = (float*) pLine->m_pVar ;
        *pVal += 0.05 ;
        }
    else if ( pLine->m_Type == TYPE_VAR_INT )
        {
        int *pVal = (int*) pLine->m_pVar ;
        int inc = 10 ;
        if ( abs(*pVal) <= 60 )
            inc = 1 ;
        *pVal += inc ;
        }
    else if ( pLine->m_Type == TYPE_VAR_BOOL )
        {
        bool *pVal = (bool*) pLine->m_pVar ;
        *pVal = !*pVal ;
        }
    }

// defilement
if (  BoutonGau && !Modif )
    {
    if ( iChamps > -1 )
        iChamps-- ;
    else
        iChamps = g_GlobalVar.m_Config.m_LinesVect.size()-1 ;
    }

if ( BoutonDroi && !Modif  )
    {
    int size = (g_GlobalVar.m_Config.m_LinesVect.size()-1) ;
    if ( iChamps < size )
        iChamps++ ;
    else
        iChamps = -1 ;
    }

return ECRAN_4_CfgFch ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 2b, TAM/CTR
/// modification activation.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::Ecran2bTmaMod()
{
static int NumTmaCtr = -1 ;

// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

// construction du tableau des zones
std::vector<CZoneAer*> VecZonesMod ;
std::vector<CZoneAer*> VecZonesConst ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetNbZones() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetZoneArr() ;

// vecteur des zones a afficher
for ( long iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;

    // selection modifible / constante
    std::vector<CZoneAer*> * pVect ;
    if ( pZone->m_DansFchActivation )
        pVect = & VecZonesMod ;
    else
        pVect = & VecZonesConst ;

    // test si deja affiché
    long DejaFait = false ;
    for ( int iv = 0 ; iv < pVect->size() ; iv++ )
        {
        if ( (*pVect)[iv]->m_NomAff == pZone->m_NomAff )
            {
            DejaFait = true ;
            break ;
            }
        }

    // ajout du nom de zone
    if ( DejaFait )
        continue ;

    pVect->push_back( pZone ) ;
    }

// selection zone
CZoneAer * pZone = NULL ;
if ( NumTmaCtr >= 0 && NumTmaCtr < VecZonesMod.size() )
    pZone = VecZonesMod[NumTmaCtr] ;

int NumTmaCtrConst = NumTmaCtr - VecZonesMod.size() ;
if ( NumTmaCtrConst >= 0 && NumTmaCtrConst < VecZonesConst.size() )
    pZone = VecZonesConst[NumTmaCtrConst] ;

display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
    display.fillScreen(GxEPD_WHITE);

    // titre
    if ( pZone == NULL )
        {
        display.setCursor(10, 70);
        display.print("Ret  ^^^ Cent.");
        display.setCursor(5, 90);
        display.print("<Mod. B. G.D.>");
        }
    else
        {
        // num tma/ctr
        display.setCursor(0, 15);
        display.print(NumTmaCtr);
        display.print(":");
        // nom
        //display.setCursor(0, 35);
        display.print(pZone->m_NomAff.c_str());
        // activation
        display.setCursor(0, 60);
        display.print( "Active:");
        if ( pZone->m_Activee )
            display.print( "1");
        else
            display.print( "0");
        if ( pZone->m_DansFchActivation )
            display.print( " mod.");
        else
            display.print( " const");
        // altibasse
        display.setCursor(0, 80);
        display.print( "A bas : ");
        display.print( pZone->m_AltiBasse );

        // periode
        display.setCursor(0,100);
        display.print( "p.de. : ");
        display.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeDebutJour : -1 );
        display.print( "-" );
        display.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeDebutMois : -1 );
        display.setCursor(0,120);
        display.print( "p.fi. : ");
        display.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeFinJour : -1 );
        display.print( "-" );
        display.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeFinMois : -1 );

        // altisemaine
        display.setCursor(0,140);
        display.print( "A sem. : ");
        display.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_AltiBassePeriodeSemaine : -1 );
        // altiweekend
        display.setCursor(0,160);
        display.print( "A week : ");
        display.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_AltiBassePeriodeWeekEnd : -1 );

        }
    }
while (display.nextPage());

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
bool BCentre = BoutonCentre() ;
if ( BCentre && pZone == NULL )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_2a_TmaAll ;
    }

// si modification activation
if ( BCentre && pZone != NULL )
    {
    m_MillisEcran0 = millis() ;
    if ( pZone->m_DansFchActivation )
        {
        pZone->m_Activee = !pZone->m_Activee ;
        g_GlobalVar.m_ZonesAerAll.EcritureFichierZonesActive() ;
        }
    return ECRAN_2a_TmaAll ;
    }

// decrementation numero de zone
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    NumTmaCtr-- ;
    if ( NumTmaCtr < -1 )
        NumTmaCtr = NbZones - 1 ;
    }

// incrementation numero de zone
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    int Size = VecZonesMod.size()+VecZonesConst.size()-1 ;
    NumTmaCtr++ ;
    if ( NumTmaCtr > Size )
        NumTmaCtr = 0 ;
    }

return ECRAN_2b_TmaMod ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 2a, TAM/CTR titre
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::Ecran2aTmaAll()
{
// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

// construction du tableau des zones
std::vector<CZoneAer*> VecZonesMod ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetNbZones() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetZoneArr() ;

// vecteur des zones activables
for ( long iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;
    if ( pZone->m_DansFchActivation )
        VecZonesMod.push_back( pZone ) ;
    }

// titre
char TmpTitre[15] ;
sprintf( TmpTitre , "%2d B. Cen. Mo." , g_GlobalVar.m_ZonesAerAll.GetNbZones() ) ;

display.setPartialWindow( 0, 0, 200 , 200 );
display.firstPage();
do
    {
    display.fillScreen(GxEPD_WHITE);

    // titre
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor( 0, 20 );
    display.print( TmpTitre ) ;

    // zones active
    long xcol = 0 ;
    long yligne = 15 ;
    display.setFont(&FreeMonoBold9pt7b);
    for ( int iz = 0 ; iz < VecZonesMod.size() ; iz++ )
        {
        display.setCursor(0+xcol, 40 + yligne );
        if ( VecZonesMod[iz]->m_Activee )
            display.print( VecZonesMod[iz]->m_NomAff.c_str() ) ;
        else
            display.print( "-" ) ;
        yligne += 17 ;
        if ( iz == 8 )
            {
            xcol = 105 ;
            yligne = 15 ;
            }
        }
    }
while (display.nextPage());

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_3_Sys ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_1_Histo ;
    }

// si changement modification zone
if ( BoutonCentre() )
    return ECRAN_2b_TmaMod ;

return ECRAN_2a_TmaAll ;
}

#endif
