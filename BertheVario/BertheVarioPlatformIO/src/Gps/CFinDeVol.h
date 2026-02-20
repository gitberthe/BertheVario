////////////////////////////////////////////////////////////////////////////////
/// \file CFinDeVol.h
///
/// \brief
///
/// \date creation   : 24/08/2024
/// \date 22/10/2025 : DISTANCE_LOIN_XY_METRE passe a 25m avec le HDOP
/// \date 23/10/2025 : suppression m_ADejaEteLointOuHaut
/// \date 30/12/2025 : STATIONNARITE_XY_METRE de 15 a 7
/// \date 02/01/2026 : stationnarite en metre fonction de m_vitesse_igc_kmh
///

#ifndef _FINDEVOL_
#define _FINDEVOL_

#ifdef STACK_DEBUG
 #define TAILLE_PILE_FE              (90)     ///< taille en secondes pile de calcul
#else
 #define TAILLE_PILE_FE              (5)     ///< taille en secondes pile de calcul
#endif

#define HAUTEUR_LOIN_DECO_METRE     (4.)    ///< altitude pour toujours en vol
#define HAUTEUR_LOIN_SOL_METRE_DECO   (20.)   ///< hauteur pour decollage
#define HAUTEUR_LOIN_SOL_METRE_ATTERO (45.)   ///< hauteur pour attero

#define VITESSE_FAIBLE_HOR_KMH      (6.)                         ///< pour toujours en vol
#define STATIONNARITE_Z_METRE       (3.)                         ///< pour fin de vol

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine une fin de vol.
class CFinDeVol
{
public :
    CFinDeVol() ;
    void PushPos4FlihgtEnd() ;
    void InitFinDeVol() ;
    bool IsFlightEnd() ;
    bool IsInFlight() const ;
    bool IsAtGround() const ;
    static float GetDistanceMetres( float x , float y ) ;
    static float GetDistanceLatLon( float lat , float lon ) ;

private :
    /// \brief structure pour stationnarite
    struct st_pos
        {
        float m_Lat  = 90. ;
        float m_Lon  = 180. ;
        float m_Alti = 9999. ;
        float m_VitHor  = 40. ;
        float m_VitVer  = 10. ;
        float m_HDop = 100. ;
        } ;

    bool IsHDopOk() const ;  ///< hdop stable et fixe en dessous de HDOP_BON 3.0

    struct st_pos m_PosArr[TAILLE_PILE_FE] ;  ///< tableau des dernieres mesures
    bool          m_pile_full = false ;       ///< si la pile est pleine
    int           m_ipile = 0 ;

    bool          m_AuDessusDuDeco ;        ///< pour inhibition des boutons
    bool          m_LoinDuSol ;
    bool          m_FaibleVitesseHor ;
    bool          m_FaibleVitesseVert ;
} ;

#endif
