////////////////////////////////////////////////////////////////////////////////
/// \file CStabGps.h
///
/// \brief
///
/// \date creation     : 03/04/2024
/// \date modification : 13/01/2025
///

#ifndef _STABGPS_
#define _STABGPS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de savoir si les points gps ne sont pas trop dispersés.
class CStabGps
{
public :
    ~CStabGps() ;

    void PushGpPos4Stab() ;
    bool IsGpsStable() const ;
    void SetAlwaysStable()
            { m_AlwaysStable = true ; } ;
    bool IsAlwaysStable() const
            { return m_AlwaysStable ; } ;

private :

    struct st_pos
        {
        float m_Lat = 0 ;
        float m_Lon = 0 ;
        } ;

    struct st_pos * m_PosArr = NULL ;   ///< tableau des dernieres mesures
    bool  m_pile_full = false ;         ///< si la pile est pleine
    int   m_ipile = 0 ;                 ///< position dans le buffer
    int   m_TaillePile = 0 ;            ///< taille pile stabilisation gps
    bool  m_AlwaysStable = false ;      ///< lors d'un redemmarage en vols cause stationnaire long
} ;

#endif
