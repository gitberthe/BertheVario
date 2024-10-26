////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.cpp
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 03/09/2024
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
/// \brief reset bouton droit.
void CBoutons::RazBoutonDroit()
{
m_BoutonDroit  = false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Tache de scan des boutons avec beep d'appui.
void CBoutons::TacheScanButton(void* param)
{
CBoutons * pThis = (CBoutons*) param ;

// a 10 hz
while( g_GlobalVar.m_TaskArr[SCAN_BUTON_NUM_TASK].m_Run )
    {
    bool beep = false ;

    // si purge bouton
    if ( g_GlobalVar.m_DelayPurgeMs != 0 )
        {
        pThis->m_BoutonGauche = false ;
        pThis->m_BoutonCentre = false ;
        pThis->m_BoutonDroit = false ;
        unsigned long time = millis() ;
        while( millis()-time < g_GlobalVar.m_DelayPurgeMs )
            {
            digitalRead(BUTTON_A_PIN) ;
            digitalRead(BUTTON_B_PIN) ;
            digitalRead(BUTTON_C_PIN) ;
            delay( 1 ) ;
            }
        g_GlobalVar.m_DelayPurgeMs = 0 ;
        }

    // reboot si pas en vol
    if ( !digitalRead(BUTTON_A_PIN) && digitalRead(BUTTON_B_PIN) && !digitalRead(BUTTON_C_PIN) )
        {
        // pas de reboot en vol certain
        if ( g_GlobalVar.m_FinDeVol.IsFlightLocked() )
            continue ;
        Serial.println( "reboot button") ;
        CGlobalVar::Reboot() ;
        }

    // memorisation bouton gauche
    if( !pThis->m_BoutonGauche )
        {
        unsigned long time = millis() ;
        while ( !digitalRead(BUTTON_A_PIN) )
            {
            // appui 100ms minimum
            if( (millis()-time) >= DELAY_TRUE )
                {
                pThis->m_BoutonGauche = true ;
                beep = true ;
                break ;
                }
            delay( 1 ) ;
            }
        }

    // memorisation bouton centre
    if ( !pThis->m_BoutonCentre )
        {
        unsigned long time = millis() ;
        while ( !digitalRead(BUTTON_B_PIN) )
            {
            // appui 100ms minimum
            if( (millis()-time) >= DELAY_TRUE )
                {
                pThis->m_BoutonCentre = true ;
                beep = true ;
                break ;
                }
            delay( 1 ) ;
            }

        }

    // memorisation bouton droit
    if ( !pThis->m_BoutonDroit )
        {
        unsigned long time = millis() ;
        while ( !digitalRead(BUTTON_C_PIN) )
            {
            // appui 100ms minimum
            if( (millis()-time) >= DELAY_TRUE )
                {
                pThis->m_BoutonDroit = true ;
                beep = true ;
                break ;
                }
            delay( 1 ) ;
            }
        }

    // beep d'appui
    if ( beep )
        {
        beep = false ;
        CGlobalVar::BeepOk() ;
        }

    //delay( DELAY_TRUE ) ;
    }

g_GlobalVar.m_TaskArr[SCAN_BUTON_NUM_TASK].m_Stopped = true ;
}
