////////////////////////////////////////////////////////////////////////////////
/// \file CZonesFchValid.h
///
/// \brief
///
/// \date 30/03/2024 : creation
/// \date 13/10/2026 : modification pour _COMP_ZONES_AERIENNES_
///

#ifndef _ZONESFCH_
#define _ZONESFCH_

#define ZONE_VALID_IN_FCH  "/valid/zonvalin.txt"

#ifdef _COMP_ZONES_AERIENNES_
 #define ZONE_VALID_OUT_FCH "zonvalou.txt"
#else
 #define ZONE_VALID_OUT_FCH "/valid/zonvalou.txt"
#endif

//#define ZONE_FRONT_OUT_FCH "/valid/zonfront.txt"

////////////////////////////////////////////////////////////////////////////////
/// \file Classe qui test et valide les zones aeriennes. fichier in =>
/// CZonesAerAll::CalcZone() => fichier out.
class CZonesFchValid : public CZonesAerAll
{
public :
    void Valid( const char * NomZoneAer_txt = NULL , const char * NomZoneValIn_txt = NULL ) ;

private :
    int   m_Ligne ;         ///< ligne fichier in
    CFile m_FileOutValid ;  ///< fichier se sortie validation detection zone in
    //File  m_FileOutFront ;  ///< fichier se sortie validation detection zone frontiere

    void TraiteBufferValidZoneIn( char * buff ) ;
} ;

#endif
