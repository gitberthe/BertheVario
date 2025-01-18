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

#define DELAY_TRUE 10

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'utilisation des boutons. Scan 10hz avec beep.
class CBoutons
{
public :
    void InitButton() ;
    bool BoutonGauche() ;
    bool BoutonCentre() ;
    bool BoutonDroit() ;
    void RazBoutons() ;
    void RazBoutonDroit() ;
    void PurgeBoutons( unsigned long DelayMs )
            { m_DelayPurgeMs = DelayMs ; } ;

private :
    bool m_BoutonGauche = false ;
    bool m_BoutonCentre = false ;
    bool m_BoutonDroit  = false ;
    unsigned long m_DelayPurgeMs = 0 ;      ///< delay de purge des boutons en ms

    static void TacheScanButton( void * param) ;
} ;

#endif
