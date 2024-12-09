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
/// \date 09/12/2024 : modification
///

#include "CompZoneAerienne.h"

using namespace std;
using namespace nlohmann ;

char NumVer[]="20241209a" ;

////////////////////////////////////////////////////////////////////////////////
/// \brief
void split(const string &chaine, vector<string> &elements)
{
string sousChaine ;
char TmpChar[1000] ;
strcpy( TmpChar , chaine.c_str() ) ;
char * pChar = strtok( TmpChar , " \n\t" ) ;
while ( pChar != NULL )
    {
    elements.push_back(pChar);
    pChar = strtok( NULL , " \n\t" ) ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief parametre de la ligne de commande (clermont)
class CCmdLineParam
{
public :
    double m_LatCentreDeg = 45.783329 ; ///< centre clermont
    double m_LonCentreDeg =  3.08333 ;  ///< centre clermont
    double m_RayonDeg     = 120. * 1000. / UnMilesEnMetres / MilesParDegres ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction main.
int main(int argc, char *argv[])
{
// destruction des png et zone gnuplot
const long TailleBuffer = 10000 ;
char TmpChar[TailleBuffer] ;
//string path = std::filesystem::current_path() ;
getcwd( TmpChar , TailleBuffer ) ;
string path = TmpChar ;
#ifdef WIN32
 chdir( "./zones_gnuplot" ) ;
 system( "del *.txt" ) ;
 system( "del *.png" ) ;
 chdir( "..") ;
#else
 sprintf( TmpChar , "rm -rf %s/zones_gnuplot/*.txt %s/zones_gnuplot/*.png", path.c_str() , path.c_str() ) ;
 system( TmpChar ) ;
#endif

// verification ligne de commande
if ( argc < 2 || (strcmp(argv[1],"--batch") != 0 && argc < 4) || (strcmp(argv[1],"--batch") == 0 && argc < 4) )
    {
    cerr << argv[0] << " lat_deg lon_deg rayon_km " << endl ;
    cerr << argv[0] << " --batch terconnu.txt rayon_km " << endl ;
    cerr << "version du " << NumVer << endl ;
    cerr << " ex pour clermont : ./CompZoneAerienne 45.783329 3.08333 120 > zonesaer.txt" << endl ;
    cerr << " ex mode batch    : ./CompZoneAerienne --batch terconnu.txt 80 > zonesaer.txt" << endl ;
    return 1 ;
    }

// vecteur des differents points centre/rayon de recherche de zone
std::vector<CCmdLineParam> VectPtCentreRayon ;

// fichier batch de rayon/centre
if ( strcmp(argv[1],"--batch") == 0 )
    {
    ifstream ifs_zprc( argv[2] );
    double RayonKm = atof( argv[3] ) ;
    for( std::string line; getline( ifs_zprc, line ); )
        {
        vector<string> VecMot ;
        split(line, VecMot);

        // ligne vide ou commentaire
        if ( VecMot.size() == 0 || VecMot[0][0] == '#' )
            continue ;

        if ( VecMot.size() != 0 && VecMot[0] == "break")
            break ;

        // ajout zone
        CCmdLineParam CmdLineParam ;
        CmdLineParam.m_LatCentreDeg = atof( VecMot[2].c_str() ) ;
        CmdLineParam.m_LonCentreDeg = atof( VecMot[3].c_str() ) ;
        CmdLineParam.m_RayonDeg = RayonKm * 1000. / UnMilesEnMetres / MilesParDegres ;

        VectPtCentreRayon.push_back( CmdLineParam ) ;
        }
    ifs_zprc.close() ;
    }
// parametres de la ligne de commande
else
    {
    CCmdLineParam CmdLineParam ;

    CmdLineParam.m_LatCentreDeg = atof( argv[1] ) ;
    CmdLineParam.m_LonCentreDeg = atof( argv[2] ) ;
    CmdLineParam.m_RayonDeg = atof( argv[3] ) * 1000. / UnMilesEnMetres / MilesParDegres ;

    VectPtCentreRayon.push_back( CmdLineParam ) ;
    }

std::ifstream ifs(IN_ZONE_FILE);
json jf = json::parse(ifs);

// nombre de zonnes totales
long NbAreas = jf["headerFile"]["numberOfAreas"] ;
//cout << "Nombre total de zones : " << NbAreas  << endl ;

// entete fichier
std::vector< CZone > VecZone ;

cout << "#Nom Zone;alti basse;lon0_deg,lat0_deg;lon1_deg,lat1_deg" << endl ;

// pour tout les points centre de recherche
for ( size_t iptcentre = 0 ; iptcentre < VectPtCentreRayon.size() ; iptcentre++ )
    {
    const CCmdLineParam & CmdLineParam = VectPtCentreRayon[iptcentre] ;
    double LonCentreDeg = CmdLineParam.m_LonCentreDeg ;
    double LatCentreDeg = CmdLineParam.m_LatCentreDeg ;
    double RayonDeg     = CmdLineParam.m_RayonDeg ;

    // pour toutes les zones, determination si dans perimetres
    // et ajout au VecZone
    for ( long izf = 0 ; izf < NbAreas ; izf++ )
        {
        CZone Zone ;
        // nom
        Zone.m_Name = jf["features"][izf]["properties"]["name"] ;

        // zone a ne pas traiter
        if ( strstr( Zone.m_Name.c_str() , "LTA FRANCE 1" ) != NULL )
            {
            cerr << Zone.m_Name << " **** non traite **** " << endl ;
            continue ;
            }

        // bas de zone
        Zone.m_Bottom = jf["features"][izf]["properties"]["bottom_m"] ;

        // coordonnees de la zone
        auto CordArr = jf["features"][izf]["geometry"]["coordinates"][0] ;
        for (auto it = CordArr.begin(); it != CordArr.end(); ++it)
            {
            CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
            Zone.m_VecPtsBig.push_back( pStPts ) ;
            pStPts->m_Lon = (*it)[0] ;
            pStPts->m_Lat = (*it)[1] ;
            }

        // si zone deja faite
        bool ZoneDejaFaite = false ;
        for ( size_t izdf = 0 ; izdf < VecZone.size() ; izdf++ )
            if ( VecZone[izdf] == Zone )
                {
                ZoneDejaFaite = true ;
                break ;
                }

        if ( ZoneDejaFaite )
            continue ;

        // determination si un points de la zone dans le perimetre
        bool DansPerimetre = false ;
        for ( long nbc = 0 ; nbc < (long)Zone.m_VecPtsBig.size() ; nbc++ )
            {
            double DistanceAngu = sqrtf( powf( Zone.m_VecPtsBig[nbc]->m_Lon - LonCentreDeg , 2 ) + powf( Zone.m_VecPtsBig[nbc]->m_Lat - LatCentreDeg , 2 ) ) ;
            if ( DistanceAngu < RayonDeg )
                {
                DansPerimetre = true ;
                break ;
                }
            }

        // si dans perimetre affichage et pas en doublons
        if ( DansPerimetre )
            VecZone.push_back( Zone ) ;
        }
    }

// compression des zones
for ( long iz = VecZone.size() -1 ; iz >= 0 ; iz-- )
    VecZone[iz].Compress() ;

// tri croissant des zones compressees
std::sort( VecZone.begin() , VecZone.end() ) ;

// ecriture des zones sur cout, fichier gnuplot
double RayonMaxAllKm = 0 ;
double ResolutionMoyenneMetre = 0 ;
for ( long iz = VecZone.size() -1 ; iz >= 0 ; iz-- )
    {
    CZone & Zone = VecZone[iz] ;
    char NomFichierGnuplotAvecChemin[3000] ;

    #ifndef WIN32
    // fichier de zone pour gnuplot avant compression
    // gnuplot> replot "zone_c.txt" with linespoints 6
    sprintf(NomFichierGnuplotAvecChemin,"./zones_gnuplot/%03ld_avc.txt",iz) ;
    ofstream ofs_znc(NomFichierGnuplotAvecChemin, std::ofstream::out);
    for ( long ip = 0 ; ip < (long)Zone.m_VecPtsBig.size() ; ip++ )
        {
        ofs_znc << setprecision(9)
            << Zone.m_VecPtsBig[ip]->m_Lon
            << " "
            << Zone.m_VecPtsBig[ip]->m_Lat << endl ;
        }
    // echelle 1km
    ofs_znc << setprecision(9) << Zone.m_VecPtsBig[0]->m_Lon << " " << Zone.m_VecPtsBig[0]->m_Lat << endl ;
    ofs_znc << setprecision(9) << Zone.m_VecPtsBig[0]->m_Lon + 1000./(MilesParDegres*UnMileEnMetres) << " " << Zone.m_VecPtsBig[0]->m_Lat << endl ;
    ofs_znc << setprecision(9) << Zone.m_VecPtsBig[0]->m_Lon << " " << Zone.m_VecPtsBig[0]->m_Lat << endl ;
    ofs_znc << setprecision(9) << Zone.m_VecPtsBig[0]->m_Lon << " " << Zone.m_VecPtsBig[0]->m_Lat + 1000./(MilesParDegres*UnMileEnMetres) << endl ;
    ofs_znc.close() ;
    #endif

    // ecriture pour fichier texte sur cout
    cout << Zone.m_Name << ";" << Zone.m_Bottom << ";" ;
    for ( long nbc = 0 ; nbc < (long)Zone.m_VecPtsSmall.size() ; nbc++ )
        cout << "" << setprecision(9) << Zone.m_VecPtsSmall[nbc]->m_Lon << "," << Zone.m_VecPtsSmall[nbc]->m_Lat << ";" ;
    cout << endl ;

    #ifndef WIN32
    // fichier de zone pour gnuplot apres compression
    sprintf(NomFichierGnuplotAvecChemin,"./zones_gnuplot/%03ld_apc.txt",iz) ;
    ofstream ofs_zc(NomFichierGnuplotAvecChemin, std::ofstream::out);
    for ( long ip = 0 ; ip < (long)Zone.m_VecPtsSmall.size() ; ip++ )
        {
        ofs_zc << setprecision(9)
            << Zone.m_VecPtsSmall[ip]->m_Lon
            << " "
            << Zone.m_VecPtsSmall[ip]->m_Lat << endl ;
        }
    ofs_zc.close() ;
    #endif

    // resolution zone
    double RayonMaxZoneKm = Zone.GetRayonMaxKm() ;
    double ResolutionZoneMetre = RayonMaxZoneKm * 1000. / pow(2.,15) ;
    short  ResolutionEntiere   = ceil(ResolutionZoneMetre) ;
    ResolutionMoyenneMetre += ResolutionEntiere ;
    RayonMaxAllKm = MAX ( RayonMaxAllKm , RayonMaxZoneKm ) ;

    // cerr
    cerr << setw(3) << fixed << setfill(' ') << iz << ", comp: " << setw(4) << Zone.m_NbPtsAvantCompression << "/" << setw(4) << Zone.m_NbPtsApresCompression
         << ", rayon: " << setw(5) << setprecision(1) << RayonMaxZoneKm
         << ", resol: " << ResolutionZoneMetre << "/" << ResolutionEntiere
         << ", err: " << fixed << setw(4) << setprecision(1) << Zone.m_ErreurMoyenneEnMetre
         << " : " << Zone.m_Name << endl ;

    // png gnuplot
    #ifndef WIN32
     sprintf( TmpChar , "%s/zones_gnuplot/gnuplot.sh %s/zones_gnuplot/%03ld", path.c_str() , path.c_str() , iz ) ;
     system( TmpChar ) ;
    #endif
    }

ResolutionMoyenneMetre /= VecZone.size() ;
cerr << "Rayon max toutes zones en km         : " << RayonMaxAllKm << endl ;
cerr << "Resolution max short admise en metre : " << RayonMaxAllKm * 1000. / pow(2.,15) << endl ;
cerr << "Resolution moyenne en metre          : " << setprecision(2) << ResolutionMoyenneMetre << endl ;

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
