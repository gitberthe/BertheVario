////////////////////////////////////////////////////////////////////////////////
/// \file CZonesAerAll.h
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date 06/01/2026 : modification pour free zone periodique
/// \date 07/01/2026 : modification pour zone active non compresse
///

#ifndef _ZONE_AR_ALL_
#define _ZONE_AR_ALL_

#define NOM_FCH_ZONE_AER "/config/zonesaer.txt"
#define NOM_FCH_ZONE_PER "/config/zonesper.txt"
#define NOM_FCH_ZONE_ACT "/config/zonesact.txt"

#define NOM_FCH_OUT_NOM_ZONE "/valid/zonomout.txt"

#define ZONE_EN_DEHORS          0
#define ZONE_DEDANS             1
#define ZONE_DESSOUS            2
#define ZONE_DESSUS             3

#define NB_KILO_FREE_MEM        50

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe regoupant et calculant les zones aeriennes du fichier zonesaer.txt
class CZonesAerAll : public CLGArrayPtr<CZoneAer> , public CDate //, public CDistFront
{
public :
    CZonesAerAll() ;

    void LectureFichiers() ;
    void EcritureFichierZonesActive() ;
    void CalcZone() ;
    void TriZonesNom() ;
    void GestActivationZone( CZoneAer * , bool Active ) ;

    void SetDatePeriode() ;

    int GetNbZones() const
        { return GetSize() ; } ;

    void DeleteAll() ;

    CMutex      m_Mutex ;    ///< mutex pour nom zone CalcZone() et Ecran()
    CLGString   m_NomZoneDansDessous ;  ///< nom de zone dans ou sous laquelle on est
    int         m_DansDessousUneZone ;  ///< si dans ou dessous une zone
    static bool IsEnoughMemory() ;

protected :
    File            m_FileNomZoneOut ;///< fichier des noms de zones courts pour veriofiocation transformation
    int             m_Plafond4Valid ; ///< pour validation
    bool            m_IsWeekEnd ;     ///< pour savoir si l'on est en week end

    CZoneAer * Find( const char * NomOri ) ;

private :

    CLGArrayPtr<const CZoneAer> m_VecZoneInArea ;   ///< zones comme TMA et RTBA
    CLGArrayPtr<const CZoneAer> m_VecZonesRet ;     ///< pour un tri plaus rapide

    void        LectureFichierZonesAer() ;
    CZoneAer *  TraiteBufferZoneAer( char * buff ) ;
    void        LectureFichierZonesPeriode() ;
    void        TraiteBufferZonePeriode( char * buff ) ;
    void        LectureFichierZonesActive() ;
    void        TraiteBufferZoneActive( char * buff ) ;

    void        TriZonesParAltitude( CLGArrayPtr< const CZoneAer > &VecZones) ;
    void        ClasseZoneRetenueAltitude( CLGArrayPtr< const CZoneAer > &VecZones ) ;


} ;

#endif
