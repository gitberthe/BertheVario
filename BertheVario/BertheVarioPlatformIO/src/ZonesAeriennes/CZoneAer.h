////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.h
///
/// \brief
///
/// \date creation   : 23/03/2024
/// \date 26/10/2025 : tableau de st_coord_poly et plus *.
/// \date 27/10/2025 : Remplacement de m_RayonMetre par CRectIn.
/// \date 06/01/2026 : modification pour uncompress non systematique
/// \date 07/01/2026 : modification pour zone active non compresse
///

#ifndef _ZONE_AR_
#define _ZONE_AR_

#define ALTI_BASSE           0
#define ALTI_PERIODE_SEMAINE 1
#define ALTI_PERIODE_WEEKEND 2

//#define DIST_METRE_4_ZONE       50  ///< distance en metre pour le calcul/compression entre 2 points
//#define ANGLE_DEGRES_4_ZONE     6   ///< angle pour supression/compression si points alignés

// FL 195	5800 mètres
// FL 175	5332 mètres
// FL 155	4722 mètres
// FL 125	3808 mètres
// FL 115	3503 mètres
// FL 085	2589 mètres
// SFC	sol

////////////////////////////////////////////////////////////////////////////////
/// \brief Parametres de date et altitude pour une derogation ffvl de zone
class CZoneAerDerogFfvl
{
public :
    short     m_AltiBassePeriodeSemaine=-1;///< altitude basse dans la periode en semaine
    short     m_AltiBassePeriodeWeekEnd=-1;///< altitude basse dans la periode en week-end
    short     m_PeriodeDebutJour = -1 ;   ///< periode debut jour pour altitude
    short     m_PeriodeFinJour   = -1 ;   ///< periode fin jour pour altitude
    short     m_PeriodeDebutMois = -1 ;   ///< periode debut mois pour altitude
    short     m_PeriodeFinMois   = -1 ;   ///< periode fin mois pour altitude
    short     m_AltiAPrendreEnCompte = ALTI_BASSE ;   ///< altitude a prendre en compte fonction de la date
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Une zone aerienne, avec un nombre de points diminué (distance minimale
/// entre 2 points).
/// Periode semaine/week end, Active ou non. sous forme de tableau de pointeur
/// dans CZonesAerAll.
class CZoneAer : public CRectIn
{
public :
    ~CZoneAer() ;

    /// \brief pour le tri
    enum TriPar
        {
        TriParNom ,
        TriParAltitude ,
        //TriParDistance
        } ;

    // type de zone
    enum TypeZone
        {
        ZoneGeneric ,
        ZoneCorent ,
        ZoneStYan
        } ;

    static  void TrierPar( TriPar Tri )
                { ms_TriPar = Tri ; } ;
    int     GetAltiBasse() const ;
    bool    HavePeriod() const
                { return m_pDerogFfvl != NULL ; } ;
    bool    IsProtect() const
                { return m_HauteurSolZoneProtege > 0 ; } ;

    TypeZone GetTypeZone() const
                { return m_TypeZone ; } ;
    void     SetTypeZone() ;
    int      GetAltiSolZone() const ;
    CLGString GetNom4Activation() const ;
    CLGString GetNom4AffichageActivation() const ;

    char *              m_pNomAff ;                 ///< nom court de la zone a afficher
    bool                m_Activee = true ;          ///< zone activee
    bool                m_DansFchActivation = false;///< si dans fichier d'activation de zones pour reecriture apres configuration menu
    short               m_AltiBasse=-1 ;            ///< altitude basse de la zone par defaut
    CZoneAerDerogFfvl * m_pDerogFfvl = NULL ;       ///< si zone avec derogation ffvl

    void CompressZone() ;
    void UnCompressZone() ;
    short GetHauteurSolZoneProtege() const
            { return m_HauteurSolZoneProtege ; } ;
    void SetHauteurSolZoneProtege( short hauteur )
            { m_HauteurSolZoneProtege = hauteur ; } ;
    void SetPolygoneArr( st_coord_poly * pPoly , short NbPts )
            { m_PolygoneArr = pPoly ; m_NbPts = NbPts ; } ;
    st_coord_poly * GetPolygoneArr()
            { return m_PolygoneArr ; } ;
    short GetNbPts()
            { return m_NbPts ; } ;

    bool operator > ( const CZoneAer & Zone ) const ;
    bool operator < ( const CZoneAer & Zone ) const ;

private :
    st_coord_poly * m_PolygoneArr=NULL; ///< tableau des points de la zone
    short           m_NbPts = 0 ;       ///< nombre de points de la zone

    short           *m_LowResShortArr=NULL;   ///< tableau des coordonnees relatives au barycentre pour compression short

    st_coord_poly   m_Barycentre ;      ///< pour une recherche rapide

    float           m_ResolutionDegres ;            ///< resolution en metre de la zone lors de compression short
    short           m_HauteurSolZoneProtege = -1 ;  ///< hauteur de la zone protegee

    TypeZone        m_TypeZone = ZoneGeneric ;  ///< type de zone, generique, corent, st yan

    static TriPar   ms_TriPar ;  ///< pour un tri par nom/distance/altitude

    void FreeFloat() ;
    void FreeShort() ;
} ;

#endif
