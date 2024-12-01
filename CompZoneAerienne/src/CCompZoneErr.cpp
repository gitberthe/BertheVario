////////////////////////////////////////////////////////////////////////////////
/// \file CCompZoneErr.cpp
///
/// \brief
///
/// \date creation     : 30/11/2024
/// \date modification : 01/12/2024
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'erreur moyenne en metre entre les 2 courbes.
double CCompZoneErr::GetErrMetres( const std::vector<CVecZoneReduce::st_coord_poly*> & BigZone , const std::vector<CVecZoneReduce::st_coord_poly*> & SmallZone )
{
double ErrMetre = 0 ;
long   NbSect = 0 ;
// pour tous les points de la grosse zone
for ( long ipbz = 0 ; ipbz < (long)BigZone.size() ; ipbz++ )
    {
    const CVecZoneReduce::st_coord_poly* pPBz = BigZone[ipbz] ;
    double distance_min = DBL_MAX ;
    // distance à la petite zone
    for ( long ipsz = 0 ; ipsz < (long)SmallZone.size()-1 ; ipsz++ )
        {
        CVecZoneReduce::st_coord_poly* ptdeb=SmallZone[ipsz] ;
        CVecZoneReduce::st_coord_poly* ptfin=SmallZone[ipsz+1] ;
        distance_min = MIN ( distance_min , GetErrMetres(ptdeb,ptfin,pPBz) ) ;
        }
    // dernier segment
    distance_min = MIN ( distance_min , GetErrMetres( SmallZone[0] ,SmallZone[(long)SmallZone.size()-1], pPBz) ) ;

    // ajout de la distance
    ErrMetre += distance_min ;
    NbSect++ ;
    }
return ErrMetre/NbSect ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la distance du point à la droite en metre ou si le point
/// projete n'est pas entre les points A/B, distance A ou B
double CCompZoneErr::GetErrMetres( const CVecZoneReduce::st_coord_poly* pPtLineA , const CVecZoneReduce::st_coord_poly* pPtLineB , const CVecZoneReduce::st_coord_poly* pPt )
{
// pt == point A
if ( pPt->m_Lat == pPtLineA->m_Lat && pPt->m_Lon == pPtLineA->m_Lon )
    return 0. ;
// pt == point B
if ( pPt->m_Lat == pPtLineB->m_Lat && pPt->m_Lon == pPtLineB->m_Lon )
    return 0. ;
// pas de droite ptA == ptb
if ( pPtLineA->m_Lat == pPtLineB->m_Lat && pPtLineA->m_Lon == pPtLineB->m_Lon )
    {
    CPoint2D Pt( pPt->m_Lon , pPt->m_Lat );
    CPoint2D PtA( pPtLineA->m_Lon , pPtLineA->m_Lat );
    double Distance = Pt.GetDistanceAuPoint(PtA) * MilesParDegres * UnMileEnMetres ;
    return Distance ;
    }

// points
CPoint2D Pt( pPt->m_Lon , pPt->m_Lat );
CPoint2D PtA( pPtLineA->m_Lon , pPtLineA->m_Lat );
CPoint2D PtB( pPtLineB->m_Lon , pPtLineB->m_Lat );
// vecteur droite
CVecteur2D VecDroite(PtA,PtB) ;
CDroite2D Droite( VecDroite , PtA ) ;
// point projete
CPoint2D PtProj = Droite.GetProjectionDuPoint(Pt) ;

// si dans le segment de droite
CVecteur2D VecPtSurDroite( PtA , PtProj ) ;
double DistanceDroite = DBL_MAX ;
if ( VecDroite.GetNorm() >= VecPtSurDroite.GetNorm() && VecPtSurDroite.GetAngleDeg( VecDroite ) < 10 )
    DistanceDroite = Pt.GetDistanceAuPoint(PtProj) * MilesParDegres * UnMileEnMetres ;

// distance à points A et B
double DistanceA = Pt.GetDistanceAuPoint(PtA) * MilesParDegres * UnMileEnMetres ;
double DistanceB = Pt.GetDistanceAuPoint(PtB) * MilesParDegres * UnMileEnMetres ;

// distance min
double DistanceMinPt = MIN( DistanceA , DistanceB ) ;
double DistanceMin = MIN( DistanceMinPt , DistanceDroite ) ;
return DistanceMin ;
}
