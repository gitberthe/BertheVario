////////////////////////////////////////////////////////////////////////////////
/// \file CZone.h
///
/// \brief
///
/// \date 01/12/2024 : creation
/// \date 05/12/2024 : modification
///

#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \brief Zone aerienne pour tri suivant nombre de points. Pour optimization
/// memoire/compression zone BertheVario
class CZone : public CCompZoneErr
{
public :
    std::string     m_Name ;                        ///< nom de la zone
    std::string     m_NameSansBlanc ;               ///< nom de la zone sans blanc
    long            m_Bottom ;                      ///< altitude basse de la zone
    long            m_NbPtsAvantCompression = 0 ;   ///< nombre de points avant compression
    long            m_NbPtsApresCompression = 0 ;   ///< nombre de points apres compression
    double          m_ErreurMoyenneEnMetre = 0. ;   ///< erreur moyenne en metres de la zone compressee par rapport à la zone non compressee
    double          m_ErreurMaxEnMetre = 0. ;       ///< erreur maximum en metres de la zone compressee par rapport à la zone non compressee
    double          m_DeltaLatLonMax = 0. ;         ///< maximum excurtion en latitude/longitude par rapport au baricentre
    double          m_ResolVario = 0. ;             ///< Resolution en metre codee sur un short.
    std::vector<CVecZoneReduce::st_coord_poly*> m_VecPtsBig ;   ///< vecteur des points avant compression
    std::vector<CVecZoneReduce::st_coord_poly*> m_VecPtsSmall ; ///< vecteur des points apres compression

    COpenAirFile::TypeAltitudesAH_AL m_TypeAltitudeOpenAir = COpenAirFile::AucuneAltitude ;  ///< type d'altitude, AGL, AMSL
    COpenAirFile::TypeEspacesAC      m_TypeEspaceOpenAir   = COpenAirFile::AucunTypeDEspace ;///< type d'espace, quietude, A, B, C...
    COpenAirFile::TypeZonesAY        m_TypeZoneOpenAir     = COpenAirFile::AucunTypeDeZone ;///< type d'espace, quietude, A, B, C...

    static const double ms_ErreurMaxiAdmiseVerif ;  ///< erreur de verification
    static const double ms_ErreurMaxi ;             ///< erreur recherchee maxi

    void            Compress() ;

    bool operator < ( const CZone & Zone ) const ;
    bool operator > ( const CZone & Zone ) const ;
    bool operator == ( const CZone & Zone ) const ;
    bool operator != ( const CZone & Zone ) const ;

private :
    double  GetDistanceMaxMetres( const std::vector<CVecZoneReduce::st_coord_poly*> & Vec ) const ;
    double  GetResolVarioMetres() const ;
    double  GetDeltaLatLonMaxKm( const std::vector<CVecZoneReduce::st_coord_poly*> & Vec ) const ;
} ;

bool MySortFonction( const CZone * pZA , const CZone * pZB ) ;
