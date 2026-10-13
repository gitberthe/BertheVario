////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.h
///
/// \brief
///
/// \date creation     : 10/10/2026
/// \date modification : 10/10/2026
///

#pragma once

#ifdef _COMP_ZONES_AERIENNES_
 #include "../../BertheVario/BertheVarioPlatformIO/src/LibGeneral/LibGeneral.h"
 #include "../../BertheVario/BertheVarioPlatformIO/src/Gps/CHgt2Agl.h"
#endif // _COMP_ZONES_AERIENNES_

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour validation zone
class CTerrain
{
public :
    float m_Lat = 0. ;          ///< longitude E+, O- en degres
    float m_Lon = 0. ;          ///< longitude N+, S- en degres
    float m_AltiBaroRec = 0. ;  ///< altitude barometrique en metres recalee par fichier hgt
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief parametre de la ligne de commande (clermont)
class CCmdLineParam
{
public :
    double m_LatCentreDeg = 45.783329 ; ///< centre clermont
    double m_LonCentreDeg =  3.08333 ;  ///< centre clermont
    double m_RayonDeg     = 120. * 1000. / UnMilesEnMetres / MilesParDegres ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief classe de variable deu projet
class CGlobalVar
{
public :
    long TraiteLocaliteAutour( std::vector<std::string> &LigneDeCommande ) ;
    long TraiteFichierJson() ;
    long TraiteRepertoireOpenAir() ;
    long EcritureFichierZones() ;
    long EcritureFichiersGnuPlot() ;
    long EcritureFichiersKml() ;

    #ifdef _COMP_ZONES_AERIENNES_
     long ValidCompZoneAeriennes( const char * zoneaer_txt , const char * valin_txt ) ;
    #endif

    double m_DelatLatLonMaxZoneKm = 0 ;
    double m_ResolutionMoyenneMetre = 0 ;
    double m_ResolutionMaxAllMetres = 0. ;
    double m_ErreurMaxiMetres = 0. ;
    double m_ErreurMoyenneMetres = 0. ;

    bool m_OpenAir = false ;    ///< si openair file

    std::string                 m_path  ;               ///< repertoire courant
    std::vector<CCmdLineParam>  m_VectPtCentreRayon ;   ///< vecteur des differents points centre/rayon de recherche de zone
    std::vector< CZone* >       m_VecZones ;             ///< vecteur des zones

    // pour validation zone
    int      m_Annee ;
    int      m_Mois ;
    int      m_Jour ;
    CTerrain m_TerrainPosCur ;
    float    m_AltitudeSolHgt = 0 ;///< altitude du sol hgt
    CHgt2Agl m_Hgt2Agl ; ///< pour determiner la hauteur sol

private :
    bool VerificationZoneAdmissible( const CZone * pZone, double LatCentreDeg, double LonCentreDeg, double RayonDeg ) ;
    void split(const std::string &chaine, std::vector<std::string> &elements) ;

} ;
