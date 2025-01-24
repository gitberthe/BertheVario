////////////////////////////////////////////////////////////////////////////////
/// \file CFinDeVol.h
///
/// \brief
///
/// \date creation     : 24/08/2024
/// \date modification : 23/01/2024
///

#ifndef _FINDEVOL_
#define _FINDEVOL_

#define TAILLE_PILE_FE 12
#define VITESSE_FAIBLE 5.
#define ALTI_DESSUS_DECO   5.
#define	VZ_PETITE   0.4

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine une fin de vol.
class CFinDeVol
{
public :
    void PushPos4FlihgtEnd() ;
    void InitFinDeVol() ;
    bool IsFlightEnd() ;
    bool IsFlightLocked() const ;
    bool IsInFlight() const ;

private :
    float GetDistance( float x , float y ) ;

    bool m_ADejaEteLointOuHaut = false ;

    struct st_pos
        {
        float m_Lat  = 0. ;
        float m_Lon  = 0. ;
        float m_Alti = 0. ;
        float m_VitHor  = 0. ;
        float m_VitVer  = 0. ;
        } ;

    struct st_pos m_PosArr[TAILLE_PILE_FE] ;  ///< tableau des dernieres mesures
    bool  m_pile_full = false ;         ///< si la pile est pleine
    int   m_ipile = 0 ;
} ;

#endif
