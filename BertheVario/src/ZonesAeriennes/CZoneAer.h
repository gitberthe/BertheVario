////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.h
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 21/07/2024
///

#ifndef _ZONE_AR_
#define _ZONE_AR_

#define DIST_METRE_4_ZONE  100

#define ALTI_BASSE           0
#define ALTI_PERIODE_SEMAINE 1
#define ALTI_PERIODE_WEEKEND 2

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
    int     m_AltiBassePeriodeSemaine=-1;///< altitude basse dans la periode en semaine
    int     m_AltiBassePeriodeWeekEnd=-1;///< altitude basse dans la periode en week-end
    int     m_PeriodeDebutJour = -1 ;   ///< periode debut jour pour altitude
    int     m_PeriodeFinJour   = -1 ;   ///< periode fin jour pour altitude
    int     m_PeriodeDebutMois = -1 ;   ///< periode debut mois pour altitude
    int     m_PeriodeFinMois   = -1 ;   ///< periode fin mois pour altitude
    int     m_AltiAPrendreEnCompte = ALTI_BASSE ;   ///< altitude a prendre en compte fonction de la date
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Une zone aerienne, avec un nombre de points diminué (distance minimale
/// entre 2 points).
/// Periode semaine/week end, Active ou non. sous forme de tableau de pointeur
/// dans CZonesAerAll.
class CZoneAer
{
public :
    ~CZoneAer() ;

    /// \brief un point du polygone
    struct st_coord_poly
        {
        float m_Lat ;   ///< latitude en degres
        float m_Lon ;   ///< longitude en degres
        } ;

    static  void TriParNom( bool Tri )
                { ms_TriParNom = Tri ; } ;
    int     GetAltiBasse() const ;
    bool    HavePeriod() const
                { return m_pDerogFfvl != NULL ; } ;
    std::string  m_NomAff ;             ///< nom court de la zone a afficher
    std::string  m_NomOri ;             ///< nom entier de la zone dans fichier origine
    bool    m_Activee = true ;          ///< zone activee
    bool    m_DansFchActivation = false;///< si dans fichier d'activation de zones pour reecriture apres configuration menu
    int     m_AltiBasse=-1 ;            ///< altitude basse de la zone par defaut
    CZoneAerDerogFfvl * m_pDerogFfvl = NULL ; ///< si zone avec derogation ffvl

    friend class CZonesAerAll ;

    bool operator > ( const CZoneAer & Zone ) const ;
    bool operator < ( const CZoneAer & Zone ) const ;

private :
    st_coord_poly **m_PolygoneArr=NULL; ///< tableau des points de la zone
    int             m_NbPts = 0 ;       ///< nombre de points de la zone
    float           m_Area = 0. ;       ///< surface pour les zones imbriquee ex : TAM 2.2

    st_coord_poly   m_Barycentre ;      ///< pour une recherche rapide
    float           m_RayonMetre ;      ///< pour une recherche rapide

    static bool ms_TriParNom ;  ///< pour un tri par nom
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de reduction du nombre de points par suppression de ceux plus
/// proche du barycentre.
class CVecReduce
{
public :
    void Set( std::vector<CZoneAer::st_coord_poly*> & VecToReduce )
        { m_pVecOrigine = & VecToReduce ; } ;
    void ReduceTo( int DistanceMetresEntrePoints ) ;

private :
    std::vector<CZoneAer::st_coord_poly*> * m_pVecOrigine ;

    /*struct st_pts_i_b
        {
        int   m_ipts ;          ///< position vecteur origine
        float m_DistanceBari ;  ///< distance baricentre

        bool operator > ( const st_pts_i_b & pts ) const
            { return m_DistanceBari > pts.m_DistanceBari ; } ;
        bool operator < ( const st_pts_i_b & pts ) const
            { return m_DistanceBari < pts.m_DistanceBari  ; } ;
        } ; */
} ;

#endif
