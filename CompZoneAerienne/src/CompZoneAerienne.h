////////////////////////////////////////////////////////////////////////////////
/// \file CompZoneAerienne.h
///
/// \brief
///
/// \date 26/11/2024 : creation
/// \date 09/12/2024 : modification
///

#ifndef _BVTZonesAerienne_
#define _BVTZonesAerienne_

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>
#include <cfloat>
#include <unistd.h>
#include <filesystem>
#include <unistd.h>

///////////
// macro //
///////////

#define FABS(x)     fabs(x)
#define SQRT(x)     sqrt(x)
#define SIN(x)      sin(x)
#define COS(x)      cos(x)
#define ACOS(x)     acos(x)
#define POW(x,y)    pow(x,y)
#define FMOD(x,y)   fmod(x,y)
#define FLOOR(x)    floor(x)
#define MODF(x,y)   modf(x,y)

#define MIN( a , b )   (((a)<(b)) ? (a) : (b))
#define MAX( a , b )   (((a)>(b)) ? (a) : (b))

////////////////
// constantes //
////////////////

#define T_PI                   (3.1415926535)
#define UnMileEnMetres         (1852.)
#define MilesParDegres         (60.)

// include du projet
#include "nlohmann/json.hpp"
#include "CVecteur2D.h"
#include "CDroite2D.h"
#include "CVecZoneReduce.h"
#include "CNuage2Droite.h"
#include "CGroupeAligne.h"
#include "CCompZoneErr.h"
#include "CZone.h"

#define IN_ZONE_FILE    "data/20240615_ffvl-cfd.geojson"
#define UnMilesEnMetres (1852.)

#endif
