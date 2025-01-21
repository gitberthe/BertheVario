////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.h
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 20/01/2025
///

#ifndef _BOUTONS_
#define _BOUTONS_

//#define DELAY_ATTENTE_APRES_APPUI   15
#define DELAY_DOUBLE_APPUI          300
#define DELAY_APPUI_LONG            1500
#define DELAY_PURGE_LONG            2000

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'utilisation des boutons. Scan 10hz avec beep.
class CBoutons
{
public :
    void InitButton() ;
    bool BoutonGauche() ;
    bool BoutonGaucheLong() ;
    bool BoutonGaucheDoubleAppui() ;
    bool BoutonGaucheTousAppui() ;

    bool BoutonCentre() ;
    bool BoutonCentreLong() ;
    bool BoutonCentreDoubleAppui() ;
    bool BoutonCentreTousAppui() ;

    bool BoutonDroit() ;
    bool BoutonDroitLong() ;
    bool BoutonDroitDoubleAppui() ;
    bool BoutonDroitTousAppui() ;


    void RazBoutons() ;
    void RazBoutonDroit() ;
    void PurgeBoutons( unsigned long DelayMs )
            { m_DelayPurgeMs = DelayMs ; } ;
    void BootEffectue()
            { m_Boot = false ; } ;

private :
    void TestReboot() ;

    bool m_Boot = true ;                ///< true jusqu'a la fin du boot pour eviter des boutons long

    bool m_BoutonGauche            = false ;
    bool m_BoutonGauchePrisEnCompte= true ;
    bool m_BoutonGaucheLong        = false ;
    bool m_BoutonGaucheDoubleAppui = false ;
    unsigned long m_BoutonGaucheDernierAppui = 0 ;

    bool m_BoutonCentre            = false ;
    bool m_BoutonCentrePrisEnCompte= true ;
    bool m_BoutonCentreLong        = false ;
    bool m_BoutonCentreDoubleAppui = false ;
    unsigned long m_BoutonCentreDernierAppui = 0 ;

    bool m_BoutonDroit            = false ;
    bool m_BoutonDroitPrisEnCompte= true ;
    bool m_BoutonDroitLong        = false ;
    bool m_BoutonDroitDoubleAppui = false ;
    unsigned long m_BoutonDroitDernierAppui = 0 ;

    unsigned long m_DelayPurgeMs = 0 ;  ///< delay de purge des boutons en ms
    unsigned long m_DelayAttenteMs= 0 ; ///< delay d'attente avant nouvel appuy en ms



    static void TacheScanButton( void * param) ;
} ;

#endif
