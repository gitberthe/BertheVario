////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.h
///
/// \brief Variable globale du projet
///
/// \date creation     : 02/03/2024
/// \date modification : 14/01/2025
///

#ifndef _GLOBALVAR_
#define _GLOBALVAR_

class CSimuVol ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Variable globale du projet. Contient CConfigFile m_Config, CMS5611 m_MS5611
/// , CMpu9250 m_Mpu9250, CZonesAerAll m_ZonesAer et CHgt2Agl m_Hgt2Agl.
#if ( TYPE_SCREEN == 154 )
 class CGlobalVar : public CGps, public CScreen154, public CVarioBeep, public CSDCard, public CNumTaskArr, public CFinSol , public CSoundSvr , public CRandoVol
#endif
#if ( TYPE_SCREEN == 290 )
 class CGlobalVar : public CGps, public CScreen290, public CVarioBeep, public CSDCard, public CNumTaskArr, public CFinSol , public CSoundSvr , public CRandoVol
#endif
{
public :
    CGlobalVar() ;

    CMutex      m_MutexCore ; ///< mutex pour mise a jour des % cpu
    CMutex      m_MutexI2c ; ///< mutex d'accet au bus car trop de plantage nan alti baro / beeper / cap magnetique
    CMutex      m_MutexVariable ; ///< mutex de la variable globale pour lat/lon gps sous forme char entre tache gps / igc
    CConfigFile m_Config ;  ///< variables issues du fichier de config
    CMS5611     m_MS5611 ;  ///< capteur de pression
    CMpu9250    m_Mpu9250 ; ///< capteur accelerometrique
    CFilesHistoVols m_HistoVol ; ///< historique du vol precedent
    CZonesFch   m_ZonesAerAll ;///< tableau des zones aeriennes pris en compte
    CHgt2Agl    m_Hgt2Agl ; ///< pour determiner la hauteur sol
    CPileVit    m_PileVit ; ///< pour lancement vol suivant la vitesse gps

    bool m_ModeHttp ;                   ///< si mode http wifi
    bool m_ModeRandoVol ;               ///< si mode rando-vol
    bool m_BeepAttenteGVZone = true ;   ///< beep d'attente Gps/Vitesse/Zone
    bool m_StopLoop = false ;           ///< pour reboot

    // pour affichage
    float m_VitVertMS ;     ///< vitesse verticale
    int   m_DureeVolMin ;   ///< duree vol en minutes

    // pour elaboration finesse
    CTerrainsConnu m_TerrainArr ;     ///< fichier de terrains connus pour elaboration finesse
    CLocTerrain    m_TerrainPosCur ;  ///< position courante
    CLocTerrain    m_TerrainPosDeco ; ///< position de decollage

    // parametres Gps
    float m_AltiGps = 0. ;      ///< alti gps en metres
    int   m_AltitudeSolHgt = 0 ;    ///< altitude sol hgt
    int   m_CapGpsDeg = 0 ;     ///< cap gps en degres
    float m_VitesseKmh = 0. ;   ///< vitesse gps en km/h
    int   m_Annee = 1 ;         ///< Annee gps
    int   m_Mois = 1 ;          ///< Mois gps
    int   m_Jour = 1 ;          ///< Jour gps
    int   m_HeureSec = 0 ;      ///< heure gps en seconde
    char  m_LonChar[10] ;       ///< pour fichier igc
    char  m_LatChar[10] ;       ///< pour fichier igc

    // ecran statistiques
    int   m_PercentCore0 ;      ///< pourcentage utilisation core 0
    int   m_PercentCore1 ;      ///< pourcentage utilisation core 1

    float GetDeriveDeg() ;
    float GetVoltage() ;
    void  GainMemoire() ;
    static void  RelancerEnregistrementFichier() ;

    // fonction d'initialisation
    void  InitAlim() ;
    void  InitI2C() ;

    static void  DeInit() ;
    static float GetDiffAngle( float AngleA , float AngleB ) ;
    static void  beeper( int frequence , int DurationMs , int Volume = VOLUME_MOYEN ) ;
    static void  BeepOk() ;
    static void  BeepError(bool small = false) ;
    static void  Reboot() ;
    static void  TacheRelanceIgc(void *param);

    friend class CSimuVol ;
} ;

#endif
