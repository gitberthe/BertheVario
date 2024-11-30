////////////////////////////////////////////////////////////////////////////////
/// \file CCompZoneErr.cpp
///
/// \brief
///
/// \date creation     : 30/11/2024
/// \date modification : 30/11/2024
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
    for ( long ipsz = 0 ; ipsz < (long)SmallZone.size() ; ipsz++ )
        {
        CVecZoneReduce::st_coord_poly* ptdeb=SmallZone[0] ;
        CVecZoneReduce::st_coord_poly* ptfin=SmallZone[((long)SmallZone.size())-1] ;
        if ( ipsz < ((long)SmallZone.size()-1) )
            {
            ptdeb = SmallZone[ipsz] ;
            ptfin = SmallZone[ipsz+1] ;
            }

        double distance = GetErrMetres(ptdeb,ptfin,pPBz) ;

        if ( distance_min > distance )
            distance_min = distance ;
        }

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
// point A
if ( pPt->m_Lat == pPtLineA->m_Lat && pPt->m_Lon == pPtLineA->m_Lon )
    return 0. ;
// point B
if ( pPt->m_Lat == pPtLineB->m_Lat && pPt->m_Lon == pPtLineB->m_Lon )
    return 0. ;
// pas de droite
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

// si en dehors du segment de droite
CVecteur2D VecPtSurDroite( PtA , PtProj ) ;
double DistanceDroite = DBL_MAX ;
if ( VecPtSurDroite.GetNorm() > VecDroite.GetNorm() || VecPtSurDroite.GetAngleDeg( VecDroite ) > 90 )
    {
    CVecteur2D VecProj( Pt , PtProj ) ;
    DistanceDroite = VecProj.GetNorm() * MilesParDegres * UnMileEnMetres ;
    }

// distance à points A et B
double DistanceA = Pt.GetDistanceAuPoint(PtA) * MilesParDegres * UnMileEnMetres ;
double DistanceB = Pt.GetDistanceAuPoint(PtB) * MilesParDegres * UnMileEnMetres ;

// distance min
double DistanceMin = MIN( DistanceA , DistanceB ) ;
DistanceMin = MIN( DistanceMin , DistanceDroite ) ;
return DistanceMin ;
}
