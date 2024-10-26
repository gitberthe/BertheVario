////////////////////////////////////////////////////////////////////////////////
/// \file CDistFront.h
///
/// \brief
///
/// \date creation     : 28/03/2024
/// \date modification : 28/03/2024
///

#ifndef _DISTFRONT_
#define _DISTFRONT_
////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de determiner si on est pres de la frontiere d'une zone.
/// Interieur/exterieur, distance la plus courte d'un de ses points de definition
/// ou d'une de ses droites.
class CDistFront
{
public :
    bool IsNearFront( CZoneAer::st_coord_poly ** PolygoneArr , int NbPts, CZoneAer::st_coord_poly PtEnCours ) ;
} ;

#endif
