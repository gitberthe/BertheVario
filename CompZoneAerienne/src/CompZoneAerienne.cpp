////////////////////////////////////////////////////////////////////////////////
/// \file CompZoneAerienne.cpp
///
/// \brief Ce programme se propose de creer un fichier texte des zones aeriennes,
/// autour d'un point centrale et dans un rayon, pour le BertheVarioTac.
///  Fichiers gnuplots avant/apres compression pour chaque zone dans zones_gnuplot.
///
/// \date creation     : 23/03/2024
/// \date 25/11/2024 : ajout de compression de zone par distance entre points et par
///                    angle de meme direction.
/// \date 29/11/2024 : modification
///

#include "CompZoneAerienne.h"

using namespace std;
using namespace nlohmann ;

char NumVer[]="20241129b" ;

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
    cout << " ex pour clermont : ./CompZoneAerienne 45.783329 3.08333 120 > zonesaer.txt" << endl ;
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

    // coordonnees de la zone
    auto CordArr = jf["features"][iz]["geometry"]["coordinates"][0] ;
    for (auto it = CordArr.begin(); it != CordArr.end(); ++it)
        {
        CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
        Zone.m_VecPts.push_back( pStPts ) ;
        pStPts->m_Lon = (*it)[0] ;
        pStPts->m_Lat = (*it)[1] ;
        }

    // determination si un points de la zone dans le perimetre
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

// compression puis ecriture des zones
CVecZoneReduce VecZoneReduce ;
for ( long iz = VecZone.size() -1 ; iz >= 0 ; iz-- )
    {
    CZone & Zone = VecZone[iz] ;
    char NomFichierGnuplotAvecChemin[3000] ;
    int NbPtsAvantComp = Zone.m_VecPts.size() ;

    // fichier de zone pour gnuplot avant compression
    // gnuplot> replot "zone_c.txt" with linespoints 6
    sprintf(NomFichierGnuplotAvecChemin,"./zones_gnuplot/%03ld_avc.txt",iz) ;
    ofstream ofs_znc(NomFichierGnuplotAvecChemin, std::ofstream::out);
    for ( long ip = 0 ; ip < (long)Zone.m_VecPts.size() ; ip++ )
        {
        ofs_znc << setprecision(8)
            << Zone.m_VecPts[ip]->m_Lon
            << " "
            << Zone.m_VecPts[ip]->m_Lat << endl ;
        }
    ofs_znc.close() ;

    // compression de points
    VecZoneReduce.Set( Zone.m_VecPts ) ;
    //VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( DIST_METRE_DROITE , -1 , DIST_METRE_PTS ) ;
    VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( DIST_METRE_DROITE , -1 , -1 ) ;
    //VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( -1 , ANGLE_DEGRES , -1 ) ;
    //VecZoneReduce.ReduceMultiDistanceDroite( DIST_METRE_MULTI_DROITE ) ;
    //VecZoneReduce.ReduceNuageDroite( DIST_METRE_NUAGE_DROITE ) ;
    //VecZoneReduce.ReduceToDistanceDroiteAngleDistancePoint( -1 , ANGLE_DEGRES , DIST_METRE_PTS ) ;
    //VecZoneReduce.ReduceNuageBravaisPearson( DIST_METRE_NUAGE_DROITE_BP , COEF_BRAVAIS_PEARSON ) ;

    // ecriture
    cout << Zone.m_Name << ";" << Zone.m_Bottom << ";" ;
    for ( long nbc = 0 ; nbc < (long)Zone.m_VecPts.size() ; nbc++ )
        cout << "" << Zone.m_VecPts[nbc]->m_Lon << "," << Zone.m_VecPts[nbc]->m_Lat << ";" ;
    cout << endl ;

    // fichier de zone pour gnuplot apres compression
    int NbPtsApresComp = Zone.m_VecPts.size() ;
    sprintf(NomFichierGnuplotAvecChemin,"./zones_gnuplot/%03ld_apc.txt",iz) ;
    ofstream ofs_zc(NomFichierGnuplotAvecChemin, std::ofstream::out);
    for ( long ip = 0 ; ip < (long)Zone.m_VecPts.size() ; ip++ )
        {
        ofs_zc << setprecision(8)
            << Zone.m_VecPts[ip]->m_Lon
            << " "
            << Zone.m_VecPts[ip]->m_Lat << endl ;
        }
    ofs_zc.close() ;

    // cerr
    cerr << iz << " " << Zone.m_Name << " " << NbPtsAvantComp << " > " << NbPtsApresComp << endl ;

    }

/*
std::vector<CVecZoneReduce::st_coord_poly*> VecTest ;
CVecZoneReduce::st_coord_poly* pSt ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 22 ;
pSt->m_Lat = 18 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 22 ;
pSt->m_Lat = 19 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 23 ;
pSt->m_Lat = 20 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 26 ;
pSt->m_Lat = 18 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 31 ;
pSt->m_Lat = 23 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 32 ;
pSt->m_Lat = 24 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 34 ;
pSt->m_Lat = 22 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 37 ;
pSt->m_Lat = 25 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 41 ;
pSt->m_Lat = 29 ;
VecTest.push_back( pSt ) ;
pSt = new CVecZoneReduce::st_coord_poly ;
pSt->m_Lon = 42 ;
pSt->m_Lat = 27 ;
VecTest.push_back( pSt ) ;

CNuage2Droite N2D ;
N2D.ApproxDroite( VecTest ) ;
CDroite2D Droite ;
N2D.GetDroite( Droite ) ; */

return 0;
}
