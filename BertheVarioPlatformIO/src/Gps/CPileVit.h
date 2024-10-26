////////////////////////////////////////////////////////////////////////////////
/// \file CPileVit.h
///
/// \brief
///
/// \date creation     : 02/09/2024
/// \date modification : 06/09/2024
///

#ifndef _CPILEVIT_
#define _CPILEVIT_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe qui permet de calculer gerer les vittesses gps pour de detecter
/// un depart de vol avec une erreure de positionnemnt XY gps habituelle lors du
/// changement du nombre de satellites.
class CPileVit
{
public :
    ~CPileVit() ;

    bool IsStartFlight() const ;
    void PusGpsVit() ;
    void ResetVit() ;
    void SatChange() ;

private :

    float * m_PosArr = NULL ;       ///< tableau des dernieres mesures
    bool  m_pile_full = false ;     ///< si la pile est pleine
    int   m_TaillePile = 0 ;        ///< taille de la pile voire fichier configuration
    int   m_ipile = 0 ;             ///< position dans le buffer
} ;

#endif
