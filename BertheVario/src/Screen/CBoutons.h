////////////////////////////////////////////////////////////////////////////////
/// \file CBoutons.h
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 24/08/2024
///

#ifndef _BOUTONS_
#define _BOUTONS_

#define DELAY_TRUE 100

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

private :
    bool m_BoutonGauche = false ;
    bool m_BoutonCentre = false ;
    bool m_BoutonDroit  = false ;

    static void TacheScanButton( void * param) ;
} ;

#endif
