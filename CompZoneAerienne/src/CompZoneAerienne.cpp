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

char NumVer[]="20261010b" ;

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

// traitement ligne de commande
std::vector<std::string> LigneDeCommande ;
long ilc ;
if ( argc < 4 )
    goto erreur ;
for ( ilc = 1 ; ilc < argc ; ilc++ )
    {
    // openair
    if ( strcmp(argv[ilc],"--openair") == 0 )
        g_GlobalVar.m_OpenAir = true ;
    // mode fichier
    else if ( strcmp(argv[ilc],"--batch") == 0 )
        {
        if ( argc < ilc + 3 )
            goto erreur ;
        for ( ; ilc < argc ; ilc++ )
            LigneDeCommande.push_back( argv[ilc] ) ;
        }
    // directe 1 point
    else if ( strcmp(argv[ilc],"--point") == 0 )
        {
        if ( argc < ilc + 3 )
            goto erreur ;
        for ( ilc++ ; ilc < argc ; ilc++ )
            LigneDeCommande.push_back( argv[ilc] ) ;
        }
    else
        {
        erreur :
        cerr << "version du " << NumVer << endl ;
        cerr << " " << argv[0] << " lat_deg lon_deg rayon_km " << endl ;
        cerr << " " << argv[0] << " --batch terconnu.txt rayon_km " << endl ;
        cerr << " " << argv[0] << " --point latitude longitude rayon_km " << endl ;
        cerr << " " << argv[0] << " --openair : pour donnees openair" << endl ;
        cerr << endl ;
        cerr << "    ex pour clermont : ./CompZoneAerienne --point 45.783329 3.08333 120 > zonesaer.txt" << endl ;
        cerr << "    ex pour paillaret: ./CompZoneAerienne --point 45.503415 2.852121 5" << endl ;
        cerr << "    ex mode batch    : ./CompZoneAerienne --batch terconnu.txt 60 > zonesaer.txt" << endl ;
        return 1 ;
        }
    }

// destruction des png et zone gnuplot
#ifdef WIN32
if ( g_GlobalVar.m_OpenAir )
    {
    chdir( "./zones_gnuplot_openair" ) ;
    system( "del *.txt" ) ;
    system( "del *.png" ) ;
    chdir( "..") ;
    }
else
    {
    chdir( "./zones_gnuplot_bazile" ) ;
    system( "del *.txt" ) ;
    system( "del *.png" ) ;
    chdir( "..") ;
    }
#else
if ( g_GlobalVar.m_OpenAir )
    {
    sprintf( TmpChar , "rm -rf %s/%s/*.txt %s/%s/*.png", g_GlobalVar.m_path.c_str(), CHEMIN_GNUPLOT_OPENAIR, g_GlobalVar.m_path.c_str(), CHEMIN_GNUPLOT_OPENAIR ) ;
    if ( system( TmpChar ) )
        cerr << "impossible de faire le rm de zones_gnuplot_openair" << endl ;
    }
else
    {
    sprintf( TmpChar , "rm -rf %s/%s/*.txt %s/%s/*.png", g_GlobalVar.m_path.c_str(), CHEMIN_GNUPLOT_BAZILE, g_GlobalVar.m_path.c_str(), CHEMIN_GNUPLOT_BAZILE ) ;
    if ( system( TmpChar ) )
        cerr << "impossible de faire le rm de zones_gnuplot_bazile" << endl ;
    }
#endif

// fichier batch de rayon/centre terconnu.txt ou point/rayon
g_GlobalVar.TraiteLocaliteAutour( LigneDeCommande ) ;

// ouverture fichier geojson ou openair
if ( g_GlobalVar.m_OpenAir )
    g_GlobalVar.TraiteRepertoireOpenAir() ;
else
    g_GlobalVar.TraiteFichierJson() ;

// compression des zones
cerr << "compression des zones" << endl ;
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
