////////////////////////////////////////////////////////////////////////////////
/// \file CDistFront.cpp
///
/// \brief
///
/// \date creation     : 28/03/2024
/// \date modification : 09/02/2025
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie true si on est proche de la frontiere.
float CDistFront::IsNearFront( CZoneAer::st_coord_poly ** PolygoneArr , int NbPts , CZoneAer::st_coord_poly PtEnCours ,
                                int & CapFrontProche )
{
CZoneAer::st_coord_poly PtFrontProche ;
float DistanceMin = 9E20 ;

// calcul distance de tous les points
for ( int ipts = 0 ; ipts < NbPts ; ipts++ )
    {
    const CZoneAer::st_coord_poly & pts = *PolygoneArr[ipts] ;
    float dist = sqrtf( powf(pts.m_Lat-PtEnCours.m_Lat,2) + powf(pts.m_Lon-PtEnCours.m_Lon,2) ) ;
    dist *= 60 * UnMileEnMetres ;
    // distance minimale et point
    if ( DistanceMin > dist )
        {
        DistanceMin = dist ;
        PtFrontProche = pts ;
        }
    }

// pour toutes les droites des points 2 a 2
for ( int ipts = 0 ; ipts < NbPts ; ipts++ )
    {
    CZoneAer::st_coord_poly PosA ;
    CZoneAer::st_coord_poly PosB ;
    if ( ipts != NbPts - 1 )
        {
        PosA = *PolygoneArr[ipts] ;
        PosB = *PolygoneArr[ipts+1] ;
        }
    else
        {
        PosA = *PolygoneArr[ipts] ;
        PosB = *PolygoneArr[0] ;
        }

    // les droites et points
    CPoint2D PtA( PosA.m_Lon , PosA.m_Lat ) ;
    CPoint2D PtB( PosB.m_Lon , PosB.m_Lat ) ;
    CPoint2D PtC( PtEnCours.m_Lon , PtEnCours.m_Lat ) ;
    CVecteur2D VecDir( PtA , PtB ) ;
    CDroite2D Droite( VecDir , PtA ) ;

    CPoint2D PtProj = Droite.GetProjectionDuPoint( PtC ) ;
    CVecteur2D VecProjDroite( PtA , PtProj ) ;
    CVecteur2D VecPerDroite( PtProj , PtC ) ;

    // si point projete est dans le segment de droite (norme plus petite et colineaire)
    if ( VecProjDroite.GetNorm() <= VecDir.GetNorm() && VecProjDroite.GetAngleDeg( VecDir ) < 90. )
        {
        float dist = VecPerDroite.GetNorm() * 60. * UnMileEnMetres ;
        // distance minimale et point
        if ( DistanceMin > dist )
            {
            DistanceMin = dist ;
            PtFrontProche.m_Lon = PtProj.m_x ;
            PtFrontProche.m_Lat = PtProj.m_y ;
            }
        }
    }

// determination cap pint frontiere proche
CapFrontProche = atan2f( PtFrontProche.m_Lat - PtEnCours.m_Lat ,
                      PtFrontProche.m_Lon - PtEnCours.m_Lon ) *
                              -180. / PI + 90 + 360 ;
CapFrontProche %= 360 ;

return DistanceMin ;
}
