////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.cpp
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 21/01/2025
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

RazBoutons() ;

xTaskCreatePinnedToCore(TacheScanButton, "ScanButton", SCAN_BUTTON_STACK_SIZE, this, SCAN_BUTTON_PRIORITY,NULL, SCAN_BUTTON_CORE);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton gauche.
bool CBoutons::BoutonGauche()
{
if ( m_BoutonGauche && ! g_GlobalVar.m_StopLoop )
    {
    m_BoutonGauche = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton gauche long.
bool CBoutons::BoutonGaucheLong()
{
if ( m_BoutonGaucheLong && ! g_GlobalVar.m_StopLoop )
    {
    m_BoutonGaucheLong = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton gauche double appui.
bool CBoutons::BoutonGaucheDoubleAppui()
{
if ( m_BoutonGaucheDoubleAppui && ! g_GlobalVar.m_StopLoop)
    {
    m_BoutonGaucheDoubleAppui = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton centre double appui.
bool CBoutons::BoutonCentreDoubleAppui()
{
if ( m_BoutonCentreDoubleAppui && ! g_GlobalVar.m_StopLoop)
    {
    m_BoutonCentreDoubleAppui = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton droit double appui.
bool CBoutons::BoutonDroitDoubleAppui()
{
if ( m_BoutonDroitDoubleAppui && ! g_GlobalVar.m_StopLoop)
    {
    m_BoutonDroitDoubleAppui = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton tous gauche.
bool CBoutons::BoutonGaucheTousAppui()
{
if ( BoutonGauche() || BoutonGaucheLong() || BoutonGaucheDoubleAppui() )
    return true ;
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton tous centre.
bool CBoutons::BoutonCentreTousAppui()
{
if ( BoutonCentre() || BoutonCentreLong() || BoutonCentreDoubleAppui() )
    return true ;
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton tous droit.
bool CBoutons::BoutonDroitTousAppui()
{
if ( BoutonDroit() || BoutonDroitLong() || BoutonDroitDoubleAppui() )
    return true ;
return false ;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton centre.
bool CBoutons::BoutonCentre()
{
if ( m_BoutonCentre && ! g_GlobalVar.m_StopLoop)
    {
    m_BoutonCentre = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton centre.
bool CBoutons::BoutonCentreLong()
{
if ( m_BoutonCentreLong && ! g_GlobalVar.m_StopLoop)
    {
    m_BoutonCentreLong = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton droit.
bool CBoutons::BoutonDroit()
{
if ( m_BoutonDroit && ! g_GlobalVar.m_StopLoop)
    {
    m_BoutonDroit = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton droit long.
bool CBoutons::BoutonDroitLong()
{
if ( m_BoutonDroitLong && ! g_GlobalVar.m_StopLoop)
    {
    m_BoutonDroitLong = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief reset des boutons.
void CBoutons::RazBoutons()
{
m_BoutonGaucheLong =
m_BoutonCentreLong =
m_BoutonDroitLong =

m_BoutonGaucheDoubleAppui =
m_BoutonCentreDoubleAppui =
m_BoutonDroitDoubleAppui =

m_BoutonGauche =
m_BoutonCentre =
m_BoutonDroit  = false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief reset du bouton droit.
void CBoutons::RazBoutonDroit()
{
m_BoutonDroitLong =
m_BoutonDroitDoubleAppui =
m_BoutonDroit  = false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Reboot.
void CBoutons::TestReboot()
{
// reboot si pas en vol
if ( !digitalRead(BUTTON_A_PIN) && digitalRead(BUTTON_B_PIN) && !digitalRead(BUTTON_C_PIN) )
    {
    // pas de reboot en vol certain
    if ( g_GlobalVar.m_FinDeVol.IsFlightLocked() )
        return ;
    Serial.println( "reboot button") ;
    CGlobalVar::Reboot() ;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Tache de scan des boutons avec beep d'appui.
void CBoutons::TacheScanButton(void* param)
{
CBoutons * pThis = (CBoutons*) param ;

// a 100 hz
while( g_GlobalVar.m_TaskArr[SCAN_BUTON_NUM_TASK].m_Run )
    {
    delay( 1 ) ;

    // delait d'attente
    if ( g_GlobalVar.m_DelayAttenteMs != 0 )
        {
        unsigned long time = millis() ;
        while( millis()-time < g_GlobalVar.m_DelayAttenteMs )
            delay( 2 ) ;
        g_GlobalVar.m_DelayAttenteMs = 0 ;
        continue ;
        }

    // si delay purge bouton
    if ( g_GlobalVar.m_DelayPurgeMs != 0 )
        {
        unsigned long time = millis() ;
        pThis->m_BoutonGauche = false ;
        pThis->m_BoutonCentre = false ;
        pThis->m_BoutonDroit = false ;
        while( millis()-time < g_GlobalVar.m_DelayPurgeMs )
            {
            digitalRead(BUTTON_A_PIN) ;
            digitalRead(BUTTON_B_PIN) ;
            digitalRead(BUTTON_C_PIN) ;
            delay( 5 ) ;
            }
        g_GlobalVar.m_DelayPurgeMs = 0 ;
        continue ;
        }

    // test reboot
    g_GlobalVar.TestReboot() ;

    // si bouton gauche
    if ( !digitalRead(BUTTON_A_PIN) )
        {
        // au boot
        if ( g_GlobalVar.m_Boot )
            {
            CGlobalVar::BeepOk() ;
            pThis->m_BoutonGauche = true ;
            continue ;
            }
        unsigned long DebutAppui = millis() ;
        g_GlobalVar.m_StopLoop = true ;
        // mesure temps
        while ( !digitalRead(BUTTON_A_PIN) )
            {
            g_GlobalVar.TestReboot() ;
            if( (millis()-DebutAppui) >= DELAY_APPUI_LONG )
                break ;
            if ( digitalRead(BUTTON_A_PIN) )
                break ;
            }
        // appui long
        if ( (millis()-DebutAppui) >= DELAY_APPUI_LONG )
            {
            pThis->m_BoutonGaucheLong = true ;
            g_GlobalVar.m_DelayPurgeMs = DELAY_PURGE_LONG ;
            CGlobalVar::BeepOk() ;
            }
        // double appui
        else if ( (millis()-DebutAppui) < DELAY_DOUBLE_APPUI )
            {
            delay( DELAY_INTER_DOUBLE_APPUI ) ;
            while ( (millis()-DebutAppui) < DELAY_DOUBLE_APPUI )
                {
                if ( !digitalRead(BUTTON_A_PIN) )
                    {
                    pThis->m_BoutonGaucheDoubleAppui = true ;
                    CGlobalVar::beeper( 8000 , 300 ) ;
                    while ( !digitalRead(BUTTON_A_PIN) )
                        delay( 1 ) ;
                    goto end_a ;
                    }
                }
            pThis->m_BoutonGauche = true ;
            CGlobalVar::BeepOk() ;
            end_a:
                {
                }
            }
        else
            {
            pThis->m_BoutonGauche = true ;
            CGlobalVar::BeepOk() ;
            }
        g_GlobalVar.m_StopLoop = false ;
        }

    // si bouton centre
    if ( !digitalRead(BUTTON_B_PIN) )
        {
        // au boot
        if ( g_GlobalVar.m_Boot )
            {
            CGlobalVar::BeepOk() ;
            pThis->m_BoutonCentre = true ;
            continue ;
            }
        unsigned long DebutAppui = millis() ;
        g_GlobalVar.m_StopLoop = true ;
        // mesure temps
        while ( !digitalRead(BUTTON_B_PIN) )
            {
            g_GlobalVar.TestReboot() ;
            if( (millis()-DebutAppui) >= DELAY_APPUI_LONG )
                break ;
            if ( digitalRead(BUTTON_B_PIN) )
                break ;
            }
        // appui long
        if ( (millis()-DebutAppui) >= DELAY_APPUI_LONG )
            {
            //pThis->m_BoutonCentreLong = true ;
            g_GlobalVar.m_DelayPurgeMs = DELAY_PURGE_LONG ;
            CGlobalVar::BeepOk() ;
            g_GlobalVar.ScreenRaz() ;
            }
        // double appui
        else if ( (millis()-DebutAppui) < DELAY_DOUBLE_APPUI )
            {
            delay( DELAY_INTER_DOUBLE_APPUI ) ;
            while ( (millis()-DebutAppui) < DELAY_DOUBLE_APPUI )
                {
                if ( !digitalRead(BUTTON_B_PIN) )
                    {
                    //pThis->m_BoutonCentreDoubleAppui = true ;
                    CGlobalVar::beeper( 8000 , 300 ) ;
                    g_GlobalVar.ScreenRaz(true) ;
                    while ( !digitalRead(BUTTON_B_PIN) )
                        delay( 1 ) ;
                    goto end_b ;
                    }
                }
            pThis->m_BoutonCentre = true ;
            CGlobalVar::BeepOk() ;
            end_b:
                {
                }
            }
        else
            {
            pThis->m_BoutonCentre = true ;
            CGlobalVar::BeepOk() ;
            }
        g_GlobalVar.m_StopLoop = false ;
        }

    // si bouton droit
    if ( !digitalRead(BUTTON_C_PIN) )
        {
        // au boot
        if ( g_GlobalVar.m_Boot )
            {
            CGlobalVar::BeepOk() ;
            pThis->m_BoutonDroit = true ;
            continue ;
            }
        unsigned long DebutAppui = millis() ;
        g_GlobalVar.m_StopLoop = true ;
        // mesure temps
        while ( !digitalRead(BUTTON_C_PIN) )
            {
            g_GlobalVar.TestReboot() ;
            if( (millis()-DebutAppui) >= DELAY_APPUI_LONG )
                break ;
            if ( digitalRead(BUTTON_C_PIN) )
                break ;
            }
        // appui long
        if ( (millis()-DebutAppui) >= DELAY_APPUI_LONG )
            {
            pThis->m_BoutonDroitLong = true ;
            g_GlobalVar.m_DelayPurgeMs = DELAY_PURGE_LONG ;
            CGlobalVar::BeepOk() ;
            }
        // double appui
        else if ( (millis()-DebutAppui) < DELAY_DOUBLE_APPUI )
            {
            delay( DELAY_INTER_DOUBLE_APPUI ) ;
            while ( (millis()-DebutAppui) < DELAY_DOUBLE_APPUI )
                {
                if ( !digitalRead(BUTTON_C_PIN) )
                    {
                    pThis->m_BoutonDroitDoubleAppui = true ;
                    CGlobalVar::beeper( 8000 , 300 ) ;
                    while ( !digitalRead(BUTTON_C_PIN) )
                        delay( 1 ) ;
                    goto end_c ;
                    }
                }
            pThis->m_BoutonDroit = true ;
            CGlobalVar::BeepOk() ;
            end_c:
                {
                }
            }
        else
            {
            pThis->m_BoutonDroit = true ;
            CGlobalVar::BeepOk() ;
            }
        g_GlobalVar.m_StopLoop = false ;
        }
    }

g_GlobalVar.m_TaskArr[SCAN_BUTON_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

