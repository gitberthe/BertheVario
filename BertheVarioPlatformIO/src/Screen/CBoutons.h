////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.h
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 18/01/2025
///

#ifndef _BOUTONS_
#define _BOUTONS_

#define DELAY_LONG          2000
#define DELAY_PURGE_LONG    1500

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'utilisation des boutons. Scan 10hz avec beep.
class CBoutons
{
public :
    void InitButton() ;
    bool BoutonGauche() ;
    bool BoutonGaucheLong() ;
    bool BoutonCentre() ;
    bool BoutonCentreLong() ;
    bool BoutonDroit() ;
    bool BoutonDroitLong() ;
    void RazBoutons() ;
    void RazBoutonDroit() ;
    void PurgeBoutons( unsigned long DelayMs )
            { m_DelayPurgeMs = DelayMs ; } ;
    void BootEffectue()
            { m_Boot = false ; } ;

private :
    void TestReboot() ;

    bool m_Boot = true ;                ///< true jusqu'a la fin du boot pour eviter des boutons long
    bool m_BoutonGauche     = false ;
    bool m_BoutonGaucheLong = false ;
    bool m_BoutonCentre     = false ;
    bool m_BoutonCentreLong = false ;
    bool m_BoutonDroit      = false ;
    bool m_BoutonDroitLong  = false ;
    unsigned long m_DelayPurgeMs = 0 ;      ///< delay de purge des boutons en ms

    static void TacheScanButton( void * param) ;
} ;

#endif
