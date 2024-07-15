////////////////////////////////////////////////////////////////////////////////
/// \file CZonesFch.h
///
/// \brief
///
/// \date creation     : 30/03/2024
/// \date modification : 15/07/2024
///

#ifndef _ZONESFCH_
#define _ZONESFCH_

#define ZONE_VALID_IN_FCH  "/valid/zonvalin.txt"
#define ZONE_VALID_OUT_FCH "/valid/zonvalou.txt"

////////////////////////////////////////////////////////////////////////////////
/// \file Classe qui test et valide les zones aeriennes. fichier in =>
/// CZonesAerAll::CalcZone() => fichier out.
class CZonesFch : public CZonesAerAll
{
public :
    void Valid() ;

private :
    int   m_Ligne ;     ///< ligne fichier in
    File  m_FileOut ;   ///< fichier se sortie

    void TraiteBufferValidZoneIn( char * buff ) ;
} ;

#endif
