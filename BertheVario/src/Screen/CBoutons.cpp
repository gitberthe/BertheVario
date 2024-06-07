////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.h
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 07/06/2024
///

#include "../BertheVario.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui initialise les boutons
void CBoutons::InitButton()
{
#ifdef _LG_DEBUG_
 Serial.println("Init des boutons" ) ;
#endif

// Setup the button with an internal pull-up
pinMode(BUTTON_A_PIN, INPUT_PULLUP);
pinMode(BUTTON_B_PIN, INPUT_PULLUP);
pinMode(BUTTON_C_PIN, INPUT_PULLUP);

m_BoutonCentre =
m_BoutonGauche =
m_BoutonDroit = false ;

xTaskCreatePinnedToCore(TacheScanButton, "ScanButton", SCAN_BUTTON_STACK_SIZE, this, SCAN_BUTTON_PRIORITY,NULL, SCAN_BUTTON_CORE);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton gauche.
bool CBoutons::BoutonGauche()
{
if ( m_BoutonGauche )
    {
    delay( DELAY_TRUE ) ;
    m_BoutonGauche = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton centre.
bool CBoutons::BoutonCentre()
{
if ( m_BoutonCentre )
    {
    delay( DELAY_TRUE ) ;
    m_BoutonCentre = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton droit.
bool CBoutons::BoutonDroit()
{
if ( m_BoutonDroit )
    {
    delay( DELAY_TRUE ) ;
    m_BoutonDroit = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief reset des boutons.
void CBoutons::RazBoutons()
{
m_BoutonGauche =
m_BoutonCentre =
m_BoutonDroit  = false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Tache de scan des boutons avec beep d'appui.
void CBoutons::TacheScanButton(void* param)
{
CBoutons * pThis = (CBoutons*) param ;

// a 5 hz
while( g_GlobalVar.m_TaskArr[SCAN_BUTON_NUM_TASK].m_Run )
    {
    bool beep = false ;

    // reboot si pas en vol
    if ( !digitalRead(BUTTON_A_PIN) && digitalRead(BUTTON_B_PIN) && !digitalRead(BUTTON_C_PIN) )
        {
        // pas de reboot en vol certain
        if ( g_GlobalVar.IsFlightLocked() )
            continue ;
        Serial.println( "reboot button") ;
        CGlobalVar::Reboot() ;
        }

    // memorisation bouton gauche
    if ( !pThis->m_BoutonGauche && !digitalRead(BUTTON_A_PIN) )
        {
        pThis->m_BoutonGauche = true ;
        beep = true ;
        }

    // memorisation bouton centre
    if ( !pThis->m_BoutonCentre && !digitalRead(BUTTON_B_PIN) )
        {
        pThis->m_BoutonCentre = true ;
        beep = true ;
        }

    // memorisation bouton droit
    if ( !pThis->m_BoutonDroit && !digitalRead(BUTTON_C_PIN) )
        {
        pThis->m_BoutonDroit = true ;
        beep = true ;
        }

    // beep d'appui
    if ( beep )
        {
        beep = false ;
        CGlobalVar::BeepOk() ;
        }

    delay( 200 ) ;
    }

g_GlobalVar.m_TaskArr[SCAN_BUTON_NUM_TASK].m_Stopped = true ;
}
