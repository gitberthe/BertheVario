////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.cpp
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date 19/10/2025 : refonte avec CStabGps supprime.
/// \date 24/10/2025 : scan 10 hz
/// \date 27/11/2025 : plus de bip lors d'appui sur les boutons et mutex I2C
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

#ifdef _BERTHE_VARIO_213_
///////////////////////////////////////////////////////////////////////////////
/// \brief Test telechargement. Bouton droit long seulement appuye
bool CBoutons::TestOta()
{
if ( ms_BoutonDroitLong && digitalRead(BUTTON_A_PIN) && digitalRead(BUTTON_B_PIN) )
    return true ;
return false ;
}
#endif

unsigned long DRAM_ATTR CBoutons::ms_TimeDelayPurgeMs = 0 ;
bool DRAM_ATTR CBoutons::ms_BoutonGauche = false ;
bool DRAM_ATTR CBoutons::ms_BoutonGaucheLong = false ;

bool DRAM_ATTR CBoutons::ms_BoutonCentre = false ;
bool DRAM_ATTR CBoutons::ms_BoutonCentreLong = false ;

bool DRAM_ATTR CBoutons::ms_BoutonDroit = false ;
bool DRAM_ATTR CBoutons::ms_BoutonDroitLong = false ;

unsigned long DRAM_ATTR CBoutons::ms_TimePushButtonA = 0 ;
unsigned long DRAM_ATTR CBoutons::ms_TimePushButtonB = 0 ;
unsigned long DRAM_ATTR CBoutons::ms_TimePushButtonC = 0 ;

bool DRAM_ATTR CBoutons::ms_Want2Reboot = false ;

TimerHandle_t DRAM_ATTR CBoutons::ms_countdownTimer_A = NULL ;
TimerHandle_t DRAM_ATTR CBoutons::ms_countdownTimer_B = NULL ;
TimerHandle_t DRAM_ATTR CBoutons::ms_countdownTimer_C = NULL ;


///////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui initialise les boutons
void CBoutons::InitButton()
{
#ifdef _LG_DEBUG_
 Serial.println("Init des boutons" ) ;
#endif

// Setup the button with an internal pull-up
//g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 pinMode(BUTTON_A_PIN, INPUT_PULLUP);
 pinMode(BUTTON_B_PIN, INPUT_PULLUP);
 pinMode(BUTTON_C_PIN, INPUT_PULLUP);
//g_GlobalVar.m_MutexI2c.RelacherMutex() ;

RazBoutons() ;

ms_countdownTimer_A = xTimerCreate("CountdownA", DELAY_APPUI_LONG / portTICK_PERIOD_MS,
    pdFALSE, // One-shot timer
    NULL, TimerAppuiLongFinished_A );
ms_countdownTimer_B = xTimerCreate("CountdownB", DELAY_APPUI_LONG / portTICK_PERIOD_MS,
    pdFALSE, // One-shot timer
    NULL, TimerAppuiLongFinished_B );
ms_countdownTimer_C = xTimerCreate("CountdownC", DELAY_APPUI_LONG / portTICK_PERIOD_MS,
    pdFALSE, // One-shot timer
    NULL, TimerAppuiLongFinished_C );

attachInterrupt(BUTTON_A_PIN, Ext_INT_A_ISR_Change, CHANGE);
attachInterrupt(BUTTON_B_PIN, Ext_INT_B_ISR_Change, CHANGE);
attachInterrupt(BUTTON_C_PIN, Ext_INT_C_ISR_Change, CHANGE);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction interruption timer
void CBoutons::TimerAppuiLongFinished_A(TimerHandle_t xTimer)
{
//Serial.println("Countdown Complete!");
if ( ms_TimePushButtonA != 0 )
    {
    ms_BoutonGaucheLong = true ;
    ms_TimePushButtonA = 0 ;
    g_GlobalVar.PurgeBoutons( DELAY_APPUI_LONG * 2 ) ;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction interruption timer
void CBoutons::TimerAppuiLongFinished_B(TimerHandle_t xTimer)
{
//Serial.println("Countdown Complete!");
if ( ms_TimePushButtonB != 0 )
    {
    ms_BoutonCentreLong = true ;
    ms_TimePushButtonB = 0 ;
    g_GlobalVar.PurgeBoutons( DELAY_APPUI_LONG * 2 ) ;
    }

}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction interruption timer
void CBoutons::TimerAppuiLongFinished_C(TimerHandle_t xTimer)
{
//Serial.println("Countdown Complete!");
if ( ms_TimePushButtonC != 0 )
    {
    ms_BoutonDroitLong = true ;
    ms_TimePushButtonC = 0 ;
    g_GlobalVar.PurgeBoutons( DELAY_APPUI_LONG * 2 ) ;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Interruption  bonton gauche
void IRAM_ATTR CBoutons::Ext_INT_A_ISR_Change()
{
// pas de blocage car alarme en vol bouton A

// si purge boutons
if ( g_GlobalVar.TestPurgeBoutons() )
    {
    ms_TimePushButtonA = 0 ;
    return ;
    }

// timer appui long
if ( ! TestBouton( BUTTON_A_PIN ) )
    {
    if ( ms_TimePushButtonA == 0 )
        {
        ms_TimePushButtonA = millis() ;
        xTimerStart(ms_countdownTimer_A, 0);
        }
    }
// appui court si temps court
else
    {
    if ( ms_TimePushButtonA != 0 && millis() - ms_TimePushButtonA < DELAY_APPUI_LONG )
        {
        ms_BoutonGauche = true ;
        ms_TimePushButtonA = 0 ;
        //g_GlobalVar.BeepOk() ;
        g_GlobalVar.PurgeBoutons( DELAY_APPUI_LONG ) ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Interruption  bonton gauche
void IRAM_ATTR CBoutons::Ext_INT_B_ISR_Change()
{
// si pas au sol ni en vol
if ( !g_GlobalVar.m_FinDeVol.IsAtGround() )
    {
    ms_TimePushButtonB = 0 ;
    return ;
    }

// si purge boutons
if ( g_GlobalVar.TestPurgeBoutons() )
    {
    ms_TimePushButtonB = 0 ;
    return ;
    }

// timer appui long
if ( ! TestBouton( BUTTON_B_PIN ) )
    {
    if ( ms_TimePushButtonB == 0 )
        {
        ms_TimePushButtonB = millis() ;
        xTimerStart(ms_countdownTimer_B, 0);
        }
    }
// appui court si temps court
else
    {
    if ( ms_TimePushButtonB != 0 && millis() - ms_TimePushButtonB < DELAY_APPUI_LONG )
        {
        ms_BoutonCentre = true ;
        ms_TimePushButtonB = 0 ;
        //g_GlobalVar.BeepOk() ;
        g_GlobalVar.PurgeBoutons( DELAY_APPUI_LONG ) ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Interruption  bonton gauche
void IRAM_ATTR CBoutons::Ext_INT_C_ISR_Change()
{
// si pas au sol ni en vol
if ( !g_GlobalVar.m_FinDeVol.IsAtGround() )
    {
    ms_TimePushButtonC = 0 ;
    return ;
    }

bool BouA = TestBouton( BUTTON_A_PIN ) ;
bool BouB = TestBouton( BUTTON_B_PIN ) ;
bool BouC = TestBouton( BUTTON_C_PIN ) ;

// reboot si necessaire
TestReboot(BouA,BouB,BouC) ;

// si purge boutons
if ( g_GlobalVar.TestPurgeBoutons() )
    {
    ms_TimePushButtonC = 0 ;
    return ;
    }


// timer appui long
if ( ! BouC )
    {
    if ( ms_TimePushButtonC == 0 )
        {
        ms_TimePushButtonC = millis() ;
        xTimerStart(ms_countdownTimer_C, 0);
        }
    }
// appui court si temps court
else
    {
    if ( ms_TimePushButtonC != 0 && millis() - ms_TimePushButtonC < DELAY_APPUI_LONG )
        {
        ms_BoutonDroit = true ;
        ms_TimePushButtonC = 0 ;
        //g_GlobalVar.BeepOk() ;
        g_GlobalVar.PurgeBoutons( DELAY_APPUI_LONG ) ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Demande de reboot. Attention routine interupetion doit etre rapide.
void CBoutons::TestReboot( bool BouA , bool BouB , bool BouC )
{
// a et c appuye
if ( !BouA && BouB && !BouC )
    {
    // reboot
    g_GlobalVar.SetWant2Reboot() ;

    ms_BoutonGauche = false ;
    ms_BoutonCentre = false ;
    ms_BoutonDroit = false ;

    ms_BoutonGaucheLong = false ;
    ms_BoutonCentreLong = false ;
    ms_BoutonDroitLong = false ;
    }
}



///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton gauche.
bool CBoutons::BoutonGauche()
{
if ( ms_BoutonGauche ) // && ! g_GlobalVar.m_StopLoop )
    {
    ms_BoutonGauche = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton gauche long.
bool CBoutons::BoutonGaucheLong()
{
if ( ms_BoutonGaucheLong ) // && ! g_GlobalVar.m_StopLoop )
    {
    ms_BoutonGaucheLong = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton tous gauche.
bool CBoutons::BoutonGaucheTousAppui()
{
if ( BoutonGauche() || BoutonGaucheLong() /*|| BoutonGaucheDoubleAppui()*/ )
    return true ;
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton tous centre.
bool CBoutons::BoutonCentreTousAppui()
{
if ( BoutonCentre() || BoutonCentreLong() /*|| BoutonCentreDoubleAppui() */ )
    return true ;
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture bouton tous droit.
bool CBoutons::BoutonDroitTousAppui()
{
if ( BoutonDroit() || BoutonDroitLong() /* || BoutonDroitDoubleAppui() */ )
    return true ;
return false ;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton centre.
bool CBoutons::BoutonCentre()
{
if ( ms_BoutonCentre ) // && ! g_GlobalVar.m_StopLoop)
    {
    ms_BoutonCentre = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton centre.
bool CBoutons::BoutonCentreLong()
{
if ( ms_BoutonCentreLong ) // && ! g_GlobalVar.m_StopLoop)
    {
    ms_BoutonCentreLong = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton droit.
bool CBoutons::BoutonDroit()
{
if ( ms_BoutonDroit ) // && ! g_GlobalVar.m_StopLoop)
    {
    ms_BoutonDroit = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lecture reset bouton droit long.
bool CBoutons::BoutonDroitLong()
{
if ( ms_BoutonDroitLong ) // && ! g_GlobalVar.m_StopLoop)
    {
    ms_BoutonDroitLong = false ;
    return true ;
    }
return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief reset des boutons.
void CBoutons::RazBoutons()
{
ms_BoutonGaucheLong =
ms_BoutonCentreLong =
ms_BoutonDroitLong =

/*m_BoutonGaucheDoubleAppui =
m_BoutonCentreDoubleAppui =
m_BoutonDroitDoubleAppui =*/

ms_BoutonGauche =
ms_BoutonCentre =
ms_BoutonDroit  = false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief reset du bouton droit.
void CBoutons::RazBoutonDroit()
{
ms_BoutonDroitLong =
//m_BoutonDroitDoubleAppui =
ms_BoutonDroit  = false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Delay ou les botons sont purges
void CBoutons::PurgeBoutons( unsigned long DelayMs )
{
ms_TimeDelayPurgeMs = millis() + DelayMs ;
}

