////////////////////////////////////////////////////////////////////////////////
/// \file CDistFront.h
///
/// \brief
///
/// \date creation     : 28/03/2024
/// \date modification : 08/02/2025
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
    int m_DistXYNextZone = 9999 ;  ///< distance de la prochaine zone
    int m_DistAltCurZone = 9999 ;  ///< hauteur restante du plafond prochaine zone
    CZoneAer::st_coord_poly m_PtFrontProche ;///< point de la frontiere proche

    float IsNearFront( CZoneAer::st_coord_poly ** PolygoneArr , int NbPts, CZoneAer::st_coord_poly PtEnCours , CZoneAer::st_coord_poly & PtFrontProche ) ;
} ;

#endif
