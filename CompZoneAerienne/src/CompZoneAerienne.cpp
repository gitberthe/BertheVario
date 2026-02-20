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

#define CHEMIN_GNUPLOT "zones_gnuplot"

using namespace std;
using namespace nlohmann ;

char NumVer[]="20260112a" ;

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
if ( getcwd( TmpChar , TailleBuffer ) == NULL )
    cerr << "imposible d'avoir le cwd" << endl ;

string path = TmpChar ;
#ifdef WIN32
 chdir( "./zones_gnuplot" ) ;
 system( "del *.txt" ) ;
 system( "del *.png" ) ;
 chdir( "..") ;
#else
 sprintf( TmpChar , "rm -rf %s/%s/*.txt %s/%s/*.png", path.c_str(), CHEMIN_GNUPLOT, path.c_str(), CHEMIN_GNUPLOT ) ;
 if ( system( TmpChar ) )
    cerr << "impossible de faire le rm de zones_gnuplot" << endl ;
#endif

// verification ligne de commande
if ( argc < 2 || (strcmp(argv[1],"--batch") != 0 && argc < 4) || (strcmp(argv[1],"--batch") == 0 && argc < 4) )
    {
    cerr << "version du " << NumVer << endl ;
    cerr << " " << argv[0] << " lat_deg lon_deg rayon_km " << endl ;
    cerr << " " << argv[0] << " --batch terconnu.txt rayon_km " << endl ;
    cerr << "    ex pour clermont : ./CompZoneAerienne 45.783329 3.08333 120 > zonesaer.txt" << endl ;
    cerr << "    ex mode batch    : ./CompZoneAerienne --batch terconnu.txt 60 > zonesaer.txt" << endl ;
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

// ouverture fichier geojson
std::ifstream ifs(IN_ZONE_FILE);
if ( ! ifs.is_open() )
    {
    cerr << "**** impossible d'ouvrir : " << IN_ZONE_FILE << endl ;
    exit( 1 ) ;
    }
json jf = json::parse(ifs);

// nombre de zonnes totales
long NbAreas = jf["headerFile"]["numberOfAreas"] ;

// vecteur des zones
std::vector< CZone* > VecZone ;

// entete fichier
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
        CZone * pZone = new CZone ;

        // nom
        pZone->m_Name = jf["features"][izf]["properties"]["name"] ;

        // zone a ne pas traiter
        if ( strstr( pZone->m_Name.c_str() , "LTA FRANCE 1" ) != NULL )
            {
            cerr << pZone->m_Name << " **** non traite **** " << endl ;
            continue ;
            }

        // bas de zone
        pZone->m_Bottom = jf["features"][izf]["properties"]["bottom_m"] ;

        // coordonnees de la zone
        auto CordArr = jf["features"][izf]["geometry"]["coordinates"][0] ;
        for (auto it = CordArr.begin(); it != CordArr.end(); ++it)
            {
            CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
            pZone->m_VecPtsBig.push_back( pStPts ) ;
            pStPts->m_Lon = (*it)[0] ;
            pStPts->m_Lat = (*it)[1] ;
            }

        // si zone deja faite
        bool ZoneDejaFaite = false ;
        for ( size_t izdf = 0 ; izdf < VecZone.size() ; izdf++ )
            if ( *VecZone[izdf] == *pZone )
                {
                ZoneDejaFaite = true ;
                break ;
                }

        if ( ZoneDejaFaite )
            continue ;

        // determination si un points de la zone dans le perimetre
        bool DansPerimetre = false ;
        for ( long nbc = 0 ; nbc < (long)pZone->m_VecPtsBig.size() ; nbc++ )
            {
            double DistanceAngu = sqrtf( powf( pZone->m_VecPtsBig[nbc]->m_Lon - LonCentreDeg , 2 ) + powf( pZone->m_VecPtsBig[nbc]->m_Lat - LatCentreDeg , 2 ) ) ;
            if ( DistanceAngu < RayonDeg )
                {
                DansPerimetre = true ;
                break ;
                }
            }

        // si dans perimetre affichage et pas en doublons
        if ( DansPerimetre )
            VecZone.push_back( pZone ) ;
        }
    }

// compression des zones
cerr << "compression zone" << endl ;
for ( long iz = VecZone.size() -1 ; iz >= 0 ; iz-- )
    {
    VecZone[iz]->Compress() ;
    cerr << setw(5) << iz << "\r" << flush ;
    }

// tri de-croissant des zones compressees
std::sort( VecZone.begin() , VecZone.end() , MySortFonction ) ;
//std::reverse( VecZone.begin() , VecZone.end() ) ;

// ecriture des zones sur cout, fichier gnuplot
double DelatLatLonMaxZoneKm = 0 ;
double ResolutionMoyenneMetre = 0 ;
double ResolutionMaxAllMetres = 0. ;
double ErreurMaxiMetres = 0. ;
double ErreurMoyenneMetres = 0. ;
for ( unsigned long iz = 0 ; iz < VecZone.size() ; iz++ )
    {
    CZone & Zone = *VecZone[iz] ;

    // ecriture pour fichier texte sur cout
    cout << Zone.m_Name << ";" << Zone.m_Bottom << ";" ;
    for ( long nbc = 0 ; nbc < (long)Zone.m_VecPtsSmall.size() ; nbc++ )
        cout << "" << setprecision(9) << Zone.m_VecPtsSmall[nbc]->m_Lon << "," << Zone.m_VecPtsSmall[nbc]->m_Lat << ";" ;
    cout << endl ;

    // resolution zone
    double DelatLatLonZoneKm = Zone.m_DeltaLatLonMax ;
    double ResolutionZoneMetre = Zone.m_ResolVario ;
    ResolutionMoyenneMetre += ResolutionZoneMetre ;
    ResolutionMaxAllMetres = MAX ( ResolutionMaxAllMetres , ResolutionZoneMetre ) ;
    DelatLatLonMaxZoneKm = MAX( DelatLatLonMaxZoneKm , DelatLatLonZoneKm ) ;
    ErreurMaxiMetres = MAX( Zone.m_ErreurMaxEnMetre , ErreurMaxiMetres ) ;
    ErreurMoyenneMetres += Zone.m_ErreurMoyenneEnMetre ;

    // cerr
    cerr << setw(3) << fixed << setfill(' ') << iz << ", comp: " << setw(4) << Zone.m_NbPtsAvantCompression << "/" << setw(4) << Zone.m_NbPtsApresCompression << " pts"
         << ", delta_latlon: " << setw(5) << setprecision(1) << DelatLatLonZoneKm << " km"
         << ", resol: " << ResolutionZoneMetre << " m"
         << ", err moyenne: " << fixed << setw(4) << setprecision(1) << Zone.m_ErreurMoyenneEnMetre << "m"
         << ", err maxi: " << fixed << setw(4) << setprecision(1) << Zone.m_ErreurMaxEnMetre << "m"
         << " : " << Zone.m_Name << endl ;

    if ( Zone.m_ErreurMaxEnMetre > CZone::ms_ErreurMaxiAdmiseVerif )
        {
        cerr << "**** erreur maximum > " << CZone::ms_ErreurMaxiAdmiseVerif << " ****" << endl ;
        exit( 1 ) ;
        }
    }

// pour gnu plot
#ifndef WIN32
cerr << "creation fichier gnuplot dans " << CHEMIN_GNUPLOT << endl ;
for ( unsigned long iz = 0 ; iz < VecZone.size() ; iz++ )
    {
    CZone & Zone = *VecZone[iz] ;
    char NomFichierGnuplotAvecChemin[3000] ;
    char NomFichierGnuplotNomDeZoneSansBlanc[3000] ;

    // nom de zone court sans blanc pour visionnage
    std::string NomZoneSansBlanc  = Zone.m_Name ;
    // on enleve les blancs et /
    long iblanc = 0 ;
    for ( unsigned long i = 0 ; i < NomZoneSansBlanc.length() ; i++ )
        {
        if ( NomZoneSansBlanc[i] == ' ' || NomZoneSansBlanc[i] == '/' ||
             NomZoneSansBlanc[i] == '(' || NomZoneSansBlanc[i] == ')' ||
             NomZoneSansBlanc[i] == '"' || NomZoneSansBlanc[i] == '\'' )
            {
            NomZoneSansBlanc[i] = '_' ;
            iblanc++ ;
            }
        if ( iblanc > 8 )
            {
            NomZoneSansBlanc[i] = 0 ;
            break ;
            }
        }
    Zone.m_NameSansBlanc = NomZoneSansBlanc ;

    // fichier de zone pour gnuplot avant compression
    // gnuplot> replot "zone_c.txt" with linespoints 6
    sprintf(NomFichierGnuplotAvecChemin,"./%s/%03ld_avc_%s.txt",CHEMIN_GNUPLOT,iz,NomZoneSansBlanc.c_str()) ;
    string NomFichierAvc = NomFichierGnuplotAvecChemin ;
    ofstream ofs_znc(NomFichierAvc.c_str(), std::ofstream::out);
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

    // fichier de zone pour gnuplot apres compression
    sprintf(NomFichierGnuplotAvecChemin,"./%s/%03ld_apc_%s.txt",CHEMIN_GNUPLOT,iz,NomZoneSansBlanc.c_str()) ;
    string NomFichierApc = NomFichierGnuplotAvecChemin ;
    ofstream ofs_zc(NomFichierApc.c_str(), std::ofstream::out);
    for ( long ip = 0 ; ip < (long)Zone.m_VecPtsSmall.size() ; ip++ )
        {
        ofs_zc << setprecision(9)
            << Zone.m_VecPtsSmall[ip]->m_Lon
            << " "
            << Zone.m_VecPtsSmall[ip]->m_Lat << endl ;
        }
    ofs_zc.close() ;

    sprintf(NomFichierGnuplotNomDeZoneSansBlanc,"./%s/%03ld_txt_%s.txt",CHEMIN_GNUPLOT, iz, NomZoneSansBlanc.c_str() ) ;
    ofstream ofs_nom_zone(NomFichierGnuplotNomDeZoneSansBlanc, std::ofstream::out);
         ofs_nom_zone   << "num : " << setw(3) << fixed << setfill(' ') << iz << endl
                        << " nom : " << Zone.m_Name << endl
                        << " alti. min: " << Zone.m_Bottom << " m" << endl
                        << " compression:  " << Zone.m_NbPtsAvantCompression << "/" << setw(4) << Zone.m_NbPtsApresCompression << " pts" << endl
                        << " delta max:    " << setprecision(1) << Zone.m_DeltaLatLonMax << " km" << endl
                        << " resol delta: " << Zone.m_ResolVario << " metres" << endl
                        << " err moyenne:  " << setprecision(1) << Zone.m_ErreurMoyenneEnMetre << " m" << endl
                        << " err maxi:  " << setprecision(1) << Zone.m_ErreurMaxEnMetre << " m" << endl ;
    ofs_nom_zone.close() ;

    // png gnuplot
    cerr << "gnuplot " << NomFichierAvc << endl ;
    sprintf( TmpChar , "%s/%s/gnuplot.sh %s %s %s/%s/%03ld_png_%s.png %c %s => del max: %.1f km, alti min: %ld m, pts: %ld/%ld, resol/e-moy/e-max: %.2f/%.1f/%.1fm %c",
                        path.c_str(), CHEMIN_GNUPLOT,
                        NomFichierAvc.c_str(),
                        NomFichierApc.c_str(),
                        path.c_str(), CHEMIN_GNUPLOT, iz, NomZoneSansBlanc.c_str() ,
                        '"' , Zone.m_Name.c_str() , Zone.m_DeltaLatLonMax , Zone.m_Bottom ,
                        Zone.m_VecPtsBig.size() , Zone.m_VecPtsSmall.size() ,
                        Zone.m_ResolVario , Zone.m_ErreurMoyenneEnMetre, Zone.m_ErreurMaxEnMetre,
                        '"' ) ;
    //cerr << TmpChar << "\r" << flush ;
     if ( system( TmpChar ) )
        cerr << "impossible de faire zones_gnuplot/gnuplot.sh" << endl ;
    }
#endif

// creation des fichier kml
cerr << "creation fichier kml dans " << CHEMIN_KML << endl ;
sprintf( TmpChar , "rm -rf %s/%s/*.kml", path.c_str() , CHEMIN_KML ) ;
if ( system( TmpChar ) )
    cerr << "impossible de faire le rm de " << CHEMIN_KML << endl ;
CKmlFile KmlFile ;
for ( unsigned long iz = 0 ; iz < VecZone.size() ; iz++ )
    {
    CZone & Zone = *VecZone[iz] ;
    // fichier kml de la zone au sol
    KmlFile.WriteFile(Zone,iz) ;
    }

ResolutionMoyenneMetre /= VecZone.size() ;
ErreurMoyenneMetres /= VecZone.size() ;
cerr << "DeltaLatLon max toutes zones en km        : " << DelatLatLonMaxZoneKm << endl ;
cerr << "Resolution vario max short delta en metre : " << ResolutionMaxAllMetres << endl ;
cerr << "Resolution vario moyenne en metre         : " << setprecision(2) << ResolutionMoyenneMetre << endl ;
cerr << "Erreur moyenne toutes zone en metre       : " << setprecision(2) << ErreurMoyenneMetres << endl ;
cerr << "Erreur maximum toutes zone en metre       : " << setprecision(2) << ErreurMaxiMetres << endl ;
cerr << endl << "Fin du programme (err maxi admise verif " << CZone::ms_ErreurMaxiAdmiseVerif << " m)" << endl ;

return 0;
}
