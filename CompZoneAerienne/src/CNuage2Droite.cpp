////////////////////////////////////////////////////////////////////////////////
/// \file CNuage2Droite.cpp
///
/// \brief
///
/// \date creation     : 28/11/2024
/// \date modification : 28/11/2024
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne m_ax et m_b fonction du nuage de ppints.
/// Renvoie une droite 2d.
CDroite2D CNuage2Droite::ApproxDroite( const std::vector<CVecZoneReduce::st_coord_poly*> & VecNuage )
{
double MoyenneLat = 0 ;
double MoyenneLon = 0 ;
const long size = VecNuage.size() ;

// croite par defaut si un point
if ( size <= 1 )
    return CDroite2D() ;

// calcul moyenne lat et lon
for ( long i = 0 ; i  < size ; i++ )
    {
    const CVecZoneReduce::st_coord_poly* pStruct = VecNuage[i] ;
    MoyenneLat += pStruct->m_Lat ;
    MoyenneLon += pStruct->m_Lon ;
    }
MoyenneLat /= size ;
MoyenneLon /= size ;

double Sx = 0 ;
double Sy = 0 ;
double Sxy = 0 ;
double Sx2 = 0 ;

// calcul coefficients
for ( long i = 0 ; i  < size ; i++ )
    {
    const CVecZoneReduce::st_coord_poly* pStruct = VecNuage[i] ;
    Sx += pStruct->m_Lon ;
    Sy += pStruct->m_Lat ;
    Sxy += pStruct->m_Lon * pStruct->m_Lat ;
    Sx2 += pow( pStruct->m_Lon , 2 ) ;
    }

double S_xy = Sxy - Sx*Sy/size ;
double S_xx = Sx2 - pow(Sx,2)/size ;

// elaboration droite
m_ax = S_xy/S_xx ;
m_b = MoyenneLat - m_ax * MoyenneLon ;

// creation droite 2d
CDroite2D Droite ;
GetDroite( Droite ) ;
return Droite ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie une droite 2 d fonction du nuage de points.
void CNuage2Droite::GetDroite( CDroite2D & Droite ) const
{
CPoint2D Origine( 0 , m_b ) ;
CPoint2D PtAutre( 1 , m_ax + m_b ) ;
CVecteur2D VecDir( Origine , PtAutre ) ;
Droite = CDroite2D( VecDir , Origine ) ;
}

