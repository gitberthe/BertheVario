////////////////////////////////////////////////////////////////////////////////
/// \file CGestEcran.h
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 09/02/2025
///

#ifndef _GESTECRANS_
#define _GESTECRANS_

#define ATTENTE_MESSAGE_GPS     -3
#define ATTENTE_STABILITE_GPS   -2
#define ATTENTE_VITESSE_VOL     -1

////////////////////////////////////////////////////////////////////////////////
/// \brief Gestion des differentes pages d'ecran.
class CGestEcrans : public CBoutons
{
public :
    CGestEcrans() ;

    // etats de l'automate
    enum EtatsAuto
        {
        ECRAN_Debut ,
        ECRAN_0_Vz ,
        ECRAN_1_Histo ,
        ECRAN_2a_ListeIgc ,
        ECRAN_2b_ConfirmArchIgc ,
        ECRAN_3a_TmaAll ,
        ECRAN_3b_TmaMod ,
        ECRAN_4_CfgFch ,
        ECRAN_5_TmaDessous ,
        ECRAN_6_Sys ,
        ERREUR ,
        FIN
        } ;

    void AfficheAll() ;
    void LancerTacheCalcul() ;
    EtatsAuto GetEtatAuto() const   ///< renvoi l'etat de l'automate d'affichage
                { return m_EtatAuto ; } ;
    bool IsPageChanged() const      ///< renvoie si l'on vient de chabger de page
        { return m_PageChanged ; } ;

    static void  GetCapChar( int CapDeg , char * NomCap ) ;

protected :
    virtual EtatsAuto EcranVz()      = 0 ;
    virtual EtatsAuto EcranHisto()   = 0 ;
    virtual EtatsAuto EcranTmaAll() = 0 ;
    virtual EtatsAuto EcranTmaMod() = 0 ;
    virtual EtatsAuto EcranSys() = 0 ;
    virtual EtatsAuto EcranCfgFch() = 0 ;
    virtual EtatsAuto EcranListeIgcFch() = 0 ;
    virtual EtatsAuto EcranConfimeArchIgcFch() = 0 ;
    virtual EtatsAuto EcranTmaDessous() = 0 ;
    virtual void ScreenRaz() = 0 ;
    virtual void ScreenOff() = 0 ;

    EtatsAuto Erreur() ;

    static void TacheScreenCalcul(void * param) ;

    const int       m_SecRetourEcran0 = 15 ; ///< auto retour ecran 0
    unsigned long   m_MillisEcran0 ; ///< time out de retour ecran 1 vers 0
    unsigned long   m_MillisVzPositive ; ///< time out de retour affichage finesse
    bool            m_PageChanged = false ;    ///< si on vient de changer de page ecran

    /// \brief etat de l'automate avec son pointeur de fonction
    class CEtatAutoFunc
        {
        public :
            CGestEcrans::EtatsAuto (CGestEcrans::*m_pFunction)() ;
        } ;

    EtatsAuto        m_EtatAuto = ECRAN_Debut ; //ECRAN_0_Vz ;  ///< etat courant de l'automate
    CEtatAutoFunc    m_Automate[FIN] ;       ///< l'automate
} ;

#endif
