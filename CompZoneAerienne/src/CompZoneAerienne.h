////////////////////////////////////////////////////////////////////////////////
/// \file CompZoneAerienne.h
///
/// \brief
///
/// \date 26/11/2024 : creation
/// \date 10/10/2026 : ajout de CGlobalVar
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
#include <string.h>
#include <dirent.h>

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

#define T_PI                    (3.1415926535)
#define UnMileEnMetres          (1852.)
#define MilesParDegres          (60.)
#define UnMilesEnMetres         (1852.)
#define PiedsVersMetres         (0.3048)
#define FlVersMetres            (100*0,3048)

#define Deg2Rad     (T_PI/180.)
#define Rad2Deg     (180./T_PI)

// include du projet
#include "nlohmann/json.hpp"
#include "geometrie/CVecteur2D.h"
#include "geometrie/CDroite2D.h"
#include "zone/CVecZoneReduce.h"
#include "geometrie/Segments.h"
#include "geometrie/CVecteur3D.h"
#include "geometrie/CPolygone.h"
#include "zone/CNuage2Droite.h"
#include "zone/CGroupeAligne.h"
#include "zone/CCompZoneErr.h"
#include "openair/COpenAirFile.h"
#include "zone/CZone.h"
#include "kml/CKmlFile.h"
#include "CGlobalVar.h"

// noms / repertoires de fichier
#define IN_ZONE_FILE_JSON           "data/20250417_ffvl-cfd.geojson"
#define IN_ZONE_DIR_OPENAIR         "data/openair"
#define CHEMIN_GNUPLOT_BAZILE       "zones_gnuplot_bazile"
#define CHEMIN_GNUPLOT_OPENAIR      "zones_gnuplot_openair"

using namespace std ;

extern CGlobalVar g_GlobalVar ;

#endif
