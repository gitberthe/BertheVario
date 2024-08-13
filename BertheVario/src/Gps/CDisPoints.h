////////////////////////////////////////////////////////////////////////////////
/// \file CDisPoints.h
///
/// \brief
///
/// \date creation     : 12/08/2024
/// \date modification : 13/08/2024
///

#ifndef _CDISPOINT_
#define _CDISPOINT_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe qui permet de calculer la dispersion moyenne des points
/// (ayant une vitesse gps minimale sur x secondes) autour de leur barycentre.
/// Permet ainsi de detecter un depart de vol avec une erreure de positionnemnt
/// XY gps habituelle de 50m au lancement.
class CDisPoints
{
public :
    ~CDisPoints() ;

    bool IsInFlight() const ;
    void PusGpsPos4Disp() ;
    void ResetXY() ;

private :

    struct st_pos
        {
        float m_Lat = 0 ;
        float m_Lon = 0 ;
        } ;

    struct st_pos * m_PosArr = NULL ;  ///< tableau des dernieres mesures
    bool  m_pile_full = false ;        ///< si la pile est pleine
    int   m_TaillePile ;               ///< taille de la pile voire fichier configuration
    int   m_ipile = 0 ;                ///< position dans le buffer
} ;

#endif
