////////////////////////////////////////////////////////////////////////////////
/// \file BVZonesAerienne.cpp
///
/// \brief Ce programme se propose de creer un fichier texte des zones aeriennes,
/// autour d'un point centrale et dans un rayon, pour le GnuVario firmware BertheVario.
///
/// \date creation     : 23/03/2024
/// \date modification : 20/07/2024
///

#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"
#include "CVecZoneReduce.h"

#define IN_ZONE_FILE    "data/20240615_ffvl-cfd.geojson"
#define UnMilesEnMetres (1852.)

using namespace std;
using namespace nlohmann ;

char NumVer[]="20241126b" ;

// centre clermont
float LatCentreDeg = 45.783329 ;
float LonCentreDeg =  3.08333 ;

// rayon 120km
float RayonDeg = 120. * 1000. / UnMilesEnMetres / 60. ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Zone aerienne pour tri suivant nombre de points. Pour optimization
/// memoire/compression zone BertheVario
class CZone
{
public :
    std::string     m_Name ;
    long            m_Bottom ;
    std::vector<CVecZoneReduce::st_coord_poly*> m_VecPts ;

    bool operator < ( const CZone & Zone ) const
        { return m_VecPts.size() < Zone.m_VecPts.size() ; } ;
    bool operator > ( const CZone & Zone ) const
        { return m_VecPts.size() > Zone.m_VecPts.size() ; } ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction main.
int main(int argc, char *argv[])
{
if ( argc < 4 )
    {
    cout << argv[0] << " lat_deg lon_deg rayon_km " << endl ;
    cout << "version du " << NumVer << endl ;
    cout << " ex pour clermont : ./BVZoneAerienne 45.783329 3.08333 120 > zonesaer.txt" << endl ;
    return 1 ;
    }

// parametres de la ligne de commande
LatCentreDeg = atof( argv[1] ) ;
LonCentreDeg = atof( argv[2] ) ;
RayonDeg = atof( argv[3] ) * 1000. / UnMilesEnMetres / 60. ;

std::ifstream ifs(IN_ZONE_FILE);
json jf = json::parse(ifs);

// nombre de zonnes totales
long NbAreas = jf["headerFile"]["numberOfAreas"] ;
//cout << "Nombre total de zones : " << NbAreas  << endl ;

// entete fichier
std::vector< CZone > VecZone ;

cout << "#Nom Zone;alti basse;lon0_deg,lat0_deg;lon1_deg,lat1_deg" << endl ;
// pour toutes les zones
for ( long iz = 0 ; iz < NbAreas ; iz++ )
    {
    CZone Zone ;
    // nom
    Zone.m_Name = jf["features"][iz]["properties"]["name"] ;
    // bas
    Zone.m_Bottom = jf["features"][iz]["properties"]["bottom_m"] ;

    // coordonnees
    auto CordArr = jf["features"][iz]["geometry"]["coordinates"][0] ;
    for (auto it = CordArr.begin(); it != CordArr.end(); ++it)
        {
        CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
        Zone.m_VecPts.push_back( pStPts ) ;
        pStPts->m_Lon = (*it)[0] ;
        pStPts->m_Lat = (*it)[1] ;
        }

    // determination si dans perimetre
    bool DansPerimetre = false ;
    for ( long nbc = 0 ; nbc < (long)Zone.m_VecPts.size() ; nbc++ )
        {
        float DistanceAngu = sqrtf( powf( Zone.m_VecPts[nbc]->m_Lon - LonCentreDeg , 2 ) + powf( Zone.m_VecPts[nbc]->m_Lat - LatCentreDeg , 2 ) ) ;
        if ( DistanceAngu < RayonDeg )
            {
            DansPerimetre = true ;
            break ;
            }
        }

    // si dans perimetre affichage
    if ( DansPerimetre )
        VecZone.push_back( Zone ) ;
    }

// tri croissant des zones
std::sort( VecZone.begin() , VecZone.end() ) ;

// fichier de zone pour gnuplot avant compression
// gnuplot> replot "zone_c.txt" with linespoints 6
ofstream ofs_znc("zone_nc.txt", std::ofstream::out);
CZone & Zone = VecZone[VecZone.size()-1] ;
cerr << "points de zone avant compression : " << Zone.m_VecPts.size() << endl ;
for ( long ip = 0 ; ip < (long)Zone.m_VecPts.size() ; ip++ )
    {
    ofs_znc << setprecision(5)
         << Zone.m_VecPts[ip]->m_Lon
         << " "
         << Zone.m_VecPts[ip]->m_Lat << endl ;
    }
ofs_znc.close() ;

// compression puis ecriture des zones
CVecZoneReduce VecZoneReduce ;
for ( long iz = VecZone.size() -1 ; iz >= 0 ; iz-- )
    {
    CZone & Zone = VecZone[iz] ;

    // compression de points
    //if ( Zone.m_VecPts.size() > 300 )
        {
        VecZoneReduce.Set( Zone.m_VecPts ) ;
        VecZoneReduce.ReduceToDistance( DIST_METRE_4_ZONE ) ;
        VecZoneReduce.ReduceToAngle( ANGLE_DEGRES_4_ZONE ) ;
        }

    // ecriture
    cout << Zone.m_Name << ";" << Zone.m_Bottom << ";" ;
    for ( long nbc = 0 ; nbc < (long)Zone.m_VecPts.size() ; nbc++ )
        cout << "" << Zone.m_VecPts[nbc]->m_Lon << "," << Zone.m_VecPts[nbc]->m_Lat << ";" ;
    cout << endl ;
    }

// fichier de zone pour gnuplot apres compression
ofstream ofs_zc("zone_c.txt", std::ofstream::out);
cerr << "points de zone apres compression : " << Zone.m_VecPts.size() << endl ;
for ( long ip = 0 ; ip < (long)Zone.m_VecPts.size() ; ip++ )
    {
    ofs_zc << setprecision(5)
         << Zone.m_VecPts[ip]->m_Lon
         << " "
         << Zone.m_VecPts[ip]->m_Lat << endl ;
    }
ofs_zc.close() ;

return 0;
}
