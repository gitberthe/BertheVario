////////////////////////////////////////////////////////////////////////////////
/// \file CZone.cpp
///
/// \brief
///
/// \date 01/12/2024 : creation
/// \date 05/12/2024 : modification
///

#include "CompZoneAerienne.h"

const double CZone::ms_ErreurMaxiAdmiseVerif = 50. ;
const double CZone::ms_ErreurMaxi = 15. ;

////////////////////////////////////////////////////////////////////////////////
/// \brief
bool CZone::operator < ( const CZone & Zone ) const
{
long DeltaThis = m_VecPtsBig.size() - m_VecPtsSmall.size() ;
long DeltaZone = Zone.m_VecPtsBig.size() - Zone.m_VecPtsSmall.size() ;
return DeltaThis < DeltaZone ;

//return m_VecPtsSmall.size() < Zone.m_VecPtsSmall.size() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
bool CZone::operator > ( const CZone & Zone ) const
{
long DeltaThis = m_VecPtsBig.size() - m_VecPtsSmall.size() ;
long DeltaZone = Zone.m_VecPtsBig.size() - Zone.m_VecPtsSmall.size() ;
return DeltaThis > DeltaZone ;

//return m_VecPtsSmall.size() > Zone.m_VecPtsSmall.size() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
bool CZone::operator == ( const CZone & Zone ) const
{
return ! ( (*this) != Zone ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour le tri suivant le nombre de points compresse
bool MySortFonction( const CZone * pZA , const CZone * pZB )
{
return *pZA > *pZB ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief compression d'une zone
void CZone::Compress()
{
CVecZoneReduce VecZoneReduce ;
m_NbPtsAvantCompression = m_VecPtsBig.size() ;
//double DistanceAUneDroiteMetres = 4.5 ;
double DistanceAUneDroiteMetres = ms_ErreurMaxiAdmiseVerif ;
//double DistanceEntrePoint = 8 ;
//double AngleMaxDegres = 1. ;
//DistanceAUneDroiteMetres = GetDistanceMaxMetres( m_VecPtsBig ) / 1000 ;
m_DeltaLatLonMax = GetDeltaLatLonMaxKm( m_VecPtsBig ) ;

do {
    // recopie de zone pour comparaison
    m_VecPtsSmall = m_VecPtsBig ;
    for ( size_t ip = 0 ; ip < m_VecPtsBig.size() ; ip++ )
        m_VecPtsSmall[ip] = new CVecZoneReduce::st_coord_poly( *m_VecPtsBig[ip] ) ;

    // compression de points
    VecZoneReduce.Set( m_VecPtsSmall ) ;
    VecZoneReduce.ReduceDistanceDroiteBigSmall( DistanceAUneDroiteMetres ) ;

    m_NbPtsApresCompression = m_VecPtsSmall.size() ;

    // erreur moyenne et max
    m_ErreurMoyenneEnMetre = GetErrMetres( m_VecPtsBig , m_VecPtsSmall , m_ErreurMaxEnMetre ) ;

    // pour convergence
    DistanceAUneDroiteMetres *= 0.9 ;

    // tan que erreur max > 15 metres ou > a m_DeltaLatLonMax pour les petites zones
    } while ( m_ErreurMaxEnMetre > ms_ErreurMaxi || m_ErreurMaxEnMetre > m_DeltaLatLonMax * 3 ) ;

/*VecZoneReduce.Set( m_VecPtsSmall ) ;
VecZoneReduce.ReduceDistanceInPolygon( DistanceAUneDroiteMetres ) ;
m_ErreurMoyenneEnMetre = GetErrMetres( m_VecPtsBig , m_VecPtsSmall , m_ErreurMaxEnMetre ) ;
m_NbPtsApresCompression = m_VecPtsSmall.size() ; / */

m_DeltaLatLonMax = GetDeltaLatLonMaxKm( m_VecPtsSmall ) ;
m_ResolVario = GetResolVarioMetres() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne la distance maximum lat/lon par raport au barycentre
double CZone::GetDistanceMaxMetres( const std::vector<CVecZoneReduce::st_coord_poly*> & Vec ) const
{
// calcul du barycentre
double LatBary = 0 ;
double LonBary = 0 ;
for ( size_t ip = 0 ; ip < Vec.size() ; ip++ )
    {
    const CVecZoneReduce::st_coord_poly* pSt = Vec[ip] ;
    LatBary += pSt->m_Lat ;
    LonBary += pSt->m_Lon ;
    }
LatBary /= Vec.size() ;
LonBary /= Vec.size() ;

double DistMaxLL = -1 ;
for ( size_t ip = 0 ; ip < Vec.size() ; ip++ )
    {
    const CVecZoneReduce::st_coord_poly* pSt = Vec[ip] ;
    float DistLL = fabsf( LatBary - pSt->m_Lat ) ;
    if ( DistLL > DistMaxLL )
        DistMaxLL = DistLL ;
    DistLL = fabsf( LonBary - pSt->m_Lon ) ;
    if ( DistLL > DistMaxLL )
        DistMaxLL = DistLL ;
    }

return DistMaxLL * 60. * UnMilesEnMetres ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
double CZone::GetResolVarioMetres() const
{
// short maximum en float
const double ShortMax = 32767. ;

// calcul de la resolution minimum
double DistMaxLatLonZoneMetres = GetDistanceMaxMetres( m_VecPtsSmall ) ;
double ResolutionMetres = 1.05 * DistMaxLatLonZoneMetres / ShortMax ;
return ResolutionMetres ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne le rayon max de la zone autour du barycentre
double CZone::GetDeltaLatLonMaxKm( const std::vector<CVecZoneReduce::st_coord_poly*> & Vec ) const
{
// calcul du barycentre
double LatBary = 0 ;
double LonBary = 0 ;
for ( size_t ip = 0 ; ip < Vec.size() ; ip++ )
    {
    const CVecZoneReduce::st_coord_poly* pSt = Vec[ip] ;
    LatBary += pSt->m_Lat ;
    LonBary += pSt->m_Lon ;
    }
LatBary /= Vec.size() ;
LonBary /= Vec.size() ;

double DelataMaxDeg = 0 ;
for ( size_t ip = 0 ; ip < Vec.size() ; ip++ )
    {
    const CVecZoneReduce::st_coord_poly* pSt = Vec[ip] ;
    DelataMaxDeg = MAX( DelataMaxDeg , abs( LatBary - pSt->m_Lat ) ) ;
    DelataMaxDeg = MAX( DelataMaxDeg , abs( LonBary - pSt->m_Lon ) ) ;
    }
return DelataMaxDeg * MilesParDegres * UnMilesEnMetres / 1000. ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie si une zone non compressee est strictement differente en nom
/// et dimension
bool CZone::operator != ( const CZone & Zone ) const
{
// pour toutes les zones deja enregistrées
if ( m_Name != Zone.m_Name )
    return true ;

// comparaison des tailles de vecteur
const std::vector<CVecZoneReduce::st_coord_poly*> & VecOldZone = m_VecPtsBig ;
const std::vector<CVecZoneReduce::st_coord_poly*> & VecNewZone = Zone.m_VecPtsBig ;
if ( VecNewZone.size() != VecOldZone.size() )
    return true ;

// comparaison des points
for ( size_t ipt = 0 ; ipt < VecOldZone.size() ; ipt++ )
    if ( (VecOldZone[ipt]->m_Lat != VecNewZone[ipt]->m_Lat) ||
         (VecOldZone[ipt]->m_Lon != VecNewZone[ipt]->m_Lon))
        return true ;

return false ;
}
