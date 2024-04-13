////////////////////////////////////////////////////////////////////////////////
/// \file CHgt2Agl.h
///
/// \brief
///
/// \date creation     : 01/04/2024
/// \date modification : 01/04/2024
///

#ifndef _HGT2AGL_
#define _HGT2AGL_

#include <stdio.h>
#include <math.h>
#include <iostream>

#define REP_HGT_FILE "../../HgtData/"

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de lecture de fichier HGT pour avoir l'altitude sol fonction
/// de la position geographique Wgs84.
class CHgt2Agl
{
public :
    short GetGroundZ( float LonDeg , float LatDeg ) ;

private :
    short swap( short a ) ;
} ;

#endif
