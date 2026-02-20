////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.h
///
/// \brief
///
/// \date creation   : 09/03/2024
/// \date 27/11/2025 : appuy long a 1000ms.
///

#ifndef _BOUTONS_
#define _BOUTONS_

//#define DELAY_INTER_DOUBLE_APPUI    50
//#define DELAY_DOUBLE_APPUI          700
#define DELAY_APPUI_LONG             700
#define DELAY_ATTENTE_COURT          400
#define DELAY_ATTENTE_IDLE            50

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'utilisation des boutons. Scan 10hz avec beep.
class CBoutons
{
public :
    void InitButton() ;
    bool BoutonGauche() ;
    bool BoutonGaucheLong() ;
    //bool BoutonGaucheDoubleAppui() ;
    bool BoutonGaucheTousAppui() ;

    bool BoutonCentre() ;
    bool BoutonCentreLong() ;
    //bool BoutonCentreDoubleAppui() ;
    bool BoutonCentreTousAppui() ;

    bool BoutonDroit() ;
    bool BoutonDroitLong() ;
    //bool BoutonDroitDoubleAppui() ;
    bool BoutonDroitTousAppui() ;


    void RazBoutons() ;
    void RazBoutonDroit() ;
    void PurgeBoutons( unsigned long DelayMs ) ;

    static bool TestBouton( int NumBouton ) ;

    static void SetWant2Reboot( bool b = true ) ;
    static bool GetWant2Reboot() ;

    #ifdef _BERTHE_VARIO_213_
     bool TestOta() ;
    #endif

private :
    static bool DRAM_ATTR ms_BoutonGauche ;
    static bool DRAM_ATTR ms_BoutonGaucheLong ;
    //bool m_BoutonGaucheDoubleAppui = false ;

    static bool DRAM_ATTR ms_BoutonCentre ;
    static bool DRAM_ATTR ms_BoutonCentreLong ;
    //bool m_BoutonCentreDoubleAppui = false ;

    static bool DRAM_ATTR ms_BoutonDroit ;
    static bool DRAM_ATTR ms_BoutonDroitLong ;
    //bool m_BoutonDroitDoubleAppui = false ;

    static unsigned long DRAM_ATTR ms_TimeDelayPurgeMs ;  ///< delay de purge des boutons en ms
    static unsigned long DRAM_ATTR ms_TimePushButtonA ;
    static unsigned long DRAM_ATTR ms_TimePushButtonB ;
    static unsigned long DRAM_ATTR ms_TimePushButtonC ;

    static TimerHandle_t DRAM_ATTR ms_countdownTimer_A ;
    static TimerHandle_t DRAM_ATTR ms_countdownTimer_B ;
    static TimerHandle_t DRAM_ATTR ms_countdownTimer_C ;

    static bool DRAM_ATTR ms_Want2Reboot ;

    bool        TestPurgeBoutons() ;

    static void TestReboot(bool,bool,bool) ;

    static void IRAM_ATTR Ext_INT_A_ISR_Change() ;
    static void IRAM_ATTR Ext_INT_B_ISR_Change() ;
    static void IRAM_ATTR Ext_INT_C_ISR_Change() ;

    static void TimerAppuiLongFinished_A(TimerHandle_t xTimer) ;
    static void TimerAppuiLongFinished_B(TimerHandle_t xTimer) ;
    static void TimerAppuiLongFinished_C(TimerHandle_t xTimer) ;
    //static void DoReboot( bool BoutonA , bool BoutonB , bool BoutonC ) ;
    //static void TacheScanButton( void * param) ;
} ;

///////////////////////////////////////////////////////////////////////////////
/// \brief Attention inline dans les ISR
inline bool CBoutons::TestBouton( int NumBouton )
{
//Mutex.PrendreMutex() ;
return digitalRead(NumBouton) ;
//Mutex.RelacherMutex() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
inline bool CBoutons::TestPurgeBoutons()
{
/*TestBouton( BUTTON_A_PIN ) ;
TestBouton( BUTTON_B_PIN ) ;
TestBouton( BUTTON_C_PIN ) ;*/
if ( millis() < ms_TimeDelayPurgeMs )
    return true ;

return false ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
inline void CBoutons::SetWant2Reboot( bool b )
{
ms_Want2Reboot = b ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
inline bool CBoutons::GetWant2Reboot()
{
return ms_Want2Reboot ;
}


#endif
