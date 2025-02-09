////////////////////////////////////////////////////////////////////////////////
/// \file CZonesFchValid.h
///
/// \brief
///
/// \date creation     : 30/03/2024
/// \date modification : 09/02/2025
///

#ifndef _ZONESFCH_
#define _ZONESFCH_

#define ZONE_VALID_IN_FCH  "/valid/zonvalin.txt"
#define ZONE_VALID_OUT_FCH "/valid/zonvalou.txt"
#define ZONE_FRONT_OUT_FCH "/valid/zonfront.txt"

////////////////////////////////////////////////////////////////////////////////
/// \file Classe qui test et valide les zones aeriennes. fichier in =>
/// CZonesAerAll::CalcZone() => fichier out.
class CZonesFchValid : public CZonesAerAll
{
public :
    void Valid() ;

private :
    int   m_Ligne ;         ///< ligne fichier in
    File  m_FileOutValid ;  ///< fichier se sortie validation detection zone in
    File  m_FileOutFront ;  ///< fichier se sortie validation detection zone frontiere

    void TraiteBufferValidZoneIn( char * buff ) ;
} ;

#endif
