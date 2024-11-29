////////////////////////////////////////////////////////////////////////////////
/// \file CompZoneAerienne.h
///
/// \brief
///
/// \date 26/11/2024 : creation
/// \date 29/11/2024 : modification
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

///////////
// macro //
///////////

#define FABS(x)     fabsf(x)
#define SQRT(x)     sqrtf(x)
#define SIN(x)      sinf(x)
#define COS(x)      cosf(x)
#define ACOS(x)     acosf(x)
#define POW(x,y)    powf(x,y)
#define FMOD(x,y)   fmodf(x,y)
#define FLOOR(x)    floorf(x)
#define MODF(x,y)   modff(x,y)

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

#define IN_ZONE_FILE    "data/20240615_ffvl-cfd.geojson"
#define UnMilesEnMetres (1852.)

#endif
