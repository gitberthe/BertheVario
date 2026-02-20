////////////////////////////////////////////////////////////////////////////////
/// \file CAutoPage.h
///
/// \brief Automate de sequencement des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 23/01/2025
///

#ifndef _CAUTOPAGE_
#define _CAUTOPAGE_

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour la compatibilite 3 vario
class CGestEcrans
{
public :
    // etats de l'automate
    enum EtatsAuto
        {
        ECRAN_0_Vz ,
        ECRAN_1_Histo ,
        ECRAN_2a_ListeIgc ,
        ECRAN_2b_ConfirmArchIgc ,
        ECRAN_3a_TmaAll ,
        ECRAN_3b_TmaMod ,
        ECRAN_4_CfgFch ,
        ECRAN_5_TmaDessous ,
        ECRAN_6_Sys ,
        ECRAN_6b_CalMag ,
        ECRAN_6c_TelechFirm ,
        ECRAN_7_WifiFileMgr ,
        ECRAN_8_Menu ,
        ECRAN_9a_RandoVolMenu ,
        ECRAN_9b_RandoVolCarte ,
        ECRAN_9c_RandoVolInfo ,
        ERREUR ,
        FIN
        } ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Automate de sequencement des pages.
class CAutoPages : public CGestEcrans
{
public :
    CAutoPages() ;

    void      SequencementPages() ;
    EtatsAuto GetEtatAuto() const   ///< renvoie l'etat de l'automate d'affichage
                { return m_EtatAuto ; } ;

    bool IsPageChanged() const      ///< renvoie si l'on vien de chabger de page
        { return m_PageChanged ; } ;
    void ResetTimeOut()             ///< remet le time out page Vz à zero lors de configuration Cfg file ou Tma
        { m_MillisEcran0 = millis() ; } ;

    CMutex m_MutexTft ;

protected :

    virtual EtatsAuto EcranVz() = 0 ;
    virtual EtatsAuto EcranHisto() = 0 ;
    virtual EtatsAuto EcranListeIgcFch() = 0 ;
    virtual EtatsAuto EcranTmaAll() = 0 ;
    virtual EtatsAuto EcranCfgFch() = 0 ;
    virtual EtatsAuto EcranTmaDessous() = 0 ;
    virtual EtatsAuto EcranSys() = 0 ;
    virtual EtatsAuto EcranWifiFileMgr() = 0 ;
    virtual EtatsAuto EcranMenu() = 0 ;
    virtual EtatsAuto EcranCalibreMagnetique() = 0 ;
    virtual EtatsAuto EcranTelechargementFirmware() = 0 ;

    virtual EtatsAuto EcranTmaMod() = 0 ;
    virtual EtatsAuto EcranConfimeArchIgcFch() = 0 ;

    virtual CAutoPages::EtatsAuto EcranRandoVolMenu() = 0 ;
    virtual CAutoPages::EtatsAuto EcranRandoVolCarte() = 0 ;
    virtual CAutoPages::EtatsAuto EcranRandoVolInfo() = 0 ;

    virtual void ScreenRaz( bool SaufBoutons = true ) = 0 ;
    virtual void ScreenRazButtons() = 0 ;

    void SetVzEtatAuto() ;
    //virtual void ScreenOff() = 0 ;

    EtatsAuto Erreur() ;

    /// \brief etat de l'automate avec son pointeur de fonction
    class CEtatAutoFunc
        {
        public :
            CAutoPages::EtatsAuto (CAutoPages::*m_pFunction)() ;
        } ;

    int     m_CfgFileiChamps = -1 ;         ///< lors du deplacement dans les variables cfg file
    bool    m_CalibrationEnCours = false ;  ///< calibration magnetique en cours

private :
    EtatsAuto       m_EtatAuto = ECRAN_0_Vz ;   ///< etat courant de l'automate
    CEtatAutoFunc   m_Automate[FIN] ;       ///< l'automate
    bool            m_PageChanged = false ; ///< si l'on vient de changer de page
    const int       m_SecondesRetourEcran0 = 12;///< auto retour ecran 0
    unsigned long   m_MillisEcran0 ;            ///< time out de retour ecran 1 vers 0

    //static void TacheTouch(void *param) ;

} ;

#endif
