////////////////////////////////////////////////////////////////////////////////
/// \file CNuage2Droite.h
///
/// \brief
///
/// \date creation     : 28/11/2024
/// \date modification : 28/11/2024
///

#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe d'approximation par une droite d'un nuage de points
class CNuage2Droite
{
private :
    double m_ax ;
    double m_b ;
    void GetDroite( CDroite2D & Droite ) const ;

public :
    CDroite2D ApproxDroite( const std::vector<CVecZoneReduce::st_coord_poly*> & VecNuage ) ;
} ;
