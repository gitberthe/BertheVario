////////////////////////////////////////////////////////////////////////////////
/// \file CGroupeAligne.cpp
///
/// \brief
///
/// \date creation     : 28/11/2024
/// \date modification : 29/11/2024
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction de tri des points par longitude croissante/decroissante
void CSortPts::Sort( std::vector<CVecZoneReduce::st_coord_poly*> & VecToSort )
{
long size  = VecToSort.size() ;
std::vector< CSortPts > VectCSortPts ;

// constrauction vecteur de points a trier
for ( long i = 0 ; i < size ; i++ )
    {
    CSortPts SortPts ;
    SortPts.m_Lon = VecToSort[i]->m_Lon ;
    SortPts.m_ipts = i ;
    VectCSortPts.push_back( SortPts ) ;
    }

// tri vecteur
std::sort( VectCSortPts.begin() , VectCSortPts.end() ) ;

// reoganisation croissante/decroisante
std::vector<CVecZoneReduce::st_coord_poly*> VecSorted ;
if ( VecToSort[0]->m_Lon < VecToSort[VecToSort.size()-1]->m_Lon )
    {
    for ( long i = 0 ; i < size ; i++ )
        VecSorted.push_back( VecToSort[ VectCSortPts[i].m_ipts ] ) ;
    }
else
    {
    for ( long i = size - 1 ; i >= 0 ; i-- )
        VecSorted.push_back( VecToSort[ VectCSortPts[i].m_ipts ] ) ;
    }

// recopie veteur de sortie
VecToSort = VecSorted ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul le coefficieb de correlation de Bravais Pearson.
double CGroupeAligne::GetBravaisPearson()
{
const std::vector<CVecZoneReduce::st_coord_poly*> & VecpStruct = m_Vect ;
//for ( long ip = 0 ; ip < (long)VecIPts.size() ; ip++ )
//    VecpStruct.push_back( (*m_pVecOrigine)[VecIPts[ip]] ) ;

double MoyenneLat = 0 ;
double MoyenneLon = 0 ;
const long size = VecpStruct.size() ;

if ( size <= 2 )
    return 1. ;

for ( long i = 0 ; i  < size ; i++ )
    {
    const CVecZoneReduce::st_coord_poly* pStruct = VecpStruct[i] ;
    MoyenneLat += pStruct->m_Lat ;
    MoyenneLon += pStruct->m_Lon ;
    }
MoyenneLat /= size ;
MoyenneLon /= size ;

double Num = 0 ;
double SumDenLat = 0 ;
double SumDenLon = 0 ;
for ( long i = 0 ; i  < size ; i++ )
    {
    const CVecZoneReduce::st_coord_poly* pStruct = VecpStruct[i] ;
    Num += (pStruct->m_Lat-MoyenneLat)*(pStruct->m_Lon-MoyenneLon) ;
    SumDenLat += pow(pStruct->m_Lat-MoyenneLat,2) ;
    SumDenLon += pow(pStruct->m_Lon-MoyenneLon,2) ;
    }

return Num / sqrt(SumDenLat*SumDenLon) ;
}
