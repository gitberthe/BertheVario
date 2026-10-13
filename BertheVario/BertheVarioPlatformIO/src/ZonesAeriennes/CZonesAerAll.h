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

#ifdef _COMP_ZONES_AERIENNES_
 #define NOM_FCH_ZONE_ACT       "SDCardAllVario/config/zonesact.txt"
 #define NOM_FCH_ZONE_PER       "SDCardAllVario/config/zonesper.txt"
 #define NOM_FCH_OUT_NOM_ZONE   "zonomout.txt"
#else
 #define NOM_FCH_ZONE_AER       "/config/zonesaer.txt"
 #define NOM_FCH_ZONE_PER       "/config/zonesper.txt"
 #define NOM_FCH_ZONE_ACT       "/config/zonesact.txt"
 #define NOM_FCH_OUT_NOM_ZONE   "/valid/zonomout.txt"
#endif

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

    #ifndef _COMP_ZONES_AERIENNES_
     CMutex      m_Mutex ;    ///< mutex pour nom zone CalcZone() et Ecran()
    #endif
    CLGString   m_NomZoneDansDessous ;  ///< nom de zone dans ou sous laquelle on est
    int         m_DansDessousUneZone ;  ///< si dans ou dessous une zone
    static bool IsEnoughMemory() ;

protected :
    CFile           m_FileNomZoneOut ;///< fichier des noms de zones courts pour verification transformation
    int             m_Plafond4Valid ; ///< pour validation
    bool            m_IsWeekEnd ;     ///< pour savoir si l'on est en week end

    CZoneAer *  Find( const char * NomOri ) ;

    void        LectureFichierZonesAer( const char * NomFchZoneAer_txt = NULL ) ;
    void        LectureFichierZonesPeriode() ;
    void        LectureFichierZonesActive() ;

private :

    CLGArrayPtr<const CZoneAer> m_VecZoneInArea ;   ///< zones comme TMA et RTBA
    CLGArrayPtr<const CZoneAer> m_VecZonesRet ;     ///< pour un tri plaus rapide

    CZoneAer *  TraiteBufferZoneAer( char * buff ) ;

    void        TraiteBufferZonePeriode( char * buff ) ;
    void        TraiteBufferZoneActive( char * buff ) ;

    void        TriZonesParAltitude( CLGArrayPtr< const CZoneAer > &VecZones) ;
    void        ClasseZoneRetenueAltitude( CLGArrayPtr< const CZoneAer > &VecZones ) ;


} ;

#endif
