////////////////////////////////////////////////////////////////////////////////
/// \file CPolygone.h
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 28/09/2024
///

#ifndef _POLYGONE_
#define _POLYGONE_

////////////////////////////////////////////////////////////////////////////////
/// \brief Definition d'un polygone par points et test si on est a l'interieur.
/// Donne aussi la surface pour les zones imbriquées.
class CPolygone
{
public :
    static void CalcBarycentre( CZoneAer::st_coord_poly ** PtsArr , int NbPts , CZoneAer::st_coord_poly & PtsBarycentre ) ;
    static float GetAreaSize( CZoneAer::st_coord_poly ** PtsArr , int NbPts , CZoneAer::st_coord_poly & PtsBarycentre ) ;
    static bool IsIn( CZoneAer::st_coord_poly ** PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsToTest ) ;
    static bool IsInOld( CZoneAer::st_coord_poly ** PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsToTest ) ;
} ;

#endif
