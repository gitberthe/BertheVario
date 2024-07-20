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

#define IN_ZONE_FILE    "data/20240615_ffvl-cfd.geojson"
#define UnMilesEnMetres (1852.)

using namespace std;
using namespace nlohmann ;

char NumVer[]="20240720a" ;

// centre clermont
float LatCentreDeg = 45.783329 ;
float LonCentreDeg =  3.08333 ;

// rayon 150km
float RayonDeg = 120. * 1000. / UnMilesEnMetres / 60. ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Zone aerienne pour tri suivant nombre de points. Pour optimization
/// memoire/compression zone BertheVario
class CZone
{
public :
    std::string     m_Name ;
    long            m_Bottom ;
    vector<float>   m_LonArr ;
    vector<float>   m_LatArr ;

    bool operator < ( const CZone & Zone ) const
        { return m_LonArr.size() < Zone.m_LonArr.size() ; } ;
    bool operator > ( const CZone & Zone ) const
        { return m_LonArr.size() > Zone.m_LonArr.size() ; } ;
} ;


////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction main.
int main(int argc, char *argv[])
{
if ( argc < 4 )
    {
    cout << argv[0] << " lat_deg lon_deg rayon_km " << endl ;
    cout << "version du " << NumVer << endl ;
    cout << " ex pour clermont : BVZoneAerienne 45.783329 3.08333 120 > zonesaer.txt" << endl ;
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
        Zone.m_LonArr.push_back( (*it)[0] ) ;
        Zone.m_LatArr.push_back( (*it)[1] ) ;
        }

    // determination si dans perimetre
    bool DansPerimetre = false ;
    for ( long nbc = 0 ; nbc < (long)Zone.m_LonArr.size() ; nbc++ )
        {
        float DistanceAngu = sqrtf( powf( Zone.m_LonArr[nbc] - LonCentreDeg , 2 ) + powf( Zone.m_LatArr[nbc] - LatCentreDeg , 2 ) ) ;
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

// ecriture des zones
for ( long iz = VecZone.size() -1 ; iz >= 0 ; iz-- )
    {
    cout << VecZone[iz].m_Name << ";" << VecZone[iz].m_Bottom << ";" ;
    for ( long nbc = 0 ; nbc < (long)VecZone[iz].m_LonArr.size() ; nbc++ )
        cout << "" << VecZone[iz].m_LonArr[nbc] << "," << VecZone[iz].m_LatArr[nbc] << ";" ;
    cout << endl ;
    }

return 0;
}
