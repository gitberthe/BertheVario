////////////////////////////////////////////////////////////////////////////////
/// \file CGestEcran.h
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 07/06/2024
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
        ECRAN_0_Vz      = 0 ,
        ECRAN_1_Histo   = 1 ,
        ECRAN_2a_TmaAll = 2 ,
        ECRAN_2b_TmaMod = 3 ,
        ECRAN_3_Sys     = 4 ,
        ECRAN_4_CfgFch  = 5 ,
        ERREUR          = 6 ,
        FIN             = 7
        } ;

    void AfficheAll() ;
    void LancerTacheCalcul() ;
    EtatsAuto GetEtatAuto() const   ///< renvoi l'etat de l'automate d'affichage
                { return m_EtatAuto ; } ;

protected :
    virtual EtatsAuto Ecran0Vz()      = 0 ;
    virtual EtatsAuto Ecran1Histo()   = 0 ;
    virtual EtatsAuto Ecran2aTmaAll() = 0 ;
    virtual EtatsAuto Ecran2bTmaMod() = 0 ;
    virtual EtatsAuto Ecran3Sys() = 0 ;
    virtual EtatsAuto Ecran4CfgFch() = 0 ;
    EtatsAuto Erreur() ;

    static void TacheScreenCalcul(void * param) ;

    const int       m_SecRetourEcran0 = 15 ; ///< auto retour ecran 0
    unsigned long   m_MillisEcran0 ; ///< time out de retour ecran 1 vers 0
    unsigned long   m_MillisVzPositive ; ///< time out de retour affichage finesse

    /// \brief etat de l'automate avec son pointeur de fonction
    class CEtatAutoFunc
        {
        public :
            CGestEcrans::EtatsAuto (CGestEcrans::*m_pFunction)() ;
        } ;

    EtatsAuto        m_EtatAuto = ECRAN_0_Vz ;  ///< etat courant de l'automate
    CEtatAutoFunc    m_Automate[FIN] ;       ///< l'automate
} ;

#endif
