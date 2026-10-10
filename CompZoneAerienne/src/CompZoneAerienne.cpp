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
/// \date 10/10/2026 : Reorganisation code. Et calcul dessous une zone (dans le perimetre insuffisant)
///

#include "CompZoneAerienne.h"

using namespace std;

char NumVer[]="20261010a" ;

CGlobalVar g_GlobalVar ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction main.
int main(int argc, char *argv[])
{

const long TailleBuffer = 10000 ;
char TmpChar[TailleBuffer] ;

// recuperation du repartoire courant
//string path = std::filesystem::current_path() ;
if ( getcwd( TmpChar , TailleBuffer ) == NULL )
    cerr << "imposible d'avoir le cwd" << endl ;
g_GlobalVar.m_path = TmpChar ;

// destruction des png et zone gnuplot
#ifdef WIN32
 chdir( "./zones_gnuplot" ) ;
 system( "del *.txt" ) ;
 system( "del *.png" ) ;
 chdir( "..") ;
#else
 sprintf( TmpChar , "rm -rf %s/%s/*.txt %s/%s/*.png", g_GlobalVar.m_path.c_str(), CHEMIN_GNUPLOT, g_GlobalVar.m_path.c_str(), CHEMIN_GNUPLOT ) ;
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
    cerr << "    ex pour paillaret: ./CompZoneAerienne 45.503415 2.852121 3" << endl ;
    cerr << "    ex mode batch    : ./CompZoneAerienne --batch terconnu.txt 60 > zonesaer.txt" << endl ;
    return 1 ;
    }

// fichier batch de rayon/centre terconnu.txt ou point/rayon
g_GlobalVar.TraiteLocaliteAutour( argc , argv ) ;

// ouverture fichier geojson
g_GlobalVar.TraiteFichierJson() ;

// compression des zones
cerr << "compression zone" << endl ;
for ( long iz = g_GlobalVar.m_VecZone.size() -1 ; iz >= 0 ; iz-- )
    {
    g_GlobalVar.m_VecZone[iz]->Compress() ;
    cerr << setw(5) << iz << "\r" << flush ;
    }

// tri de-croissant des zones compressees
std::sort( g_GlobalVar.m_VecZone.begin() , g_GlobalVar.m_VecZone.end() , MySortFonction ) ;
//std::reverse( VecZone.begin() , VecZone.end() ) ;

// ecriture des zones sur cout, fichier gnuplot
g_GlobalVar.EcritureFichierZones() ;

// images de comparaison de zones GnuPlot
g_GlobalVar.EcritureFichiersGnuPlot() ;

// fichier de comparaison de zones GoogleEarth
g_GlobalVar.EcritureFichiersKml() ;

// affichage erreur de calcul
cerr << endl ;
cerr << "DeltaLatLon max toutes zones en km        : " << g_GlobalVar.m_DelatLatLonMaxZoneKm << endl ;
cerr << "Resolution vario max short delta en metre : " << g_GlobalVar.m_ResolutionMaxAllMetres << endl ;
cerr << "Resolution vario moyenne en metre         : " << setprecision(2) << g_GlobalVar.m_ResolutionMoyenneMetre << endl ;
cerr << "Erreur moyenne toutes zone en metre       : " << setprecision(2) << g_GlobalVar.m_ErreurMoyenneMetres << endl ;
cerr << "Erreur maximum toutes zone en metre       : " << setprecision(2) << g_GlobalVar.m_ErreurMaxiMetres << endl ;
cerr << endl << "err maxi admise verif " << CZone::ms_ErreurMaxiAdmiseVerif << " m" << endl ;

// fin du programme
cerr << endl << "Fin du programme." << endl ;

return 0;
}
