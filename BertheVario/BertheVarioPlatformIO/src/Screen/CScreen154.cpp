////////////////////////////////////////////////////////////////////////////////
/// \file CScreen154.cpp
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 09/07/2025 modification de beep vario sol
/// \date 21/10/2025 : nombre de fix altibaro, debut de vol
/// \date 26/10/2025 : HDop en page sys
///

#include "../BertheVario.h"

#define ENABLE_GxEPD2_GFX 1

//#if ( TYPE_SCREEN == 154 )

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
#include "../../../../freefont/Gothic6.h"
#include "../../../../freefont/Gothic7.h"
#include "../../../../freefont/Gothic8.h"
#include "../../../../freefont/Futura6.h"
#include "../../../../freefont/Futura7.h"
#include "../../../../freefont/Futura8.h"

// note 16.11.2019: the compiler may exclude code based on constant if statements (display.epd2.panel == constant),
//                  therefore bitmaps may get optimized out by the linker

#ifdef GNU_VARIO_GRIS
 // select the display constructor line in one of the following files (old style):
 #include "Screen/GxEPD2_display_selection.h"
#else
 //#include "Screen/GxEPD2_display_selection_added.h"
 // or select the display class and display driver class in the following file (new style):
 //#include "Screen/GxEPD2_display_selection_new_style.h"
 #include "Screen/GxEPD2_display_selection_added.h"
//#endif

//#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
//SPIClass hspi(HSPI);
//#endif

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialise l'ecran.
void CScreen154::InitScreen()
{
// init ecran
//display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
//display.init(0,false,15,true) ;
display.init(0,false,50,true) ;

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
display.display(true);*/

// mise hors tension ecran
//display.hibernate();
//display.powerOff();
display.setRotation(0) ;
//display.setFullWindow() ;

// raz page precedente
//display.fillRect(0,0, 200, 200, GxEPD_BLACK ); // x y w h
//display.display(false) ;
//display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
//display.display(false) ;
//delay(SCREEN_DELAY) ;

display.setPartialWindow( 0, 0, 200 , 200 );

//ScreenRaz() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Raz de l'ecran qui fonctionne bien.
void CScreen154::ScreenRaz()
{
g_GlobalVar.RazBoutons() ;

if ( g_GlobalVar.m_FinDeVol.IsInFlight() )
    return ;

display.startWrite() ;
for (int x=0 ; x<200 ; x++ )
    for (int y=0 ; y<200 ; y++ )
        display.writePixel( x , y , GxEPD_WHITE ) ;
display.endWrite() ;
display.display(false) ;
/*
display.startWrite() ;
display.setCursor( 0 , 0 ) ;
display.print( "" );
display.fillRect(0,0, 200, 200, GxEPD_BLACK ); // x y w h
display.display(false) ;
display.setCursor( 0 , 0 ) ;
display.print( "" );
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.display(false) ;
display.endWrite() ;
*/


g_GlobalVar.m_MillisEcran0 = millis() ;

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
//display.setFullWindow() ;

//display.clearScreen() ;

//display.fillRect(0,0, 200, 200, GxEPD_BLACK ); // x y w h
//display.display(false);
//delay( SCREEN_DELAY ) ;
//display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    {
    //display.fillScreen(GxEPD_BLACK);
    //display.fillScreen(GxEPD_WHITE);
    }
//display.display(false);

// mise hors tension ecran
display.powerOff();
//display.end();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de "Connect to Wifi".
void CScreen154::AfficheConnectWifi()
{
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

// connect to wifi
display.setFont(&FreeMonoBold12pt7b);
display.setCursor(10, 100);
display.print("Connecting to\n  Wifi");

display.display(true);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de la calibration magnetique.
void CScreen154::AfficheCalibreMag()
{
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

// voltage
display.setFont(&FreeMonoBold12pt7b);
display.setCursor(0, 75);
display.print("Calibre magnitude (8)");

display.display(true);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de l'adresse ip.
void CScreen154::AfficheWifi(char * IpAdress)
{
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

// ip adresse
display.setFont(&FreeMonoBold12pt7b);
display.setCursor(0, 75);
display.print("http://");
display.print(IpAdress);
display.print(":8080");

display.display(true);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche le voltage batterie.
void CScreen154::AfficheVoltage()
{
float Voltage = g_GlobalVar.GetVoltage() ;

// affichage
char TmpChar[10] ;
sprintf( TmpChar , "%1.2fv", Voltage ) ;

display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

// voltage
display.setFont(&FreeMonoBold24pt7b);
display.setCursor(30, 60);
display.print(TmpChar);

// pourcentage
float Pourcent = (Voltage-3.7)/(4.3-3.7) * 100. ;
sprintf( TmpChar , "%3d%c", (int)Pourcent, '%' ) ;
display.setCursor(45, 110);
display.print(TmpChar);

// vserion
display.setFont(&FreeMonoBold18pt7b);
display.setCursor(5, 160);
display.print(NumVer);

display.display(true);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les boutons de calibration/wifi/rando.
void CScreen154::AfficheBoutons()
{
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

display.setFont(&FreeMonoBold24pt7b);
display.setCursor(10, 40);
display.print("Berthe\n Vario");

// boutons
display.setFont(&FreeMonoBold18pt7b);
display.setCursor(0, 130);
display.println("        C");
display.println("B       B");
display.print("C   W   R");

display.display(true);
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
display.fillRect(0, 0, 200, 200, GxEPD_WHITE ); // x y w h
display.setFont(&FreeMonoBold12pt7b);
display.setCursor(x, y);
display.print( pChar );
display.display(true) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 0.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranVz()
{
//CLocTermic LocTermic ;

#ifdef _LG_DEBUG_
    Serial.println("Affichage de screen");
#endif

if ( IsPageChanged() )
    {
    g_GlobalVar.RazBoutons() ;
    }

// vitesses verticale
float VitVert = g_GlobalVar.m_VitVertMS ;
bool SigneNeg = VitVert < 0. ;

// VZ ou VZ/finesse sol
char TmpCharVz[15] ;
if ( SigneNeg )
    sprintf( TmpCharVz , "%2.1f-" , VitVert ) ;
else
    sprintf( TmpCharVz , " %2.1f " , VitVert ) ;

// altitude barometrique
char TmpCharAlt[15] ;
sprintf( TmpCharAlt , "%4d", (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ) ;

// derive
const float DeriveMilieu = 11. ;
float DeriveAngle = g_GlobalVar.GetDeriveDeg() ;
char TmpCharAngleDerive[15] ;
if ( fabsf(DeriveAngle) >= 90. )
    {
    sprintf( TmpCharAngleDerive , " \\R/" ) ;
    // alarme sonore
    if ( g_GlobalVar.m_Config.m_alarme_reculade && g_GlobalVar.m_FinDeVol.IsInFlight() )
        {
        CGlobalVar::beeper( 7000 , 150 ) ;
        CGlobalVar::beeper( SOUND_DELAY_ONLY , 100 ) ;
        CGlobalVar::beeper( 6000 , 150 ) ;
        }
    }
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
sprintf( TmpCharHauteurSol , "%4d", (int)(g_GlobalVar.GetHauteurSol()) ) ;

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
    sprintf( TmpCharDV , "%3d", ((int)g_GlobalVar.m_DureeVolMin) ) ;

// cap
int Cap = g_GlobalVar.m_CapGpsDeg ;
char TmpCharCap[15] ;
char TmpCharNomCap[] = "  " ;
GetCapChar( Cap , TmpCharNomCap ) ;
sprintf( TmpCharCap , "%2d", (int)(Cap/10) ) ;

// nom/finesse du site le plus proche
float FinesseTerrainMinimum = 99. ;
const CLocTerrain * pTerrain = g_GlobalVar.m_TerrainArr.GetTerrainProche( FinesseTerrainMinimum ) ;
char TmpCharNomSite[15] = "----------" ;
if ( pTerrain != NULL )
    {
    int inspp = 0 ;
    // recopie nom de terrain
    for ( ; inspp < 10 && inspp < pTerrain->m_Nom.GetLength() ; inspp++ )
        TmpCharNomSite[inspp] = pTerrain->m_Nom[inspp] ;
    TmpCharNomSite[inspp] = 0 ;
    }
else
    FinesseTerrainMinimum = 99 ;
char TmpCharFinesseSite[5] ;
sprintf( TmpCharFinesseSite , "%2d" , (int)FinesseTerrainMinimum ) ;

// zone aerienne
static int AffTerrainFront = 0 ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.PrendreMutex() ;
 CLGString NomZoneDessous = g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous ;
 int DansUneZone          = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone ;
 //CLGString NomZoneLimite  = g_GlobalVar.m_ZonesAerAll.m_NomZoneEnLimite ;
 //int LimiteZone             = g_GlobalVar.m_ZonesAerAll.m_LimiteZone ;
 //int DistFront      = g_GlobalVar.m_ZonesAerAll.m_DistXYNextZone ;
 //int AltFront       = g_GlobalVar.m_ZonesAerAll.m_DistAltCurZone ;
 //int CapFrontDeg    = g_GlobalVar.m_ZonesAerAll.m_CapFrontProche ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;

// raz page precedente
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

// affichage valeur de VZ
//int16_t tbx, tby;
//uint16_t tbw, tbh;

///////////////////////////////////////////
// nom de zone aerienne ou termique/terrain
if ( g_GlobalVar.m_Hgt2Agl.m_ErreurFichier )
    {
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(0, 15);
    display.print("*** ERREUR *** * FICHIER HGT*");
    //if ( g_GlobalVar.m_BeepVario )
    //    CGlobalVar::BeepError(true) ;
    }
else if ( DansUneZone == ZONE_DEDANS )
    {
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(0, 15);
    display.print((const char*)NomZoneDessous);
    if ( g_GlobalVar.m_BeepVario )
        {
        CGlobalVar::beeper( 6000 , 50 ) ;
        CGlobalVar::beeper( SOUND_DELAY_ONLY , 100 ) ;
        CGlobalVar::beeper( 6000 , 50 ) ;
        CGlobalVar::beeper( SOUND_DELAY_ONLY , 100 ) ;
        CGlobalVar::beeper( 4000 , 50 ) ;
        }
    }
/*else if ( LimiteZone == ZONE_LIMITE_ALTI )
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
    } */
else
    {
    /////////////
    // bandeaux 1
    // terrain finesse
    const int y1 = 30 ;
    //if ( (AffTerrainFront++/3)%2 )
        {
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(0, y1);
        display.print(TmpCharNomSite);
        display.setFont(&FreeMonoBold18pt7b);
        display.setCursor(155, y1);
        display.print(TmpCharFinesseSite);
        }
    // dist/alt/cap frontiere zone
    /*else
        {
        display.setCursor(2, y1);
        char TmpCharFront[25] ;
        char TmpCharCap[25] ;
        GetCapChar( CapFrontDeg , TmpCharCap ) ;
        // altitude frontiere
        if ( AltFront > 500 )
            {
            display.setFont(&FreeMonoBold18pt7b);
            //            "500"
            display.print("   ");
            display.setFont(&FreeMonoBold12pt7b);
            //            "A "
            display.print("  ");
            }
        else
            {
            display.setFont(&FreeMonoBold18pt7b);
            sprintf( TmpCharFront , "%3d", AltFront ) ;
            display.print(TmpCharFront);
            display.setFont(&FreeMonoBold12pt7b);
            display.print("A ");
            }
        // distance frontiere < DISTANCE_PROCHE_XY
        if ( DistFront > 999 )
            {
            display.setFont(&FreeMonoBold18pt7b);
            //            "999SE"
            display.print("     ");
            }
        else
            {
            display.setFont(&FreeMonoBold18pt7b);
            sprintf( TmpCharFront , "%3d%s", DistFront , TmpCharCap ) ;
            display.print(TmpCharFront);
            }
        }*/
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
// VZ
const uint16_t xv = 30 ;
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

bool EnVol = g_GlobalVar.m_FinDeVol.IsInFlight() ;

// si changement d'ecran
if ( BoutonCentre() && !EnVol )
    return ECRAN_1_Histo ;

// si activation / desactivation beep attente Gps / Vitesse
if ( /*(g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
      g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
      g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS ) &&*/
      BoutonGauche() )
    g_GlobalVar.m_BeepVario = ! g_GlobalVar.m_BeepVario ;

// bouton long gauche masque bouton court, attention
if ( BoutonGaucheLong() && !EnVol )
    return ECRAN_6_Sys ;

return ECRAN_0_Vz ;

//display.hibernate();
//display.powerOff();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 1. Historique de vol.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranHisto()
{
char * pCharNomFch = NULL ;
static int ivol = 0 ;
int y = 20 ;

// lecture des histo
g_GlobalVar.m_HistoVol.LectureFichiers() ;

if ( IsPageChanged() )
    {
    g_GlobalVar.RazBoutons() ;
    ivol = g_GlobalVar.m_HistoVol.m_HistoDir.GetSize() - 1 ;
    if ( ivol < 0 )
        ivol = 0 ;
    }

// si pas de fichiers histo
if ( g_GlobalVar.m_HistoVol.m_HistoDir.GetSize() == 0 )
    {
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.setFont(&FreeMonoBold12pt7b);
    // message
    display.setCursor(0, 20);
    display.print("0 histo");
    display.display(true);

    goto fin_histo ;
    }

// typr de debut de vol
int TypeDebutVol ;
TypeDebutVol = g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_DebutDeVol ;
char CharTypeDebutVol[20] ;
if ( TypeDebutVol == DEBUT_VOL_BOUTON )
    strcpy( CharTypeDebutVol , "bo" ) ;
else if ( TypeDebutVol == DEBUT_VOL_VIT_HOR )
    strcpy( CharTypeDebutVol , "vh" ) ;
else if ( TypeDebutVol == DEBUT_VOL_VIT_VERT )
    strcpy( CharTypeDebutVol , "vz" ) ;
else if ( TypeDebutVol == DEBUT_VOL_HAUTEUR_SOL )
    strcpy( CharTypeDebutVol , "hs" ) ;
else
    strcpy( CharTypeDebutVol , "in" ) ;

// nom fichier
char NomFchIgc[20] ;
strcpy( NomFchIgc , g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_NomIgc ) ;
pCharNomFch = strtok( NomFchIgc , "/." ) ;
char TmpCharAffVol[40] ;
sprintf( TmpCharAffVol , "%d/%d %s%s", ivol+1 , g_GlobalVar.m_HistoVol.m_HistoDir.GetSize() , pCharNomFch , CharTypeDebutVol ) ;

char TmpCharAltiDeco[20] ;
sprintf( TmpCharAltiDeco , "%4dm", (int)g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_ZDeco ) ;

char TmpCharAltiMax[20] ;
sprintf( TmpCharAltiMax , "%4dm", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_ZMax ) ;

char TmpCharAltiMin[20] ;
sprintf( TmpCharAltiMin , "%4dm", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_ZMin ) ;

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
sprintf( TmpCharTV , "% 4.1f'", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_TempsDeVol ) ;

display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.setFont(&FreeMonoBold12pt7b);
// nom fch igc
display.setCursor(0, y);
display.print(TmpCharAffVol);

// alti decollage
y += 25 ;
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

// alti min
y += 20 ;
display.setCursor(0, y);
display.print("Z min :");
display.setCursor(110, y);
display.print(TmpCharAltiMin);

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
display.display(true);

// fin de la fonction
fin_histo :

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    {
    g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
    g_GlobalVar.m_HistoVol.m_HistoDir.FreeExtra() ;
    return ECRAN_0_Vz ;
    }

if ( BoutonGaucheLong() )
    {
    g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
    g_GlobalVar.m_HistoVol.m_HistoDir.FreeExtra() ;
    return ECRAN_0_Vz ;
    }

// si changement de numero histo vol
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    ivol++ ;
    if ( ivol >= g_GlobalVar.m_HistoVol.m_HistoDir.GetSize() )
        ivol = g_GlobalVar.m_HistoVol.m_HistoDir.GetSize() - 1 ;
    g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
    return ECRAN_1_Histo ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    m_MillisEcran0 = millis() ;
    ivol-- ;
    if ( ivol < 0 )
        ivol = 0 ;
    g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
    return ECRAN_1_Histo ;
    }

// si changement d'ecran
if ( BoutonCentre() )
    {
    g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
    return ECRAN_2a_ListeIgc ;
    }

if ( BoutonGaucheLong() )
    {
    g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
    g_GlobalVar.m_HistoVol.m_HistoDir.FreeExtra() ;
    return ECRAN_0_Vz ;
    }

g_GlobalVar.m_HistoVol.m_HistoDir.SetSize(0) ;
return ECRAN_1_Histo ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche/modifie les [variables] du fichier de
/// configuration.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranCfgFch()
{
static char TmpMod[100] = {0} ;
CLGString Name ;
CLGString Value ; ;

static bool Modif = false ;
static int iChamps = -1 ;

// si nouvelle page
if ( IsPageChanged() )
    {
    g_GlobalVar.RazBoutons() ;
    iChamps = -1 ;
    }


// sortie
if ( iChamps == -1 )
    {
    strcpy( TmpMod , "" ) ;
    Name = " Editeur Cfg" ;
    g_GlobalVar.m_Config.EcritureFichier() ;
    g_GlobalVar.m_Config.ConstructVect() ;
    }
else
    {
    g_GlobalVar.m_Config.GetChar( iChamps , Name , Value ) ;
    m_MillisEcran0 = millis() ;
    }

display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.setFont(&FreeMonoBold12pt7b);
// Mod
display.setCursor(0, 20);
display.print(TmpMod) ;
// NomVar
display.setCursor(0, 60);
display.print(Name) ;
// ValNomVar
display.setCursor(0, 140);
display.print(Value) ;
display.display(true);

bool BoutonCent = BoutonCentre() ;
bool BoutonGau  = BoutonGauche() ;
bool BoutonDroi = BoutonDroit() ;
// sortie ecran
if ( BoutonCent && !Modif && iChamps == -1 )
    {
    iChamps = -1 ;
    return ECRAN_5_TmaDessous ;
    }

// modification variable
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

// defilement -
if ( BoutonGau && !Modif )
    {
    if ( iChamps > -1 )
        iChamps-- ;
    else
        iChamps = 0 ;
    }
// defilement +
if ( BoutonDroi && !Modif  )
    {
    int size = (g_GlobalVar.m_Config.m_LinesVect.GetSize()-1) ;
    if ( iChamps < size )
        iChamps++ ;
    }

// si time out ecran en premiere page
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( ((Temps/1000) > m_SecRetourEcran0) && (iChamps == -1) )
    {
    g_GlobalVar.m_Config.EcritureFichier() ;
    iChamps = -1 ;
    return ECRAN_0_Vz ;
    }

// fin variables
if ( BoutonDroitLong() && !Modif )
    iChamps = g_GlobalVar.m_Config.m_LinesVect.GetSize()-1 ;

// debut variable
bool GaucheLong = BoutonGaucheLong() ;
if ( GaucheLong && !Modif && iChamps >= 0)
    iChamps = 0 ;
// retour page
if ( GaucheLong && !Modif && iChamps < 0 )
    {
    iChamps = -1 ;
    return ECRAN_3a_TmaAll ;
    }

return ECRAN_4_CfgFch ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 2b, TAM/CTR
/// modification activation.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranTmaMod()
{
static int NumTmaCtr = 0 ;

if ( IsPageChanged() )
    {
    g_GlobalVar.RazBoutons() ;
    }

// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

// construction du tableau des zones
CLGArrayPtr<CZoneAer> VecAffZones ;
VecAffZones.SetDeleteObjet(false) ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetSize() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetData() ;

// construction vecteur des zones a afficher dans fichier activation
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    if ( pZoneArr[iz]->m_DansFchActivation )
        VecAffZones.Add( pZoneArr[iz] ) ;
    }
// ajout des zones a afficher pas dans le fichier activation
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    if ( !pZoneArr[iz]->m_DansFchActivation )
        VecAffZones.Add( pZoneArr[iz] ) ;
    }

// zones de meme nom que selectionnee
CLGArrayPtr<CZoneAer> VecZone2Mod ;
VecZone2Mod.SetDeleteObjet(false) ;
if ( NumTmaCtr >= 0 && NumTmaCtr < VecAffZones.GetSize() )
    {
    CZoneAer * pZone = VecAffZones[NumTmaCtr] ;
    for ( int iz = 0 ; iz < VecAffZones.GetSize() ; iz++ )
        if ( ! strcmp( VecAffZones[iz]->m_pNomAff , pZone->m_pNomAff ) )
            VecZone2Mod.Add( VecAffZones[iz] ) ;
    }

display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.setFont(&FreeMonoBold12pt7b);
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
    display.print( pZone->GetHauteurSolZoneProtege() );
    }

display.display(true);


// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
bool BCentre = BoutonCentre() ;
if ( BCentre && VecZone2Mod.GetSize() == 0 )
    return ECRAN_3a_TmaAll ;

// si modification activation
if ( BCentre && VecZone2Mod.GetSize() != 0 )
    {
    m_MillisEcran0 = millis() ;
    for ( int iz = 0 ; iz < VecZone2Mod.GetSize() ; iz++ )
        {
        CZoneAer * pZone = VecZone2Mod[iz] ;
        if ( pZone->m_DansFchActivation )
            {
            g_GlobalVar.m_ZonesAerAll.GestActivationZone( pZone , !pZone->m_Activee ) ;
            // compte rendu ecran
            //display.setFont(&FreeMonoBold9pt7b);
            display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
            display.setCursor( 0 , 140 ) ;
            display.println( "zone" ) ;
            if ( pZone->m_Activee )
                display.print( " activee" ) ;
            else
                display.print( " de-activee" ) ;
            display.display(true) ;
            // ecriture fichier zone
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
    if ( NumTmaCtr < 0 )
        {
        NumTmaCtr = VecAffZones.GetSize()-1 ;
        return ECRAN_3a_TmaAll ;
        }
    }

// incrementation numero de zone
if ( BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    int Size = VecAffZones.GetSize()-1 ;
    NumTmaCtr++ ;
    if ( NumTmaCtr > Size )
        NumTmaCtr = 0 ;
    }

if ( BoutonGaucheLong() )
    NumTmaCtr = 0 ;

if ( BoutonDroitLong() )
    NumTmaCtr = VecAffZones.GetSize()-1 ;

return ECRAN_3b_TmaMod ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran 2a, TAM/CTR titre
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranTmaAll()
{
// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

if ( IsPageChanged() )
    {
    g_GlobalVar.RazBoutons() ;
    }

// construction du tableau des zones activable en premier
CLGArrayPtr<CZoneAer> VecZonesMod ;
VecZonesMod.SetDeleteObjet(false) ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetSize() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetData() ;

// ajout vecteur des zones activables
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;
    if ( pZone->m_DansFchActivation )
        VecZonesMod.Add( pZone ) ;
    }
/* // ajout vecteur des zones non activables
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;
    if ( !pZone->m_DansFchActivation )
        VecZonesMod.Add( pZone ) ;
    }*/

// titre
char TmpTitre[30] ;
sprintf( TmpTitre , "%2d B. Cen. Mo." , g_GlobalVar.m_ZonesAerAll.GetSize() ) ;

display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
// titre
display.setFont(&FreeMonoBold12pt7b);
display.setCursor( 0, 20 );
display.print( TmpTitre ) ;

// zones active
int xcol = 0 ;
int yligne = 12 ;
//display.setFont(&FreeMonoBold12pt7b);
display.setFont(&FUTURAM8pt7b) ;
int izaff = 0 ;
for ( int iz = 0 ; iz < VecZonesMod.GetSize() ; iz++ )
    {
    if ( !VecZonesMod[iz]->m_DansFchActivation )
        continue ;
    display.setCursor(0+xcol, 30 + yligne );

    if ( VecZonesMod[iz]->m_Activee )
        {
        //display.print( "<" ) ;
        display.print( VecZonesMod[iz]->m_pNomAff ) ;
        }
    else
        continue ;
        /*{
        char TmpChar[50] ;
        sprintf( TmpChar , "-%s" ,  VecZonesMod[iz]->m_pNomAff ) ;
        TmpChar[9] = 0 ;
        display.print( TmpChar  ) ;
        }*/
    yligne += 17 ;
    izaff++ ;
    if ( izaff == 10 )
        {
        xcol = 110 ;
        yligne = 12 ;
        }
    }
display.display(true);

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

if ( BoutonGaucheLong() )
    return ECRAN_2a_ListeIgc ;

return ECRAN_3a_TmaAll ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations des fichiers IGC de la carte
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranListeIgcFch()
{
CLGArray<CLGString> VecNomIgc ;
CLGArray<float> VecTempsIgc ;

if ( IsPageChanged() )
    {
    g_GlobalVar.RazBoutons() ;
    }

// lecture de fichier
g_GlobalVar.ListeIgc(VecNomIgc,VecTempsIgc) ;

// calcul du toatal des temps
float TotalMin = 0 ;
int y_cursor ;
for ( int ifch = 0 ; ifch < VecNomIgc.GetSize() ; ifch++ )
    {
    float temps = VecTempsIgc[ifch] ;
    if ( temps >= 0.5 )
        TotalMin += temps ;
    }

char TmpChar[25] ;
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.setFont(&FreeMonoBold9pt7b);

// total igc
y_cursor = 15 ;
sprintf( TmpChar , "total igc:%03dm", ((int)TotalMin) ) ;
display.setCursor( 0, y_cursor );
display.print( TmpChar ) ;

int ivec = 0 ;
for ( ; ivec < VecNomIgc.GetSize() ; ivec++ )
    {
    sprintf( TmpChar , "%s %03d", (const char*)VecNomIgc[ivec] , ((int)VecTempsIgc[ivec]) ) ;
    y_cursor += 16 ;
    display.setCursor( 0, y_cursor );
    display.print( TmpChar ) ;
    }

display.display(true);

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

if ( BoutonGaucheLong() )
    return ECRAN_1_Histo ;

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

display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
// titre
display.setFont(&FreeMonoBold12pt7b);
display.setCursor( 0, 20 );
display.print( TmpChar ) ;
display.display(true);

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
if ( BoutonCentre() )
    {
    display.setCursor(0,70);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.print( "Annulation" );
    display.display(true);
    return ECRAN_2a_ListeIgc ;
    }

// si changement d'ecran
if ( BoutonGauche() )
    {
    display.setCursor(0,70);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.print( "Archivage" );
    display.display(true);
    m_MillisEcran0 = millis() ;
    g_GlobalVar.ArchiveIgc() ;
    return ECRAN_2a_ListeIgc ;
    }

// si confirme delete igc
if ( BoutonDroit() )
    {
    display.setCursor(0,70);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.print( "Archivage" );
    display.display(true);
    m_MillisEcran0 = millis() ;
    g_GlobalVar.ArchiveIgc() ;
    return ECRAN_2a_ListeIgc ;
    }

//if ( BoutonGaucheLong() )
//    return ECRAN_1_Histo ;

return ECRAN_2b_ConfirmArchIgc ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction permet de connaitre la TMA dessus.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranTmaDessous()
{
static int NbAppButGD = 0 ;
CLGString NomZone = "" ;

if ( IsPageChanged() )
    {
    g_GlobalVar.RazBoutons() ;
    NbAppButGD = 0 ;
    }

// zone au dessus
g_GlobalVar.m_ZonesAerAll.m_Mutex.PrendreMutex() ;
 if ( g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone )
    NomZone = g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    {
    NbAppButGD = 0 ;
    return ECRAN_0_Vz ;
    }

// si changement d'ecran ou non enregistrement point
if ( BoutonCentre() )
    {
    if ( NbAppButGD == 0 )
        return ECRAN_6_Sys ;
    display.setCursor(0,70);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.print( "Annulation" );
    display.display(true);
    m_MillisEcran0 = millis() ;
    NbAppButGD=0 ;
    }

// pour memorisation point
if ( BoutonGauche() || BoutonDroit() )
    {
    m_MillisEcran0 = millis() ;
    NbAppButGD++ ;
    }

if ( BoutonGaucheLong() )
    {
    NbAppButGD = 0 ;
    return ECRAN_4_CfgFch ;
    }

// demande confirmation enregistrement point
if ( NbAppButGD == 1 )
    {
    display.setFont(&FreeMonoBold12pt7b);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.setCursor(0,70);
    display.print( "Confirmation enregistrement point gps GD" );
    display.display(true);
    return ECRAN_5_TmaDessous ;
    }

// enregistrement point dans
if ( NbAppButGD >= 2  )
    {
    if ( g_GlobalVar.m_TerrainPosCur.m_Lat == 0. && g_GlobalVar.m_TerrainPosCur.m_Lon == 0. )
        {
        NbAppButGD = 0 ;
        display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
        display.setCursor(0,70);
        display.print( "Point null non enregistre" );
        display.display(true);
        return ECRAN_5_TmaDessous ;
        }
    char TmpChar[50] ;
    NbAppButGD = 0 ;
    File FchTerCon = SD.open(TERRAIN_FCH,FILE_APPEND);
    if ( !FchTerCon )
        return ECRAN_5_TmaDessous ;
    FchTerCon.seek(SeekEnd) ;
    // nom
    sprintf(TmpChar,"pt_%d%d%d    ",(int)g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec,
                                       (int)(10*g_GlobalVar.m_TerrainPosCur.m_Lat),
                                       (int)(10*g_GlobalVar.m_TerrainPosCur.m_Lon)) ;
    FchTerCon.write((const uint8_t*)TmpChar,strlen(TmpChar)) ;
    // altitude
    sprintf( TmpChar , "%d    ", (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ) ;
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
    display.setFont(&FreeMonoBold12pt7b);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.setCursor(0,70);
    display.print( "Enregistrement point gps" );
    display.display(true);
    return ECRAN_5_TmaDessous ;
    }

// affichage zone et coordonnees
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.setFont(&FreeMonoBold12pt7b);
// nom zone
display.setCursor(0,20);
display.print( "Tma Dessus:\n" );
display.println( NomZone );
display.print( "lat: " ) ;
display.println( g_GlobalVar.m_TerrainPosCur.m_Lat , 5 ) ;
display.print( "lon:  " ) ;
display.println( g_GlobalVar.m_TerrainPosCur.m_Lon , 5 ) ;
display.print( "alt:" ) ;
display.println( g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec , 0 ) ;
display.display(true);

return ECRAN_5_TmaDessous ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction affiche les informations de l'ecran sys, orienté systeme.
/// \return l'etat suivant de l'automate
CGestEcrans::EtatsAuto CScreen154::EcranSys()
{
// lancement de la tache perfmonitor
if ( IsPageChanged() )
    {
    LancerTachePerfMon() ;
    }

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
char TmpCharVB[30] ;
sprintf( TmpCharVB , "%1.2fv", g_GlobalVar.GetVoltage() ) ;

// cap magnetique
char TmpCharCM[30] ;
sprintf( TmpCharCM , "Cap m :   %3dd", (int)g_GlobalVar.m_Mpu9250.m_CapMagnetique ) ;

// hdop
char TmpCharHDop[30] ;
sprintf( TmpCharHDop , "HDop  :  %5.1f", g_GlobalVar.GetHDop() ) ;

// temperature
char TmpCharTemp[30] ;
sprintf( TmpCharTemp , "Temp  :  %4.1fd", g_GlobalVar.m_MS5611Press.GetTemperatureDegres() ) ;

// memoire
char TmpCharMem[30] ;

// pourcentage utilisation cpu
int cpu0 = g_GlobalVar.m_PercentCore0 ;
int cpu1 = g_GlobalVar.m_PercentCore1 ;

display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
display.setFont(&FreeMonoBold12pt7b);

// date et heure
display.setCursor(12, 15);
display.print(TmpCharDate) ;

// HDop
display.setCursor(0, 38);
display.print(TmpCharHDop);

// Cap Magnetique
display.setCursor(0, 58);
display.print(TmpCharCM);

// temperature
display.setCursor(0, 78);
display.print(TmpCharTemp);

// core 0 usage
display.setCursor(0, 98);
display.print("core 0:    ");
display.print(cpu0) ;
display.print("%");

// core 1 usage
display.setCursor(0,118);
display.print("core 1:    ");
display.print(cpu1) ;
display.print("%");

// memory
display.setCursor(0,138);
sprintf( TmpCharMem , "f mem :%6db", (int) esp_get_free_heap_size() ) ;
display.print(TmpCharMem);

// batterie
display.setCursor(0,158);
display.print("V bat :  ");
display.print(TmpCharVB);

// firmware
display.setCursor(0, 195);
display.print("fir:");
display.print(NumVer);

display.display(true) ;

// si time out ecran
unsigned long Temps = millis() - m_MillisEcran0 ;
if ( (Temps/1000) > m_SecRetourEcran0 )
    return ECRAN_0_Vz ;

// si changement d'ecran
if ( BoutonCentre() )
    return ECRAN_0_Vz ;

if ( BoutonGaucheLong() )
    return ECRAN_5_TmaDessous ;

return ECRAN_6_Sys ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de l'ecran rando-vol dans ses etats successifs
void CScreen154::EcranRandoVol()
{
// si inuit rando
if ( g_GlobalVar.m_EtatRando == CRandoVol::InitRando )
    {
    // beep
    CGlobalVar::beeper( 4000 , 50 ) ;

    // raz ecran
    display.setFullWindow() ;
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(20, 75);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    // message
    display.print("Acquisition\n      Gps");
    display.display(true);

    #ifdef DEBUG_RANDO_VOl
    // bouton droit forcage à vichy
    if ( BoutonDroit() )
        {
        // force gps ok
        g_GlobalVar.ForceGpsOk() ;
        // vichy
        g_GlobalVar.m_AltiGps = 254 ;
        g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = 254 ;
        g_GlobalVar.m_TerrainPosCur.m_Lat = 46.122511 ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 3.4050107 ; // */
        // charroux
        /* g_GlobalVar.m_TerrainPosCur.m_Lat = 46.19453 ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 3.14594 ; // */
        // corent
        /*g_GlobalVar.m_TerrainPosCur.m_Lat = 45.64608830455222 ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 3.1817007064819336 ; // */
        // paillaret
        /*g_GlobalVar.m_TerrainPosCur.m_Lat = 45.49812  ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 2.82271 ; // */
        // pdd
        /* g_GlobalVar.m_TerrainPosCur.m_Lat = 45.747387022763235 ;
        g_GlobalVar.m_TerrainPosCur.m_Lon = 2.972681522369385 ; // */
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
    // message
    display.setCursor(5, 75);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
    display.print("Lecture *.gpx");
    display.display(true);
    // lecture fichiers
    g_GlobalVar.LireFichiersGpx() ;
    g_GlobalVar.m_EtatRando = CRandoVol::AfficheMenu ;
    // pour vide le bouton droit
    g_GlobalVar.RazBoutons() ;
    return ;
    }

// si affichage menu
static CFileGpx * pFileGpx = NULL ;
static int NbMenuScreen = 0 ;
static int selection = 0 ;
static int glissant = 0 ;
const int NbAffLigneMenu = 10 ;
if ( NbMenuScreen++ < 8 && g_GlobalVar.m_EtatRando == CRandoVol::AfficheMenu )
    {
    // defilement du menu
    if ( g_GlobalVar.BoutonDroitTousAppui() && selection < (g_GlobalVar.m_VecGpx.GetSize()-1) )
        {
        NbMenuScreen = 0 ;
        selection++ ;
        if ( selection > NbAffLigneMenu/2 )
            glissant++ ;
        }
    if ( g_GlobalVar.BoutonGaucheTousAppui() && selection > 0 )
        {
        NbMenuScreen = 0 ;
        selection-- ;
        glissant-- ;
        if ( glissant <= 0 )
            glissant = 0 ;
        }
    if ( g_GlobalVar.BoutonCentre() )
        NbMenuScreen = 10 ;

    // affichage nom de piste
    display.setCursor(0,20);
    display.setFont(&FreeMonoBold9pt7b);
    display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h

    // nom trace des traces proches
    char TmpChar[50] ;
    for ( int it = 0 + glissant ; it < NbAffLigneMenu + glissant ; it++ )
        {
        if ( it == selection )
            sprintf(TmpChar,">%s",g_GlobalVar.GetTrackName(it)) ;
        else
            sprintf(TmpChar," %s",g_GlobalVar.GetTrackName(it)) ;
        display.println( TmpChar );
        }
    display.display(true);
    return ;
    }
// si fin affichage menu
else if ( g_GlobalVar.m_EtatRando == CRandoVol::AfficheMenu )
    {
    g_GlobalVar.m_EtatRando = CRandoVol::InitTrace ;
    return ;
    }

// si initialisation trace
if ( g_GlobalVar.m_EtatRando == CRandoVol::InitTrace )
    {
    g_GlobalVar.m_EtatRando = CRandoVol::Navigation ;

    // vecteur de la trace la plus proche
    if ( selection < 0 || selection >= g_GlobalVar.m_VecGpx.GetSize() )
        return ;

    // relecture fichier selectionné si pas terrain connu
    pFileGpx = g_GlobalVar.m_VecGpx[selection] ;
    if ( ! pFileGpx->m_PtTerConnu )
        pFileGpx->LireFichier(true) ;

    // destruction autres fichiers
    for ( int ifch = 0 ; ifch < g_GlobalVar.m_VecGpx.GetSize() ; ifch++ )
        if ( g_GlobalVar.m_VecGpx[ifch] != pFileGpx )
            delete g_GlobalVar.m_VecGpx[ifch] ;
    g_GlobalVar.m_VecGpx.SetSize(0) ;
    g_GlobalVar.m_VecGpx.FreeExtra() ;
    return ;
    }

const CLGArray<CFileGpx::StPoint> & VecPts = *(pFileGpx->m_pVecTrack) ;

// si navigation
static float Slope = pFileGpx->m_SlopeMax + 0.00001 ;
int EchelleMetre = Slope*MilesParDegres*100*UnMileEnMetres ;
if ( EchelleMetre < 2 )
    EchelleMetre = 2 ;
static int NbInfo = -1 ;

// demande page info
const int init_nb_info = 7 ;
bool BoutCentre = g_GlobalVar.BoutonCentre() ;
if ( BoutCentre )
    {
    if ( NbInfo < 0 )
        NbInfo = init_nb_info ;
    else
        NbInfo = -1 ;
    }
if ( NbInfo < -1 )
    NbInfo = -1 ;

// affichage page info
display.fillRect(0,0, 200, 200, GxEPD_WHITE ); // x y w h
if ( NbInfo-- >= 0 )
    {
    float AltitudeRest = 222 ;
    float DistanceRest = 1000 ;
    float AltitudeFait = 222 ;
    float DistanceFait = 1000 ;

    // definition point courant
    CHgt2Agl Hgt2Agl ;
    CFileGpx::StPoint PtCur ;
    PtCur.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
    PtCur.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;
    PtCur.m_Alt = Hgt2Agl.GetGroundZ( PtCur.m_Lon , PtCur.m_Lat ) ;

    // calcul info
    pFileGpx->GetInfo( PtCur , AltitudeRest , DistanceRest , AltitudeFait , DistanceFait ) ;

    // date
    char TmpCharDate[35] ;
    int secondes_date = g_GlobalVar.m_HeureSec ;
    sprintf( TmpCharDate ,"%04d%02d%02d-%02d:%02d" ,
            (int)(g_GlobalVar.m_Annee) ,
            g_GlobalVar.m_Mois ,
            g_GlobalVar.m_Jour ,
            (int) (secondes_date/3600) ,   // heure
            (int)((secondes_date/60)%60)   // minutes
            ) ;

    // altitude restante
    char TmpAltitudeRest[20] ;
    sprintf( TmpAltitudeRest , "Alt r:   %4.0fm", AltitudeRest) ;
    // distance restante
    char TmpDistanceRest[20] ;
    sprintf( TmpDistanceRest , "Dis r:  %5.0fm", DistanceRest) ;
    // altitude fait
    char TmpAltitudeFait[20] ;
    sprintf( TmpAltitudeFait , "Alt f:   %4.0fm", AltitudeFait) ;
    // distance restante
    char TmpDistanceFait[20] ;
    sprintf( TmpDistanceFait , "Dis f:  %5.0fm", DistanceFait) ;

    // temperature
    char TmpCharTemp[30] ;
    sprintf( TmpCharTemp , "Temp :   %4.1fd", g_GlobalVar.m_MS5611Press.GetTemperatureDegres() ) ;

    // v batterie
    char TmpCharVB[20] ;
    sprintf( TmpCharVB ,   "V bat:   %1.2fv", g_GlobalVar.GetVoltage() ) ;

    // memoire
    char TmpCharMem[35] ;
    sprintf( TmpCharMem ,  "f mem: %6db", (int) esp_get_free_heap_size() ) ;

    display.setFont(&FreeMonoBold12pt7b);

    // date et heure
    display.setCursor(0, 15);
    display.print(TmpCharDate) ;

    // alti restante
    display.setCursor(0, 55);
    display.print(TmpAltitudeRest);
    // distance restance
    display.setCursor(0, 75);
    display.print(TmpDistanceRest);
    // alti restante
    display.setCursor(0,105);
    display.print(TmpAltitudeFait);
    // distance restance
    display.setCursor(0,125);
    display.print(TmpDistanceFait);

    // temperature
    display.setCursor(0,155);
    display.print(TmpCharTemp);

    // memory
    display.setCursor(0,175);
    display.print(TmpCharMem);

    // batterie
    display.setCursor(0,195);
    display.print(TmpCharVB);
    }
// affichage de la carte gpx
else
    {
    float CapGpsRad = - g_GlobalVar.m_CapGpsDeg * PI / 180. ;

    // si pas d'orientation cap gps
    if ( !g_GlobalVar.m_OrientationCapGps )
        CapGpsRad = 0. ;

    // dessin de la trace
    for ( int ip = 1 ; ip < VecPts.GetSize() ; ip++ )
        {
        CFileGpx::StPoint PtsDeb ;
        CFileGpx::StPoint PtsFin ;
        PtsDeb.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat - VecPts[ip-1].m_Lat ;
        PtsDeb.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon - VecPts[ip-1].m_Lon;
        PtsFin.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat - VecPts[ip].m_Lat ;
        PtsFin.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon - VecPts[ip].m_Lon ;

        float y1 = PtsDeb.m_Lat /  Slope ;
        float x1 = PtsDeb.m_Lon / -Slope ;
        float y2 = PtsFin.m_Lat /  Slope ;
        float x2 = PtsFin.m_Lon / -Slope ;

        if ( g_GlobalVar.m_OrientationCapGps )
            {
            float x1p = x1*cosf(CapGpsRad)-y1*sinf(CapGpsRad) ;
            float y1p = x1*sinf(CapGpsRad)+y1*cosf(CapGpsRad) ;
            float x2p = x2*cosf(CapGpsRad)-y2*sinf(CapGpsRad) ;
            float y2p = x2*sinf(CapGpsRad)+y2*cosf(CapGpsRad) ;
            x1 = x1p ;
            y1 = y1p ;
            x2 = x2p ;
            y2 = y2p ;
            }

        x1 += 100 ;
        y1 += 100 ;
        x2 += 100 ;
        y2 += 100 ;

        // ligne de la trace
        display.drawLine( x1 , y1 , x2 , y2 , GxEPD_BLACK ) ;

        // point de la trace
        display.drawCircle( x1 , y1 , 1 , GxEPD_BLACK ) ;
        if ( ip == 1 )
            display.drawCircle( x1 , y1 , 3 , GxEPD_BLACK ) ;
        }

    // guidage point terrain connu
    if ( VecPts.GetSize() == 1 )
        {
        CFileGpx::StPoint PtsTerCon ;
        PtsTerCon.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat - VecPts[0].m_Lat ;
        PtsTerCon.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon - VecPts[0].m_Lon;

        float y1 = PtsTerCon.m_Lat / Slope ;
        float x1 = PtsTerCon.m_Lon / -Slope ;

        if ( g_GlobalVar.m_OrientationCapGps )
            {
            float x1p = x1*cosf(CapGpsRad)-y1*sinf(CapGpsRad) ;
            float y1p = x1*sinf(CapGpsRad)+y1*cosf(CapGpsRad) ;
            x1 = x1p ;
            y1 = y1p ;
            }

        x1 += 100 ;
        y1 += 100 ;

        display.drawCircle( x1 , y1 , 1 , GxEPD_BLACK ) ;
        display.drawCircle( x1 , y1 , 3 , GxEPD_BLACK ) ;
        }

    // position courante
    display.drawCircle( 100 , 100 , 4 , GxEPD_BLACK ) ;
    display.drawCircle( 100 , 100 , 3 , GxEPD_BLACK ) ;

    if ( !g_GlobalVar.m_OrientationCapGps )
        {
        // dessin du cap magnetique nord
        int xnm = -50 * cosf( g_GlobalVar.m_Mpu9250.m_CapMagnetique * PI / 180. - PI/2. ) + 100 ;
        int ynm =  50 * sinf( g_GlobalVar.m_Mpu9250.m_CapMagnetique * PI / 180. - PI/2. ) + 100 ;
        display.drawLine( 100 , 100 , xnm , ynm , GxEPD_BLACK ) ;

        // dessin du cap gps
        int xng = -30 * cosf( -g_GlobalVar.m_CapGpsDeg * PI / 180. + PI + PI/2. ) + 100 ;
        int yng =  30 * sinf( -g_GlobalVar.m_CapGpsDeg * PI / 180. + PI + PI/2. ) + 100 ;
        display.drawLine( 100 , 100 , xng , yng , GxEPD_BLACK ) ;
        }

    // nom de la trace
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0,10);
    display.print( pFileGpx->m_TrackName ) ;
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

    // modification echelle
    if ( g_GlobalVar.BoutonDroit() )
        Slope /= 2. ;
    if ( g_GlobalVar.BoutonGauche() )
        Slope *= 2. ;
    // modification orientation carte
    if ( g_GlobalVar.BoutonGaucheLong() || g_GlobalVar.BoutonDroitLong() )
        g_GlobalVar.m_OrientationCapGps = !g_GlobalVar.m_OrientationCapGps ;
    }

display.display(true);
}

#endif
