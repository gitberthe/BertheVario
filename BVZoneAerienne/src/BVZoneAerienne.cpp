////////////////////////////////////////////////////////////////////////////////
/// \file BVZonesAerienne.cpp
///
/// \brief Ce programme se propose de creer un fichier texte des zones aeriennes,
/// autour d'un point centrale et dans un rayon, pour le GnuVario firmware BertheVario.
///
/// \date creation     : 23/03/2024
/// \date modification : 26/03/2024
///

#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

#define IN_ZONE_FILE    "data/20231230_ffvl-cfd.geojson"
#define UnMilesEnMetres (1852.)

using namespace std;
using namespace nlohmann ;

char NumVer[]="20240326a" ;

// centre clermont
float LatCentreDeg = 45.783329 ;
float LonCentreDeg =  3.08333 ;

// rayon 100km
float RayonDeg = 100. * 1000. / UnMilesEnMetres / 60. ;

////////////////////////////////////////////////////////////////////////////////
/// \file Fonction main.
int main(int argc, char *argv[])
{
if ( argc < 4 )
    {
    cout << argv[0] << " lat_deg lon_deg rayon_km " << endl ;
    cout << "version du " << NumVer << endl ;
    cout << " ex pour clermont : BVZonesAerienne 45.783329 3.08333 100 > zonesaer.txt" << endl ;
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
cout << "#Nom Zone;alti basse;lon0_deg,lat0_deg;lon1_deg,lat1_deg" << endl ;
// pour toutes les zones
for ( long iz = 0 ; iz < NbAreas ; iz++ )
    {
    // nom
    string Name = jf["features"][iz]["properties"]["name"] ;
    // bas
    long   bottom = jf["features"][iz]["properties"]["bottom_m"] ;

    // coordonnees
    auto CordArr = jf["features"][iz]["geometry"]["coordinates"][0] ;
    vector<float> LonArr ;
    vector<float> LatArr ;
    for (auto it = CordArr.begin(); it != CordArr.end(); ++it)
        {
        LonArr.push_back( (*it)[0] ) ;
        LatArr.push_back( (*it)[1] ) ;
        }

    // determination si dans perimetre
    bool DansPerimetre = false ;
    for ( long nbc = 0 ; nbc < (long)LonArr.size() ; nbc++ )
        {
        float DistanceAngu = sqrtf( powf( LonArr[nbc] - LonCentreDeg , 2 ) + powf( LatArr[nbc] - LatCentreDeg , 2 ) ) ;
        if ( DistanceAngu < RayonDeg )
            {
            DansPerimetre = true ;
            break ;
            }
        }

    // si dans perimetre affichage
    if ( DansPerimetre )
        {
        cout << Name << ";" << bottom << ";" ;
        for ( long nbc = 0 ; nbc < (long)LonArr.size() ; nbc++ )
            cout << "" << LonArr[nbc] << "," << LatArr[nbc] << ";" ;
        cout << endl ;
        }
    }

return 0;
}
