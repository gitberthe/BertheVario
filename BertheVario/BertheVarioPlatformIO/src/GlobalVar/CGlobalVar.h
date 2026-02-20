////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.h
///
/// \brief Variable globale du projet
///
/// \date creation     : 02/03/2024
/// \date 08/07/2025 : suppression xc_track et limite zone xy alti
/// \date 09/07/2025 : modification de beep vario sol
/// \date 14/10/2025 : suppression de StabGps.
/// \date 23/10/2025 : suppression de CFinSol
/// \date 27/11/2025 : plus de beep vario au demmarrage
///

#ifndef _GLOBALVAR_
#define _GLOBALVAR_

class CSimuVol ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Variable globale du projet. Contient CConfigFile m_Config, CMS5611 m_MS5611Press
/// , CMpu9250 m_Mpu9250, CZonesAerAll m_ZonesAer et CHgt2Agl m_Hgt2Agl.
class CGlobalVar : public CGps, public CScreen154, public CVarioBeep, public CSDCard, public CNumTaskArr, public CSoundSvr , public CRandoVol
{
public :
    CGlobalVar() ;

    CMutex          m_MutexI2c ;        ///< mutex d'accet au bus car trop de plantage nan alti baro / beeper / cap magnetique
    CMutex          m_MutexLatLonGps ;  ///< mutex de la variable globale pour lat/lon gps sous forme char entre tache gps / igc
    CMutex          m_MutexQueue ;      ///< mutex queue du SounfSrv
    CConfigFile     m_Config ;  ///< variables issues du fichier de config
    CMS5611Pression m_MS5611Press ;  ///< capteur de pression
    CMpu9250        m_Mpu9250 ; ///< capteur accelerometrique
    CFilesHistoVols m_HistoVol ; ///< historique du vol precedent
    CZonesFchValid  m_ZonesAerAll ;///< tableau des zones aeriennes pris en compte
    CHgt2Agl        m_Hgt2Agl ; ///< pour determiner la hauteur sol
    CPileVit        m_PileVit ; ///< pour lancement vol suivant la vitesse gps

    bool m_ModeHttp ;               ///< si mode http wifi
    bool m_ModeRandoVol ;           ///< si mode rando-vol
    bool m_ModeChecksum ;           ///< si mode verif cheksum
    bool m_BeepVario = false ;      ///< beep d'attente Gps/Vitesse/Zone

    // pour affichage
    float m_VitVertMS ;     ///< vitesse verticale
    float m_DureeVolMin = ATTENTE_MESSAGE_GPS ;   ///< duree vol en minutes

    // pour elaboration finesse
    CTerrainsConnu m_TerrainArr ;     ///< fichier de terrains connus pour elaboration finesse
    CLocTerrain    m_TerrainPosCur ;  ///< position courante
    CLocTerrain    m_TerrainPosDeco ; ///< position de decollage

    // parametres Gps
    float m_AltiGps = 0. ;      ///< alti gps en metres
    float m_AltitudeSolHgt = 0 ;///< altitude du sol hgt
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


    void  TestPddGroundZ() ;
    void  MiseAJourAltitudesSolEtBaroRec() ;
    float GetHauteurSol() const
        { return m_TerrainPosCur.m_AltiBaroRec - m_AltitudeSolHgt ; } ;

    // fonction d'initialisation
    void  InitAlim() ;
    void  InitI2C() ;
    bool  VerifyAllChecksumGood() ;

    static void  RelancerAttenteVol() ;
    static float GetDiffAngle( float AngleA , float AngleB ) ;
    static void  beeper( int frequence , int DurationMs , int Volume = VOLUME_MOYEN ) ;
    static void  BeepOk() ;
    static void  BeepError(bool small = false) ;
    static void  Reboot() ;

    friend class CSimuVol ;

private :
    CSoundSvr::StSoundRequest m_Req ;   ///< pour le son

    static void  TacheRelanceAttenteVol(void *param);
} ;

#endif
