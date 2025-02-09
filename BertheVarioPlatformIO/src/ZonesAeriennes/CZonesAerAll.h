////////////////////////////////////////////////////////////////////////////////
/// \file CZonesAerAll.h
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 09/02/2025
///

#ifndef _ZONE_AR_ALL_
#define _ZONE_AR_ALL_

#define NOM_FCH_ZONE_AER "/config/zonesaer.txt"
#define NOM_FCH_ZONE_PER "/config/zonesper.txt"
#define NOM_FCH_ZONE_ACT "/config/zonesact.txt"

#define NOM_FCH_OUT_NOM_ZONE "/valid/zonomout.txt"

#define ZONE_EN_DEHORS          0
#define ZONE_LIMITE_ALTI        1
#define ZONE_DEDANS             2
#define ZONE_DESSOUS            4
#define ZONE_LIMITE_FRONTIERE   5
#define ZONE_DESSUS             6

#define DISTANCE_PROCHE_XY      1500

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe regoupant et calculant les zones aeriennes du fichier zonesaer.txt
class CZonesAerAll : public CSortArray<CZoneAer> , public CDate, public CDistFront
{
public :
    CZonesAerAll() ;

    void LectureFichiers() ;
    void EcritureFichierZonesActive() ;
    void CalcZone() ;
    void TriZonesNom() ;

    void SetDatePeriode() ;

    int GetNbZones() const
        { return m_NbZones ; } ;
    CZoneAer ** GetZoneArr()
        {return m_ZonesArr ; } ;

    void DeleteAll() ;

    CMutex m_Mutex ;    ///< mutex pour nom zone CalcZone() et Ecran()
    std::string m_NomZoneDansDessous ;  ///< nom de zone dans ou sous laquelle on est
    int         m_DansDessousUneZone ;  ///< si dans ou dessous une zone
    std::string m_NomZoneEnLimite   ;   ///< nom de zone en limite
    int         m_LimiteZone ;          ///< si en limite de zone

protected :
    File            m_FileNomZoneOut ;///< fichier des noms de zones courts pour veriofiocation transformation
    int             m_NbZones = 0 ;   ///< nombre de zones
    CZoneAer **     m_ZonesArr= NULL ;///< tableau des zones
    int             m_Plafond4Valid ; /// pour validation

    CZoneAer * Find( const char * NomOri ) ;

private :
    void        LectureFichierZonesAer() ;
    CZoneAer *  TraiteBufferZoneAer( char * buff ) ;
    void        LectureFichierZonesPeriode() ;
    void        TraiteBufferZonePeriode( char * buff ) ;
    void        LectureFichierZonesActive() ;
    void        TraiteBufferZoneActive( char * buff ) ;

    void        TriZonesParAltitude(std::vector< const CZoneAer * > &VecZones) ;
    void        ClasseZoneRetenueAltitude( std::vector< const CZoneAer * > &VecZones ) ;


} ;

#endif
