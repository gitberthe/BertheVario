////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.h
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 26/11/2024
///

#ifndef _ZONE_AR_
#define _ZONE_AR_

#define ALTI_BASSE           0
#define ALTI_PERIODE_SEMAINE 1
#define ALTI_PERIODE_WEEKEND 2

#define DIST_METRE_4_ZONE       50  ///< distance en metre pour le calcul/compression entre 2 points
#define ANGLE_DEGRES_4_ZONE     6   ///< angle pour supression/compression si points alignés

#define ResolCompress        (18.)

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

    std::string         m_NomAff ;             ///< nom court de la zone a afficher
    std::string         m_NomOri ;             ///< nom entier de la zone dans fichier origine
    bool                m_Activee = true ;          ///< zone activee
    bool                m_DansFchActivation = false;///< si dans fichier d'activation de zones pour reecriture apres configuration menu
    short               m_AltiBasse=-1 ;            ///< altitude basse de la zone par defaut
    CZoneAerDerogFfvl * m_pDerogFfvl = NULL ; ///< si zone avec derogation ffvl

    void CompressZone() ;
    void UnCompressZone() ;
    void FreeFloat() ;

    friend class CZonesAerAll ;

    bool operator > ( const CZoneAer & Zone ) const ;
    bool operator < ( const CZoneAer & Zone ) const ;

private :
    st_coord_poly **m_PolygoneArr=NULL; ///< tableau des points de la zone
    short           m_NbPts = 0 ;       ///< nombre de points de la zone

    short           *m_LowResShortArr=NULL;   ///< tableau des coordonnees relatives au barycentre pour compression short

    st_coord_poly   m_Barycentre ;      ///< pour une recherche rapide
    float           m_RayonMetre ;      ///< pour une recherche rapide

    static bool ms_TriParNom ;  ///< pour un tri par nom
} ;

/*////////////////////////////////////////////////////////////////////////////////
/// \brief /// \brief Classe de reduction du nombre de points par suppression de ceux qui
/// sont espacés de 50 metres ou qui sont en ligne droite de 6°.
class CVecReduce
{
public :
    void Set( std::vector<CZoneAer::st_coord_poly*> & VecToReduce )
        { m_pVecOrigine = & VecToReduce ; } ;
    void ReduceToDistance( int DistanceMetresEntrePoints ) ;
    void ReduceToAngle( int MemeDirectionEnDegres ) ;

private :
    std::vector<CZoneAer::st_coord_poly*> * m_pVecOrigine ;
} ;*/

#endif