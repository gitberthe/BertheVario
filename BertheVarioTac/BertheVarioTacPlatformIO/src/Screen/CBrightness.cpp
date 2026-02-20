////////////////////////////////////////////////////////////////////////////////
/// \file CBrightness.cpp
///
/// \brief
///
/// \date creation     : 25/01/2025
/// \date modification : 25/01/2025
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne la luminosite en fonction de l'exposition à la lumiere
/// (dans la poche desactive). Du temps du dernier touché de l'ecran et de la
/// hauteur sol.
void CBrightness::SetBrightness( bool * pToucheScreen )
{
// mutex driver ecran / handle bouton
m_Mutexluminosite.PrendreMutex() ;

static int last_brightness = 1 ;
int new_brightness = 1 ;

// dans le noir
if ( g_GlobalVar.GetBrightness() < 0. )
    new_brightness = 0 ;

// si pas page Vz
//if ( g_GlobalVar.m_Screen.GetEtatAuto() != CAutoPages::ECRAN_0_Vz )
//    new_brightness = 255 ;

// si ecran appuye
if ( pToucheScreen != NULL && *pToucheScreen )
    m_LastScreenTouch = millis() ;

// si temps assez court depuis dernier appui
if ( (millis() - m_LastScreenTouch)/1000 < g_GlobalVar.m_Config.m_lum_seconde )
    new_brightness = 255 ;

// si en vol
if ( g_GlobalVar.m_FinDeVol.IsInFlight() )
    new_brightness = 255 ;

// si premier touche ecran on desactive le touche
if ( new_brightness != last_brightness && pToucheScreen != NULL )
    *pToucheScreen = false ;

// positionnement luminosite
last_brightness = new_brightness ;
g_tft.setBrightness( new_brightness ) ;

// mutex driver ecran / handle bouton
m_Mutexluminosite.RelacherMutex() ;
}
