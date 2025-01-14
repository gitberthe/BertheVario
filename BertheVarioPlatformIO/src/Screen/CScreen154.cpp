////////////////////////////////////////////////////////////////////////////////
/// \file CScreen154.cpp
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 15/01/2025
///

#include "../BertheVario.h"

//#define ENABLE_GxEPD2_GFX

#if ( TYPE_SCREEN == 154 )

// comment out unused bitmaps to reduce code space used
//#include <bitmaps/Bitmaps200x200.h> // 1.54" b/w

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
//#define ENABLE_GxEPD2_GFX 1

#include <GxEPD2_BW.h>

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <Arduino_GFX.h>
//#include <GxEPD2_GFX.h>
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
//display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
//display.init(0,false,5,true) ;
display.init(0,false,15,true) ;

//hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
//display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));

//display.init(115200, true, 100, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
//display.init(115200, true, 10, false, SPI0, SPISettings(4000000, MSBFIRST, SPI_MODE0)); // extended init method with SPI channel and/or settings selection
display.setTextColor( GxEPD_BLACK );
/*if (display.pages() > 1)
    {
    Serial.print("pages = ");
    Serial.print(display.pages());
    Serial.print(" page height = ");
    Serial.println(display.pageHeight());
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
display.setRotation(0) ;
//display.setFullWindow() ;
display.setPartialWindow( 0, 0, 200 , 200 );

// raz page precedente
display.fillRect(0,0, 200, 200, GxEPD_BLACK ); // x y w h
display.display(true) ;
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.display(true) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Raz de l'ecran (deja fait dans display.firstPage() donc doublon)
void CScreen154::ScreenRaz()
{
//display.powerOff();
//display.setFullWindow() ;
//display.setFont(&FreeMonoBold18pt7b);
/*display.firstPage();
do  {
    display.fillScreen(GxEPD_BLACK);
    }
while (display.nextPage()); */

display.firstPage();
do  {
    //display.fillScreen(GxEPD_WHITE);
    }
while (display.nextPage());
//display.powerOff();
//display.refresh() ;

//display.clearScreen() ;
/*uint8_t * bitmap = new uint8_t[200*200] ;
for ( int i = 0 ; i < 200*200 ; i++ )
    bitmap[i] = 0 ;
display.writeImage(  bitmap, 0,0,200,200) ;
delete [] bitmap ; */
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Raz screen en reboot
void CScreen154::ScreenOff()
{
display.setFullWindow() ;

display.clearScreen() ;

display.firstPage();
do
    {
    //display.fillScreen(GxEPD_BLACK);
    display.fillScreen(GxEPD_WHITE);
    }
while (display.nextPage());

// mise hors tension ecran
display.powerOff();
//display.end();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de "Connect to Wifi".
void CScreen154::AfficheConnectWifi()
{
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

display.firstPage();
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
void CScreen154::DoRect(int x, int y, int w, int h )
{
//auto Color = (Black) ? GxEPD_BLACK : GxEPD_WHITE ;
display.fillRect(x, y, w, h, GxEPD_BLACK ); // x y w h
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
CGestEcrans::EtatsAuto CScreen154::EcranVz()
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
sprintf( TmpCharVitSol , "%4.1f", g_GlobalVar.m_VitesseKmh ) ;

// hauteur sol
char TmpCharHauteurSol[15] ;
sprintf( TmpCharHauteurSol , "%4d", (int)(g_GlobalVar.m_TerrainPosCur.m_AltiBaro-g_GlobalVar.m_AltitudeSolHgt) ) ;

// variables pour affichage vitesse/hauteur sol
static bool AffichageHauteurSol = false ;
static unsigned long TempsHauteurSol = millis() ;
// si 5 secondes depassée
if ( AffichageHauteurSol && ((millis()-TempsHauteurSol)/1000) >= 5 )
    {
    AffichageHauteurSol = !AffichageHauteurSol ;
    TempsHauteurSol = millis() ;
    }
else if ( !AffichageHauteurSol && ((millis()-TempsHauteurSol)/1000) >= 1 )
    {
    AffichageHauteurSol = !AffichageHauteurSol ;
    TempsHauteurSol = millis() ;
    }

// duree du vol
char TmpCharDV[15] ;
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
    sprintf( TmpCharDV , "%2dG" , g_GlobalVar.GetNbSat() ) ;
else if ( g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS )
    sprintf( TmpCharDV , "%2dS" , g_GlobalVar.GetNbSat()) ;
else if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL )
    sprintf( TmpCharDV , "%2dV" , g_GlobalVar.GetNbSat()) ;
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

// raz page precedente
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

// affichage valeur de VZ
int16_t tbx, tby;
uint16_t tbw, tbh;

///////////////////////////////////////////
// nom de zone aerienne ou termique/terrain
if ( g_GlobalVar.m_Hgt2Agl.m_ErreurFichier )
    {
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(0, 15);
    display.print("*** ERREUR *** * FICHIER HGT*");
    if ( g_GlobalVar.m_BeepAttenteGVZone )
        CGlobalVar::BeepError(true) ;
    }
else if ( DansUneZone == ZONE_DEDANS )
    {
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(0, 15);
    display.print(NomZoneDessous.c_str());
    if ( g_GlobalVar.m_BeepAttenteGVZone )
        {
        CGlobalVar::BeepOk() ;
        CGlobalVar::beeper( SOUND_DELAY_ONLY , 150 ) ;
        CGlobalVar::BeepOk() ;
        }
    }
else if ( LimiteZone == ZONE_LIMITE_ALTI )
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
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL )
    display.print(" ");
else
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
    // align with centered
    display.getTextBounds( TmpCharVz, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t xv ;
    if ( SigneNeg )
        xv = (display.width() - tbw) / 2 - 20 ;
    else
        xv = (display.width() - tbw) / 2 - 30 ;

    display.setFont(&FreeMonoBold24pt7b);
    display.setCursor(xv, 152);
    display.print(TmpCharVz);

    //auto Color = (SigneNeg) ? GxEPD_BLACK : GxEPD_WHITE ;
    if ( SigneNeg )
        {
        display.fillRect(0, 108, 200, 8, GxEPD_BLACK ); // x y w h
        display.fillRect(0, 160, 200, 8, GxEPD_BLACK );
        display.fillRect(0, 116, 20, 44, GxEPD_BLACK );
        display.fillRect(180,116, 20, 44, GxEPD_BLACK );
        }
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

    //auto Color = (SigneNeg) ? GxEPD_BLACK : GxEPD_WHITE ;
    if ( SigneNeg )
        {
        display.fillRect(0, 108, 105, 7, GxEPD_BLACK ); // x y w h
        display.fillRect(0, 157, 105, 7, GxEPD_BLACK );
        display.fillRect(0, 108, 7, 49, GxEPD_BLACK );
        display.fillRect(98,108, 7, 49, GxEPD_BLACK );
        }
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

// vitesse sol/hauteur sol
display.setFont(&FreeMonoBold18pt7b);
display.setCursor(102, 195);
display.drawLine( 100 ,200 ,100 , 163 , GxEPD_BLACK ) ;
if ( AffichageHauteurSol )
    {
    display.print(TmpCharVitSol);
    display.setFont(&FreeMonoBold9pt7b);
    display.print("k");
    }
else
    {
    display.print(TmpCharHauteurSol);
    display.setFont(&FreeMonoBold9pt7b);
    display.print("m");
    }

// attente nouvelle page
//while (display.nextPage()) delay(1) ;
display.display(true) ;

/*// si changement d'ecran si pas en mode V ou G/S
if ( g_GlobalVar.m_DureeVolMin != ATTENTE_VITESSE_VOL &&
     g_GlobalVar.m_DureeVolMin != ATTENTE_STABILITE_GPS &&
     g_GlobalVar.m_DureeVolMin != ATTENTE_MESSAGE_GPS &&
     BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_1_Histo ;
    }*/

// si changement d'ecran
if ( BoutonCentre() )
    return ECRAN_1_Histo ;

// si activation / desactivation beep attente Gps / Vitesse
if ( /*(g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
      g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
      g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS ) &&*/
      BoutonGauche() )
    g_GlobalVar.m_BeepAttenteGVZone = ! g_GlobalVar.m_BeepAttenteGVZone ;

return ECRAN_0_Vz ;

//display.hibernate();
//display.powerOff();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 1. Historique de vol.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranHisto()
{
static int ivol = 0 ;
int y = 20 ;

// lecture des histo
g_GlobalVar.m_HistoVol.LectureFichiers() ;

// si pas de fichiers histo
if ( g_GlobalVar.m_HistoVol.m_HistoDir.size() == 0 )
    {
    display.firstPage();
    display.setFont(&FreeMonoBold12pt7b);
    do  {
        // message
        display.setCursor(0, 20);
        display.print("0 histo");
        }
    while (display.nextPage());

    goto fin_histo ;
    }

char TmpCharNomFchIgc[20] ;
sprintf( TmpCharNomFchIgc , "%s", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_NomIgc ) ;

char TmpCharAltiDeco[20] ;
sprintf( TmpCharAltiDeco , "%4dm", (int)g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_ZDeco ) ;

char TmpCharAltiMax[20] ;
sprintf( TmpCharAltiMax , "%4dm", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_ZMax ) ;

char TmpCharVzMax[20] ;
sprintf( TmpCharVzMax , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_VzMax ) ;

char TmpCharVzMin[20] ;
sprintf( TmpCharVzMin , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_VzMin ) ;

char TmpCharVsMax[20] ;
sprintf( TmpCharVsMax , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_VsMax ) ;

char TmpCharDistMax[20] ;
sprintf( TmpCharDistMax , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_DistanceKm ) ;

// temps de vol
char TmpCharTV[20] ;
sprintf( TmpCharTV , " %3d'", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_TempsDeVol ) ;

display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
    // nom fch igc
    display.setCursor(0, y);
    display.print(TmpCharNomFchIgc);

    // alti decollage
    y += 40 ;
    display.setCursor(0, y);
    display.print("Z deco:");
    display.setCursor(110, y);
    display.print(TmpCharAltiDeco);

    // alti max
    y += 20 ;
    display.setCursor(0, y);
    display.print("Z max :");
    display.setCursor(110, y);
    display.print(TmpCharAltiMax);

    // Vz max
    y += 20 ;
    display.setCursor(0, y);
    display.print("Vz max:");
    display.setCursor(110, y);
    display.print(TmpCharVzMax);

    // Vz min
    y += 20 ;
    display.setCursor(0, y);
    display.print("Vz min:");
    display.setCursor(110, y);
    display.print(TmpCharVzMin);

    // distance max
    y += 20 ;
    display.setCursor(0, y);
    display.print("Dist. :");
    display.setCursor(110, y);
    display.print(TmpCharDistMax);

    // Vs max
    y += 20 ;
    display.setCursor(0, y);
    display.print("Vs max:");
    display.setCursor(110, y);
    display.print(TmpCharVsMax);

    // Dure vol
    y += 20 ;
    display.setCursor(0,y);
    display.print("t vol :");
    display.setCursor(110, y);
    display.print(TmpCharTV);
    }
while (display.nextPage());

// fin de la fonction
fin_histo :

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement de numero histo vol
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    ivol++ ;
    if ( ivol >= g_GlobalVar.m_HistoVol.m_HistoDir.size() )
        ivol = g_GlobalVar.m_HistoVol.m_HistoDir.size() - 1 ;
    g_GlobalVar.m_HistoVol.m_HistoDir.clear() ;
    return ECRAN_1_Histo ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    ivol-- ;
    if ( ivol < 0 )
        ivol = 0 ;
    g_GlobalVar.m_HistoVol.m_HistoDir.clear() ;
    return ECRAN_1_Histo ;
    }

// si changement d'ecran
if ( BoutonCentre() )
    {
    g_GlobalVar.m_HistoVol.m_HistoDir.clear() ;
    return ECRAN_2a_ListeIgc ;
    }

g_GlobalVar.m_HistoVol.m_HistoDir.clear() ;
return ECRAN_1_Histo ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 3, orienté systeme.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranSys()
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
sprintf( TmpAltiBaro , "%4.0fm", g_GlobalVar.m_MS5611.GetAltiMetres() ) ;

// memoire
char TmpCharMem[20] ;

// pourcantage utilisation cpu
g_GlobalVar.m_MutexCore.PrendreMutex() ;
 int cpu0 = g_GlobalVar.m_PercentCore0 ;
 int cpu1 = g_GlobalVar.m_PercentCore1 ;
g_GlobalVar.m_MutexCore.RelacherMutex() ;

display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
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
if ( BoutonCentre() )
    return ECRAN_0_Vz ;

return ECRAN_6_Sys ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche/modifie les [variables] du fichier de
/// configuration.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranCfgFch()
{
static char TmpMod[100] = {0} ;
std::string Name ;
std::string Value ; ;

static bool Modif = false ;
static int iChamps = -1 ;

// si nouvelle page
if ( IsPageChanged() )
    g_GlobalVar.m_Config.ConstructVect() ;

// sortie
if ( iChamps == -1 )
    {
    strcpy( TmpMod , "" ) ;
    Name = " Editeur Cfg\nBoutons <GCD>" ;
    }
else
    {
    g_GlobalVar.m_Config.GetChar( iChamps , Name , Value ) ;
    m_MillisEcran0 = millis() ;
    }

display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
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
    g_GlobalVar.m_Config.EcritureFichier() ;
    g_GlobalVar.m_Config.LectureFichier() ;
    return ECRAN_5_TmaDessous ;
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

// decrementation variable
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

// incrementation variable
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

// si time out ecran en premeire page
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( ((Temps/1000) > m_SecRetourEcran0) && (iChamps == -1) )
    return ECRAN_0_Vz ;


return ECRAN_4_CfgFch ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 2b, TAM/CTR
/// modification activation.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranTmaMod()
{
static int NumTmaCtr = -1 ;

// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

// construction du tableau des zones
std::vector<CZoneAer*> VecAffZones ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetNbZones() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetZoneArr() ;

// construction vecteur des zones a afficher
for ( long iz = 0 ; iz < NbZones ; iz++ )
    {
    if ( pZoneArr[iz]->m_DansFchActivation )
        VecAffZones.push_back( pZoneArr[iz] ) ;
    }
for ( long iz = 0 ; iz < NbZones ; iz++ )
    {
    if ( !pZoneArr[iz]->m_DansFchActivation )
        VecAffZones.push_back( pZoneArr[iz] ) ;
    }

// selection des zones de meme nom que selectionnee
std::vector<CZoneAer *> VecZone2Mod ;
if ( NumTmaCtr >= 0 && NumTmaCtr < VecAffZones.size() )
    {
    CZoneAer * pZone = VecAffZones[NumTmaCtr] ;
    for ( long iz = 0 ; iz < VecAffZones.size() ; iz++ )
        if ( ! strcmp( VecAffZones[iz]->m_pNomAff , pZone->m_pNomAff ) )
            VecZone2Mod.push_back( VecAffZones[iz] ) ;
    }

display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
    // titre
    if ( VecZone2Mod.size() == 0 )
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
        CZoneAer * pZone = VecAffZones[NumTmaCtr] ;
        display.print(pZone->m_pNomAff);
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

        // si zone protegee
        if ( pZone->IsProtect() )
            {
            display.setCursor(0,180);
            display.print( "hau sol: ");
            display.print( pZone->GetHauteurSolZoneProtect() );
            }
        }
    }
while (display.nextPage());

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
bool BCentre = BoutonCentre() ;
if ( BCentre && VecZone2Mod.size() == 0 )
    return ECRAN_3a_TmaAll ;

// si modification activation
if ( BCentre && VecZone2Mod.size() != 0 )
    {
    m_MillisEcran0 = millis() ;
    for ( long iz = 0 ; iz < VecZone2Mod.size() ; iz++ )
        {
        CZoneAer * pZone = VecZone2Mod[iz] ;
        if ( pZone->m_DansFchActivation )
            {
            pZone->m_Activee = !pZone->m_Activee ;
            g_GlobalVar.m_ZonesAerAll.EcritureFichierZonesActive() ;
            }
        }
    return ECRAN_3a_TmaAll ;
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
    int Size = VecAffZones.size()-1 ;
    NumTmaCtr++ ;
    if ( NumTmaCtr > Size )
        NumTmaCtr = 0 ;
    }

return ECRAN_3b_TmaMod ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 2a, TAM/CTR titre
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranTmaAll()
{
// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

// construction du tableau des zones
std::vector<CZoneAer*> VecZonesMod ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetNbZones() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetZoneArr() ;

// ajout vecteur des zones activables
for ( long iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;
    if ( pZone->m_DansFchActivation )
        VecZonesMod.push_back( pZone ) ;
    }
// ajout vecteur des zones activables
for ( long iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;
    if ( !pZone->m_DansFchActivation )
        VecZonesMod.push_back( pZone ) ;
    }

// titre
char TmpTitre[15] ;
sprintf( TmpTitre , "%2d B. Cen. Mo." , g_GlobalVar.m_ZonesAerAll.GetNbZones() ) ;

display.firstPage();
do
    {
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
        if ( !VecZonesMod[iz]->m_DansFchActivation )
            continue ;
        display.setCursor(0+xcol, 40 + yligne );

        if ( VecZonesMod[iz]->m_Activee )
            display.print( VecZonesMod[iz]->m_pNomAff ) ;
        else
            {
            char TmpChar[25] ;
            sprintf( TmpChar , "-%s" ,  VecZonesMod[iz]->m_pNomAff ) ;
            TmpChar[9] = 0 ;
            display.print( TmpChar  ) ;
            }
        yligne += 17 ;
        if ( iz == 8 )
            {
            xcol = 110 ;
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
    return ECRAN_3b_TmaMod ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_3b_TmaMod ;
    }

// si changement modification zone
if ( BoutonCentre() )
    return ECRAN_4_CfgFch ;

return ECRAN_3a_TmaAll ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations des fichiers IGC de la carte
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranListeIgcFch()
{
static std::vector<std::string> VecNomIgc ;
static std::vector<int> VecTempsIgc ;

// lecture de fichier
g_GlobalVar.ListeIgc(VecNomIgc,VecTempsIgc) ;

int TotalMin = 0 ;
int y_cursor ;
for ( int ifch = 0 ; ifch < VecNomIgc.size() ; ifch++ )
    TotalMin += VecTempsIgc[ifch] ;

char TmpChar[25] ;
display.firstPage();
do
    {
    display.setFont(&FreeMonoBold12pt7b);

    display.setFont(&FreeMonoBold9pt7b);
    int ivec = 0 ;
    y_cursor = 10 ;
    for ( ; ivec < VecNomIgc.size() ; ivec++ )
        {
        sprintf( TmpChar , "%s %03d", (const char*)VecNomIgc[ivec].c_str() , VecTempsIgc[ivec] ) ;
        y_cursor += 16 ;
        display.setCursor( 0, y_cursor );
        display.print( TmpChar ) ;
        }

    display.setFont(&FreeMonoBold12pt7b);
    sprintf( TmpChar , "tot. igc:%03dm", TotalMin ) ;
    display.setCursor( 0, y_cursor + 25 );
    display.print( TmpChar ) ;
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
    return ECRAN_2b_ConfirmArchIgc ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    return ECRAN_2b_ConfirmArchIgc ;
    }

// si changement modification zone
if ( BoutonCentre() )
    return ECRAN_3a_TmaAll ;

return ECRAN_2a_ListeIgc ;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction permet d'archiver tous les fichier IGC de la carte qui
/// sont à la racine et ayant un fichier histo.
/// Les autres petits fichiers sont detruits.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranConfimeArchIgcFch()
{
// titre
char TmpChar[] = "\n\n   Confirme\n   Archivage\n     Igc\n  Bouton GD" ;

display.firstPage();
do
    {
    // titre
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor( 0, 20 );
    display.print( TmpChar ) ;
    }
while (display.nextPage());

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
if ( BoutonCentre() )
    return ECRAN_2a_ListeIgc ;

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    g_GlobalVar.ArchiveIgc() ;
    return ECRAN_2a_ListeIgc ;
    }

// si confirme delete igc
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    g_GlobalVar.ArchiveIgc() ;
    return ECRAN_2a_ListeIgc ;
    }

return ECRAN_2b_ConfirmArchIgc ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction permet de connaitre la TMA dessus.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranTmaDessous()
{
static int NbAppButGD = 0 ;
std::string NomZone = "" ;

if ( IsPageChanged() )
    NbAppButGD = 0 ;

// zone au dessus
g_GlobalVar.m_ZonesAerAll.m_Mutex.PrendreMutex() ;
 if ( g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone )
    NomZone = g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran ou non enregistrement point
if ( BoutonCentre() )
    {
    if ( NbAppButGD == 0 )
        return ECRAN_6_Sys ;
    m_MillisEcran0 = millis() ;
    NbAppButGD=0 ;
    }

// pour memorisation point
if ( BoutonGauche() || BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    NbAppButGD++ ;
    }

// demande confirmation enregistrement point
if ( NbAppButGD == 1 )
    {
    display.firstPage();
    display.setFont(&FreeMonoBold12pt7b);
    do
        {
        display.setCursor(0,70);
        display.print( "Confirmation enregistrement point gps GD" );
        }
    while (display.nextPage());
    return ECRAN_5_TmaDessous ;
    }

// enregistrement point dans
if ( NbAppButGD >= 2 )
    {
    char TmpChar[50] ;
    NbAppButGD = 0 ;
    File FchTerCon = SD.open(TERRAIN_FCH,FILE_APPEND);
    if ( !FchTerCon )
        return ECRAN_5_TmaDessous ;
    FchTerCon.seek(SeekEnd) ;
    // nom
    sprintf(TmpChar,"pt_%d%d%d    ",(int)g_GlobalVar.m_TerrainPosCur.m_AltiBaro,
                                       (int)(10*g_GlobalVar.m_TerrainPosCur.m_Lat),
                                       (int)(10*g_GlobalVar.m_TerrainPosCur.m_Lon)) ;
    FchTerCon.write((const uint8_t*)TmpChar,strlen(TmpChar)) ;
    // altitude
    sprintf( TmpChar , "%d    ", (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;
    FchTerCon.write((const uint8_t*)TmpChar,strlen(TmpChar)) ;
    // latitude
    sprintf( TmpChar , "%.5f    ", g_GlobalVar.m_TerrainPosCur.m_Lat ) ;
    FchTerCon.write((const uint8_t*)TmpChar,strlen(TmpChar)) ;
    // longitude
    sprintf( TmpChar , "%.5f\n", g_GlobalVar.m_TerrainPosCur.m_Lon ) ;
    FchTerCon.write((const uint8_t*)TmpChar,strlen(TmpChar)) ;
    // fermeture fichier
    FchTerCon.close() ;
    // affichage
    display.firstPage();
    display.setFont(&FreeMonoBold12pt7b);
    do
        {
        display.setCursor(0,70);
        display.print( "Enregistrement point gps" );
        }
    while (display.nextPage());
    return ECRAN_5_TmaDessous ;
    }

// affichage zone et coordonnees
display.firstPage();
display.setFont(&FreeMonoBold12pt7b);
do
    {
    // nom zone
    display.setCursor(0,20);
    display.print( "Tma Dessus:\n" );
    display.println( NomZone.c_str() );
    display.print( "lat: " ) ;
    display.println( g_GlobalVar.m_TerrainPosCur.m_Lat , 5 ) ;
    display.print( "lon:  " ) ;
    display.println( g_GlobalVar.m_TerrainPosCur.m_Lon , 5 ) ;
    display.print( "alt:" ) ;
    display.println( g_GlobalVar.m_TerrainPosCur.m_AltiBaro , 0 ) ;
    }
while (display.nextPage());

return ECRAN_5_TmaDessous ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de l'ecran rando-vol dans ses etats successifs
void CScreen154::EcranRandoVol()
{
// si inuit rando
if ( g_GlobalVar.m_EtatRando == CRandoVol::InitRando )
    {
    // beep
    CGlobalVar::beeper( 6000 , 300 ) ;

    // raz ecran
    display.setFullWindow() ;
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(20, 75);
    display.firstPage();
    do
        {
        // message
        display.print("Acquisition\n      Gps");
        }
    while (display.nextPage());

    #ifdef DEBUG_RANDO_VOl
    // bouton droit forcage à vichy
    if ( BoutonDroit() )
        {
        // force gps ok
        g_GlobalVar.ForceGpsOk() ;
        // vichy
        g_GlobalVar.m_AltiGps = 254 ;
        g_GlobalVar.m_TerrainPosCur.m_AltiBaro = 254 ;
        g_GlobalVar.m_TerrainPosCur.m_Lat = 46.122511 ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 3.4050107 ; // */
        // charroux
        /* g_GlobalVar.m_TerrainPosCur.m_Lat = 46.19453 ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 3.14594 ; // */
        // corent
        g_GlobalVar.m_TerrainPosCur.m_Lat = 45.64608830455222 ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 3.1817007064819336 ; // */
        // affichage menu
        g_GlobalVar.m_EtatRando = CRandoVol::InitAfficheMenu ;
        }
    #endif
    display.setPartialWindow( 0, 0, 200 , 200 );
    // si gps ok
    if ( g_GlobalVar.IsGpsOk() )
        g_GlobalVar.m_EtatRando = CRandoVol::InitAfficheMenu ;
    return ;
    }

// si initialisation menu
if ( g_GlobalVar.m_EtatRando == CRandoVol::InitAfficheMenu )
    {
    display.setCursor(5, 75);
    display.firstPage();
    do
        {
        // message
        display.print("Lecture *.gpx");
        }
    while (display.nextPage());
    g_GlobalVar.LireFichiersGpx() ;
    g_GlobalVar.m_EtatRando = CRandoVol::AfficheMenu ;
    return ;
    }

// si affichage menu
static int NbMenu = 0 ;
static int selection = 0 ;
if ( NbMenu++ < 8 && g_GlobalVar.m_EtatRando == CRandoVol::AfficheMenu )
    {
    // defilement du menu
    if ( g_GlobalVar.BoutonDroit() && selection < (g_GlobalVar.m_VecGpx.size()-1) )
        {
        NbMenu = 0 ;
        selection++ ;
        }
    if ( g_GlobalVar.BoutonGauche() && selection > 0 )
        {
        NbMenu = 0 ;
        selection-- ;
        }
    if ( g_GlobalVar.BoutonCentre() )
        NbMenu = 10 ;

    // affichage nom de fichier
    display.setCursor(0,20);
    display.setFont(&FreeMonoBold9pt7b);
    display.firstPage();
    do
        {
        char TmpChar[50] ;
        // nom trace des traces proches

        for ( int it = 0 ; it < 10 ; it++ )
            {
            if ( it == selection )
                sprintf(TmpChar,">%s",g_GlobalVar.GetTrackName(it)) ;
            else
                sprintf(TmpChar," %s",g_GlobalVar.GetTrackName(it)) ;
            display.println( TmpChar );
            }
        }
    while (display.nextPage());
    return ;
    }
else
    g_GlobalVar.m_EtatRando = CRandoVol::Navigation ;

// vecteur de la trace la plus proche
if ( selection < 0 || selection >= g_GlobalVar.m_VecGpx.size() )
    return ;

// relecture fichier selectionné
CFileGpx * pFileGpx = g_GlobalVar.m_VecGpx[selection] ;
pFileGpx->LireFichier() ;

const std::vector<CFileGpx::StPoint> & VecPts = pFileGpx->m_VecTrack ;

// si navigation
static float Slope = pFileGpx->m_SlopeMax ;
int EchelleMetre = Slope*MilesParDegres*100*UnMileEnMetres ;

// affichage de la carte gpx
display.firstPage();
do
    {
    // on efface
    //display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

    // dessin de la trace
    for ( int ip = 1 ; ip < VecPts.size() ; ip++ )
        {
        CFileGpx::StPoint PtsDeb ;
        CFileGpx::StPoint PtsFin ;
        PtsDeb.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat - VecPts[ip-1].m_Lat ;
        PtsDeb.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon - VecPts[ip-1].m_Lon;
        PtsFin.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat - VecPts[ip].m_Lat ;
        PtsFin.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon - VecPts[ip].m_Lon ;

        PtsDeb.m_Lat /=  Slope ;
        PtsDeb.m_Lon /= -Slope ;
        PtsFin.m_Lat /=  Slope ;
        PtsFin.m_Lon /= -Slope ;

        PtsDeb.m_Lat += 100 ;
        PtsDeb.m_Lon += 100 ;
        PtsFin.m_Lat += 100 ;
        PtsFin.m_Lon += 100 ;

        Serial.println( PtsFin.m_Lon ) ;

        display.drawLine( PtsDeb.m_Lon , PtsDeb.m_Lat , PtsFin.m_Lon , PtsFin.m_Lat , GxEPD_BLACK ) ;

        if ( ip == 1 )
            display.drawCircle( PtsDeb.m_Lon , PtsDeb.m_Lat , 3 , GxEPD_BLACK ) ;
        }

    // dessin du cap magnetique nord
    int xnm = -50 * cosf( g_GlobalVar.m_Mpu9250.m_CapMagnetique * PI / 180. - PI/2. ) + 100 ;
    int ynm =  50 * sinf( g_GlobalVar.m_Mpu9250.m_CapMagnetique * PI / 180. - PI/2. ) + 100 ;

    display.drawLine( 100 , 100 , xnm , ynm , GxEPD_BLACK ) ;
    display.drawCircle( 100 , 100 , 3 , GxEPD_BLACK ) ;

    // dessin du cap gps
    int xng = -30 * cosf( -g_GlobalVar.m_CapGpsDeg * PI / 180. + PI + PI/2. ) + 100 ;
    int yng =  30 * sinf( -g_GlobalVar.m_CapGpsDeg * PI / 180. + PI + PI/2. ) + 100 ;

    display.drawLine( 100 , 100 , xng , yng , GxEPD_BLACK ) ;

    // nom de la trace
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0,10);
    display.print( pFileGpx->m_TrackName.c_str() ) ;
    // zoom + zoom-
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(0,190);
    display.print( "z-" ) ;
    display.setCursor(170,190);
    display.print( "z+" ) ;
    // echelle
    display.setCursor(60,190);
    display.print( EchelleMetre ) ;
    display.print( "m2" ) ;
    }
while (display.nextPage());

// modification echelle
if ( g_GlobalVar.BoutonDroit() )
    Slope /= 2. ;
if ( g_GlobalVar.BoutonGauche() )
    Slope *= 2. ;
}

#endif
