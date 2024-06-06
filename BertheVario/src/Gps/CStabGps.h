////////////////////////////////////////////////////////////////////////////////
/// \file CStabGps.h
///
/// \brief
///
/// \date creation     : 03/04/2024
/// \date modification : 06/06/2024
///

#ifndef _STABGPS_
#define _STABGPS_

#define TAILLE_PILE 40

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de savoir si les points gps ne sont pas trop dispersés.
class CStabGps
{
public :
    void PushGpPos() ;

    bool IsGpsStable() const ;
    void RazGpsPos() ;

private :

    struct st_pos
        {
        float m_Lat = 0 ;
        float m_Lon = 0 ;
        } ;

    struct st_pos m_PosArr[TAILLE_PILE] ;  ///< tableau des derneires mesures
    bool  m_pile_full = false ;         ///< si la pile est pleine
    int   m_ipile = 0 ;                 ///< position dans le buffer
} ;

#endif
