////////////////////////////////////////////////////////////////////////////////
/// \file CDistFront.cpp
///
/// \brief
///
/// \date creation     : 28/03/2024
/// \date modification : 10/04/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie true si on est proche de la frontiere.
bool CDistFront::IsNearFront( CZoneAer::st_coord_poly ** PolygoneArr , int NbPts , CZoneAer::st_coord_poly PtEnCours )
{

// verification distance de tous les points
for ( int ipts = 0 ; ipts < NbPts ; ipts++ )
    {
    const CZoneAer::st_coord_poly & pts = *PolygoneArr[ipts] ;
    float dist = sqrtf( powf(pts.m_Lat-PtEnCours.m_Lat,2) + powf(pts.m_Lon-PtEnCours.m_Lon,2) ) ;
    dist *= 60 * UnMileEnMetres ;
    if ( dist <= g_GlobalVar.m_Config.m_XYMargin )
        {
        //Serial.println("ici") ;
        return true ;
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

    // si point projete est dans le segment de droite (norme plus petite)
    if ( VecProjDroite.GetNorm() <= VecDir.GetNorm() )
        {
        // si distance au point
        if ( (VecPerDroite.GetNorm() * 60. * UnMileEnMetres) <= g_GlobalVar.m_Config.m_XYMargin )
            {
            //Serial.println("la") ;
            return true ;
            }
        }
    }

return false ;
}
