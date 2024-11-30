////////////////////////////////////////////////////////////////////////////////
/// \file CCompZoneErr.h
///
/// \brief
///
/// \date creation     : 30/11/2024
/// \date modification : 30/11/2024
///

#pragma once

class CCompZoneErr
{
public :
    double GetErrMetres( const std::vector<CVecZoneReduce::st_coord_poly*> & BigZone , const std::vector<CVecZoneReduce::st_coord_poly*> & SmallZone ) ;

private :
    double GetErrMetres( const CVecZoneReduce::st_coord_poly* pPtLineA , const CVecZoneReduce::st_coord_poly* pPtLineB , const CVecZoneReduce::st_coord_poly* pPt ) ;
} ;
