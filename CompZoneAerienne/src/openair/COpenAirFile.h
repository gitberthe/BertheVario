////////////////////////////////////////////////////////////////////////////////
/// \file COpenAirFile.h
///
/// \brief
///
/// \date 10/10/2026 : creation
/// \date 11/10/2026 : modification
///

#pragma once

#define PAS_CERCLE_DEG  (1.)

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe qui lit un fichier openair et rempli sont tableau de CZone.
/// Definitions :
/// Espaces interdit au parapentes: A B C D (B pas en france)
/// Espaces autorise au parapentes: E F G (F pas en france)
/// AMSL    : Above Mean See Level (au dessus du niveau moyen des mer)
/// ASFC    : Above Surface (au dessus de la surface)
/// AGL     : Above ground level (niveau de vol au dessus de la terre)
/// FL      : Flight Level (Niveau de vol)
/// GND/SFC : Ground / surface
/// MSL     : Mean Sea Level (Niveau Moyen de la mer)
/// ZSM     : Zone Sensibilité Majeur (bulle de quietude)
class COpenAirFile
{
public :
    long TraiteFichier( FILE *fp ) ;

    std::vector<CZone*> m_pZoneArr ;    ///< tableau des zones du fichier

private :
    CZone* m_pZoneCur ;     ///< zone en cours

    long TraiteLigne( std::vector<std::string> & tokens ) ;

    long TraiteACClasseDeLaZone(std::vector<std::string> & tokens) ;    ///< classe A B C D E F G ZSM
    long TraiteANNomDeLaZone(std::vector<std::string> & tokens) ;
    long TraiteAYAirspaceType(std::vector<std::string> & tokens) ;      ///< CTR,TMA
    long TraiteAHAltitudeHaute(std::vector<std::string> & tokens) ;
    long TraiteALAltitudeBasse(std::vector<std::string> & tokens) ;
    long TraiteDPPointDePolygone( CVecZoneReduce::st_coord_poly * pStPts, std::vector<std::string> & tokens ) ;
    long TraiteVVariableAssignement(std::vector<std::string> & tokens) ;
    long TraiteDBArcBetweenPoints(std::vector<std::string> & tokens) ;
    long TraiteDCCercle(std::vector<std::string> & tokens) ;

    double ExtraireAltitudeEnMetres(std::vector<std::string> & tokens) ;

    bool                            m_DClockwise = true ;   ///< pour les arc de cercle
    CVecZoneReduce::st_coord_poly   m_XCenterDaDbDc ;       ///< Centre du cercle

} ;
