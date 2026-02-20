////////////////////////////////////////////////////////////////////////////////
/// \file CPolygone.h
///
/// \brief
///
/// \date creation   : 23/03/2024
/// \date 26/10/2025 : tableau de st_coord_poly et plus *.
/// \date 27/10/2025 : Remplacement de m_RayonMetre par CRectIn.
///

#ifndef _POLYGONE_
#define _POLYGONE_

////////////////////////////////////////////////////////////////////////////////
/// \brief Definition d'un polygone par points et test si on est a l'interieur.
/// Donne aussi la surface pour les zones imbriquées.
class CPolygone
{
public :
    static void  CalcBarycentre( const CZoneAer::st_coord_poly * PtsArr , int NbPts , CZoneAer::st_coord_poly & PtsBarycentre ) ;
    static float GetAreaSize( const CZoneAer::st_coord_poly * PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsBarycentre ) ;
    static bool  IsIn( const CZoneAer::st_coord_poly * PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsToTest ) ;
    //static bool IsIn( CZoneAer::st_coord_poly ** PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsToTest ) ;
    static bool  IsInOld( const CZoneAer::st_coord_poly * PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsToTest ) ;
} ;

#endif
