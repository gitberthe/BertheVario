////////////////////////////////////////////////////////////////////////////////
/// \file CPileVit.h
///
/// \brief
///
/// \date creation   : 02/09/2024
/// \date 14/10/2025 : refonte avec CStabGps supprime.
/// \date 21/10/2025 : nombre de fix altibaro, debut de vol
/// \date 22/10/2025 : IsHDopOk()
/// \date 14/01/2026 : TAILLE_PILE_VITESSE passe de 3 a 4
///

#ifndef _CPILEVIT_
#define _CPILEVIT_

#define TAILLE_PILE_VITESSE       (4)

#define LAST_FIX_ALTIBARO_SEC    (10.)    ///< 10sec ecart multiple pour les fix alti baro

#define DEBUT_VOL_INCONNU      0
#define DEBUT_VOL_VIT_VERT     1
#define DEBUT_VOL_VIT_HOR      2
#define DEBUT_VOL_HAUTEUR_SOL  3
#define DEBUT_VOL_BOUTON       4

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe qui permet de calculer gerer les vittesses gps pour de detecter
/// un depart de vol avec une erreure de positionnemnt XY gps habituelle lors du
/// changement du nombre de satellites.
class CPileVit
{
public :
    CPileVit() ;
    ~CPileVit() ;

    struct StPileVit
        {
        float m_VitVerticale ;
        float m_VitHorizontale ;
        float m_HDop ;
        } ;

    bool IsWaitForStab() const ;
    bool IsStartFlight() ;
    bool CanFixAltiBaro() ;
    void PushData() ;
    int  GetDebutDeVol() const
            { return m_TypeDebutVol ; } ;
    void SetDebutDeVol( int dv )
            { m_TypeDebutVol = dv ; } ;
    void InitPile() ;

private :

    StPileVit m_PosArr[TAILLE_PILE_VITESSE] ;    ///< tableau des dernieres mesures
    bool      m_pile_full ;                 ///< si la pile est pleine
    int       m_ipile ;                     ///< position dans le buffer

    bool  m_HDopOk ;                ///< precision sur hdop dans la pile
    float m_MoyenneVitHor ;         ///< pour le debut de vol
    float m_MoyenneVitVert ;        ///< pour le debut de vol
    int   m_TypeDebutVol ;          ///< type de debut de vol
    int   m_CountFixAltiBaro ;      ///< nombre de recallages pour maximisation des ecrat

    unsigned long m_LastFixAltiBaro = 0 ;///< pour le dernier fix alti baro
} ;

#endif
