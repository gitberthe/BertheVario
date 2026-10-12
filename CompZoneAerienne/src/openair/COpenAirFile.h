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

class CZone ;

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
    COpenAirFile() ;

    enum TypeAltitudesAH_AL   ///< les differentes altitudes
        {
        AucuneAltitude ,
        AGL_AboveGroundLevel ,
        AMSL_AboveMeanSeaLevel ,
        ASFC_AboveSurface ,
        FL_FlightLevel,
        SFC_GND_Sol ,
        } ;

    enum TypeEspacesAC      ///< les differentes types d'espaces ZSM, A, B
        {
        AucunTypeDEspace          ,
        ZSM_PROTECT_BulleQuietude ,
        ABCD_interdit             ,
        EFG_authorize             ,
        } ;

    enum TypeZonesAY        ///< les differentes types de zone CTR TMA
        {
        AucunTypeDeZone           ,
        RTBA_R_Reglementee        ,
        ZIT_P_Interdite           ,
        GP_GliderProhibited       ,
        ZoneD_Dangeureuse         ,
        Q_DangerArea              ,
        CTR_ControlAerodrome      ,
        TMA_TerminalControlArea   ,
        AWY_Airway
        } ;

    long TraiteFichier( FILE *fp ) ;

    std::vector<CZone*> m_pZoneArr ;    ///< tableau des zones du fichier

private :

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

    long   RechercheChamp( const std::string & ToFind, const std::vector<std::string> & tokens ) ;
    double ExtraireAltitudeEnMetres(std::vector<std::string> & tokens) ;

    CZone*                          m_pZoneCur ;     ///< zone en cours
    bool                            m_DClockwise = true ;   ///< pour les arc de cercle
    CVecZoneReduce::st_coord_poly   m_XCenterDaDbDc ;       ///< Centre du cercle

    //std::map<TypeAltitudesAH_AL,std::string> m_AltitudesChamps ; ///< string des altitudes
    //std::map<TypeEspacesAC_AY,std::string>     m_TypeEspacesChamps ; ///< string des espaces, ZSM, A, B ...


} ;
