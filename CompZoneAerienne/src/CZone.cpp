////////////////////////////////////////////////////////////////////////////////
/// \file CZone.cpp
///
/// \brief
///
/// \date 01/12/2024 : creation
/// \date 01/12/2024 : modification
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief compression d'une zone
void CZone::Compress()
{
CVecZoneReduce VecZoneReduce ;
m_NbPtsAvantCompression = m_VecPtsBig.size() ;

// recopie de zone pour comparaison
m_VecPtsSmall = m_VecPtsBig ;
for ( size_t ip = 0 ; ip < m_VecPtsBig.size() ; ip++ )
    m_VecPtsSmall[ip] = new CVecZoneReduce::st_coord_poly( *m_VecPtsBig[ip] ) ;

// compression de points
VecZoneReduce.Set( m_VecPtsSmall ) ;
//VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( DIST_METRE_DROITE , -1 , DIST_METRE_PTS ) ;
VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( DIST_METRE_DROITE , -1 , -1 ) ;
//VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( -1 , ANGLE_DEGRES , -1 ) ;
//VecZoneReduce.ReduceMultiDistanceDroite( DIST_METRE_MULTI_DROITE ) ;
//VecZoneReduce.ReduceNuageDroite( DIST_METRE_NUAGE_DROITE ) ;
//VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( -1 , ANGLE_DEGRES , DIST_METRE_PTS ) ;
//VecZoneReduce.ReduceNuageBravaisPearson( DIST_METRE_NUAGE_DROITE_BP , COEF_BRAVAIS_PEARSON ) ;

m_NbPtsApresCompression = m_VecPtsSmall.size() ;

// erreur moyenne
CCompZoneErr CompZoneErr ;
m_ErreurMoyenneEnMetre = CompZoneErr.GetErrMetres( m_VecPtsBig , m_VecPtsSmall ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne le rayon max de la zone autour du barycentre
double CZone::GetRayonMaxKm() const
{
double LatBary = 0 ;
double LonBary = 0 ;
for ( size_t ip = 0 ; ip < m_VecPtsSmall.size() ; ip++ )
    {
    const CVecZoneReduce::st_coord_poly* pSt = m_VecPtsSmall[ip] ;
    LatBary += pSt->m_Lat ;
    LonBary += pSt->m_Lon ;
    }
LatBary /= m_VecPtsSmall.size() ;
LonBary /= m_VecPtsSmall.size() ;

double RayonMaxDeg = 0 ;
for ( size_t ip = 0 ; ip < m_VecPtsSmall.size() ; ip++ )
    {
    const CVecZoneReduce::st_coord_poly* pSt = m_VecPtsSmall[ip] ;
    double RayonDeg = sqrt( pow( LatBary - pSt->m_Lat , 2. ) + pow( LonBary - pSt->m_Lon , 2. ) ) ;
    RayonMaxDeg = MAX( RayonMaxDeg , RayonDeg ) ;
    }
return RayonMaxDeg * MilesParDegres * UnMilesEnMetres / 1000. ;
}
