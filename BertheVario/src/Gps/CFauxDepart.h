////////////////////////////////////////////////////////////////////////////////
/// \file CFauxDepart.h
///
/// \brief
///
/// \date creation     : 18/03/2024
/// \date modification : 11/04/2024
///

#ifndef _FAUXDEPART_
#define _FAUXDEPART_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe chargée de detecter un faux depart de vol pour cause gps non calé
/// d'ou vitesse depassant la limite lors du recalage.
/// Correction d'un probleme si vol par vent fort et stationnarite.
class CFauxDepart
{
public :
    bool IsBadFlightBegin() ;
    bool IsFlightLocked() const
        { return m_ADejaEteLointOuHaut ; }

    void InitFauxDepart( float Lat, float Lon, unsigned long Temps) ;     ///< temps de stationnarite

private :

    float         m_LastLat ;
    float         m_LastLon ;
    float         m_LastAlti ;
    unsigned long m_TempsStationnaireLatLon ;     ///< temps de stationnarite lat lon
    unsigned long m_TempsStationnaireAlti ;     ///< temps de stationnarite lat lon

    float   GetDistanceDeco() ;
    float   GetDistance( float x , float y , float z ) ;

    bool    m_ADejaEteLointOuHaut = false ;   ///< variable anti stationnarite au deco.
} ;

#endif
