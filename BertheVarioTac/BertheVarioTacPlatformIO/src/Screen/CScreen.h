////////////////////////////////////////////////////////////////////////////////
/// \file CScreen.h
///
/// \brief Definition des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 08/02/2025
///

#ifndef _CSCREEN_
#define _CSCREEN_

// define de mode G S V
#define ATTENTE_MESSAGE_GPS     -3
#define ATTENTE_STABILITE_GPS   -2
#define ATTENTE_VITESSE_VOL     -1

// define des champs a afficher page Vz
#define PAGE_VZ_DUREE_VOL  0
#define PAGE_VZ_ALTI_BARO  1
#define PAGE_VZ_VIT_SOL    2
#define PAGE_VZ_CAP_DEG    3
#define PAGE_VZ_CAP_LET    4
#define PAGE_VZ_VZ         5
#define PAGE_VZ_FIN_TER    6
#define PAGE_VZ_RECULADE   7

////////////////////////////////////////////////////////////////////////////////
/// \brief Definition des pages ecran.
class CScreen : public CTouchButtons , public CAutoPages
{
public :
    CScreen() ;

    void HandleTouchScreen() ;

    const int m_Largeur = 240 ;  ///< dimensions de l'écran
    const int m_Hauteur = 320 ;  ///< dimensions de l'écran

    void ScreenRaz( bool SaufBouton = true ) ;
    void ScreenRazButtons()
        { RazButtons() ; }  ;
    void DoChar( int x , int y , const char * ) ;

private :
    /// \brief item de la page menu
    struct StItem
        {
        int m_x , m_y ;
        std::string           m_Intitule ;
        CAutoPages::EtatsAuto m_Page ;
        } ;

    CAutoPages::EtatsAuto EcranVz() ;
    CAutoPages::EtatsAuto EcranHisto() ;
    CAutoPages::EtatsAuto EcranListeIgcFch() ;
    CAutoPages::EtatsAuto EcranTmaAll() ;
    CAutoPages::EtatsAuto EcranCfgFch() ;
    CAutoPages::EtatsAuto EcranTmaDessous() ;
    CAutoPages::EtatsAuto EcranSys() ;
    CAutoPages::EtatsAuto EcranWifiFileMgr() ;
    CAutoPages::EtatsAuto EcranTelechargementFirmware() ;
    CAutoPages::EtatsAuto EcranMenu() ;
    CAutoPages::EtatsAuto EcranCalibreMagnetique() ;
    CAutoPages::EtatsAuto EcranRandoVolMenu() ;
    CAutoPages::EtatsAuto EcranRandoVolCarte() ;
    CAutoPages::EtatsAuto EcranRandoVolInfo() ;

    CAutoPages::EtatsAuto EcranTmaMod() ;
    CAutoPages::EtatsAuto EcranConfimeArchIgcFch() ;

    void GetCapChar( int CapDeg , char * NomCap ) ;

    std::vector<CText2Screen>   m_T2SPageVzArr ;  ///< tableau des champs page Vz
} ;

#endif
