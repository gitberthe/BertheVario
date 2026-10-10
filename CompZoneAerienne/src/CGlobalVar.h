////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.h
///
/// \brief
///
/// \date creation     : 10/10/2026
/// \date modification : 10/10/2026
///

#pragma once

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

    double m_DelatLatLonMaxZoneKm = 0 ;
    double m_ResolutionMoyenneMetre = 0 ;
    double m_ResolutionMaxAllMetres = 0. ;
    double m_ErreurMaxiMetres = 0. ;
    double m_ErreurMoyenneMetres = 0. ;

    bool m_OpenAir = false ;    ///< si openair file

    std::string                 m_path  ;               ///< repertoire courant
    std::vector<CCmdLineParam>  m_VectPtCentreRayon ;   ///< vecteur des differents points centre/rayon de recherche de zone
    std::vector< CZone* >       m_VecZone ;             ///< vecteur des zones


private :
    bool VerificationZoneAdmissible( const CZone * pZone, double LatCentreDeg, double LonCentreDeg, double RayonDeg ) ;
    void split(const std::string &chaine, std::vector<std::string> &elements) ;

} ;
