////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.cpp
///
/// \brief
///
/// \date creation     : 10/10/2026
/// \date modification : 10/10/2026
///

#include "CompZoneAerienne.h"

#include "BertheVarioIncludes.h"

using namespace std ;
using namespace nlohmann ;

////////////////////////////////////////////////////////////////////////////////
/// \brief DEcoupage en mots.
void CGlobalVar::split(const string &chaine, vector<string> &elements)
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

#ifdef _COMP_ZONES_AERIENNES_
////////////////////////////////////////////////////////////////////////////////
/// \brief Procedure de validation CompZoneAeriennes.
long CGlobalVar::ValidCompZoneAeriennes( const char * zonesaer_txt , const char * valid_txt )
{
CZonesFchValid ValidZone ;

ValidZone.Valid( zonesaer_txt , valid_txt ) ;

return 0 ;
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief traite les points autour desquelle sont les zones
long CGlobalVar::TraiteLocaliteAutour(std::vector<std::string> &LigneDeCommande)
{
if ( LigneDeCommande[0] == "--batch" )
    {
    ifstream ifs_zprc( LigneDeCommande[1] ) ;
    double RayonKm = atof( LigneDeCommande[2].c_str() ) ;
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

        m_VectPtCentreRayon.push_back( CmdLineParam ) ;
        }
    ifs_zprc.close() ;
    }
// parametres de la ligne de commande
else
    {
    CCmdLineParam CmdLineParam ;

    CmdLineParam.m_LatCentreDeg = atof( LigneDeCommande[0].c_str() ) ;
    CmdLineParam.m_LonCentreDeg = atof( LigneDeCommande[1].c_str() ) ;
    CmdLineParam.m_RayonDeg = atof( LigneDeCommande[2].c_str() ) * 1000. / UnMilesEnMetres / MilesParDegres ;

    m_VectPtCentreRayon.push_back( CmdLineParam ) ;
    }

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief traite le fichier json de bazille
long CGlobalVar::TraiteFichierJson()
{
std::ifstream ifs(IN_ZONE_FILE_JSON);
if ( ! ifs.is_open() )
    {
    cerr << "**** impossible d'ouvrir : " << IN_ZONE_FILE_JSON << endl ;
    exit( 1 ) ;
    }
json jf = json::parse(ifs);

// nombre de zonnes totales
long NbAreas = jf["headerFile"]["numberOfAreas"] ;

// pour tout les points centre de recherche
for ( size_t iptcentre = 0 ; iptcentre < m_VectPtCentreRayon.size() ; iptcentre++ )
    {
    const CCmdLineParam & CmdLineParam = m_VectPtCentreRayon[iptcentre] ;
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
        if ( strstr( pZone->m_Name.c_str() , "LTA FRANCE 1" ) != NULL ||
             strstr( pZone->m_Name.c_str() , "France-Metropolitaine" ) != NULL )
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

        // si dans perimetre ou dessous ajout de la zone
        if ( VerificationZoneAdmissible( pZone, LatCentreDeg, LonCentreDeg, RayonDeg ) )
            m_VecZones.push_back( pZone ) ;
        //else
        //    delete pZone ;
        }
    }

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Verification si zone dans le perimetre ou au dessus et pas deja faite.
bool CGlobalVar::VerificationZoneAdmissible(const CZone* pZone, double LatCentreDeg, double LonCentreDeg, double RayonDeg)
{
// si zone deja faite
for ( size_t izdf = 0 ; izdf < m_VecZones.size() ; izdf++ )
    if ( *m_VecZones[izdf] == *pZone )
        return false ;

// calcul dans le perimetre mais pas dessous
for ( long nbc = 0 ; nbc < (long)pZone->m_VecPtsBig.size() ; nbc++ )
    {
    double DistanceAngu = sqrtf( powf( pZone->m_VecPtsBig[nbc]->m_Lon - LonCentreDeg , 2 ) + powf( pZone->m_VecPtsBig[nbc]->m_Lat - LatCentreDeg , 2 ) ) ;
    if ( DistanceAngu < RayonDeg )
        return true ;
    }

// calcul si dessous la zone
CVecZoneReduce::st_coord_poly StTerrainCourent ;
StTerrainCourent.m_Lat = LatCentreDeg ;
StTerrainCourent.m_Lon = LonCentreDeg ;
if ( CPolygone::IsIn( (const CVecZoneReduce::st_coord_poly **) pZone->m_VecPtsBig.data() , pZone->m_VecPtsBig.size() , StTerrainCourent ) )
    return true ;

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture repertoire openair pour creation de zones
long CGlobalVar::TraiteRepertoireOpenAir()
{
// tableau des noms de fichier
std::vector<std::string> NomFchArr ;

// ouverture du repertoire
DIR * rep = opendir(IN_ZONE_DIR_OPENAIR);
if (rep == NULL)
    {
    cout << "probleme lecture repertoire " << IN_ZONE_DIR_OPENAIR << endl ;
    exit( 1 ) ;
    }

// lecture des noms de fichier
struct dirent * ent;
while ((ent = readdir(rep)) != NULL)
    {
    std::string NomFch = IN_ZONE_DIR_OPENAIR ;
    NomFch += "/" ;
    NomFch += ent->d_name ;

    if ( NomFch != "." && NomFch != ".." )
        NomFchArr.push_back(NomFch) ;
    }
closedir(rep);

// pour tous les fichiers
for ( long iff = 0 ; iff < (long)NomFchArr.size() ; iff++ )
    {
    FILE *fp = fopen(NomFchArr[iff].c_str(), "r");
    if (!fp)
        {
        cerr << "**** impossible d'ouvrir : " << NomFchArr[iff] << endl ;
        exit( 2 ) ;
        }

    // traitement du fichier
    COpenAirFile OpenAirFile ;
    OpenAirFile.TraiteFichier( fp ) ;

    // fermeture du fichier
    fclose(fp) ;

    // ajout du premier point a la fin pour boucle le dernier segment
    // pour toutes les zone du fichier en cours
    for ( long iz = 0 ; iz < (long) OpenAirFile.m_pZoneArr.size() ; iz++ )
        {
        CZone * pZone = OpenAirFile.m_pZoneArr[iz] ;

        if ( pZone->m_VecPtsBig.size() == 0 )
            {
            cerr << "**** zone de taille nulle : " << pZone->m_Name << endl ;
            continue ;
            }

        CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
        *pStPts = *pZone->m_VecPtsBig[ 0 ] ;
        pZone->m_VecPtsBig.push_back( pStPts ) ;
        }

    // pour tout les points centre de recherche
    for ( size_t iptcentre = 0 ; iptcentre < m_VectPtCentreRayon.size() ; iptcentre++ )
        {
        const CCmdLineParam & CmdLineParam = m_VectPtCentreRayon[iptcentre] ;
        double LonCentreDeg = CmdLineParam.m_LonCentreDeg ;
        double LatCentreDeg = CmdLineParam.m_LatCentreDeg ;
        double RayonDeg     = CmdLineParam.m_RayonDeg ;

        // pour toutes les zone du fichier en cours
        for ( long iz = 0 ; iz < (long) OpenAirFile.m_pZoneArr.size() ; iz++ )
            {
            CZone * pZone = OpenAirFile.m_pZoneArr[iz] ;

            if ( pZone == NULL )
                continue ;

            // si dans perimetre ou dessous ajout de la zone
            if ( VerificationZoneAdmissible( pZone, LatCentreDeg, LonCentreDeg, RayonDeg ) )
                m_VecZones.push_back( pZone ) ;
            // pas de delete car plusieur fichier avec meme zone
            //else
            //    delete pZone ;
            }
        }
    }

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture du fichier de zones pour BertheVario
long CGlobalVar::EcritureFichierZones()
{
// entete fichier
cout << "#Nom Zone;alti basse;lon0_deg,lat0_deg;lon1_deg,lat1_deg" << endl ;

for ( unsigned long iz = 0 ; iz < m_VecZones.size() ; iz++ )
    {
    CZone & Zone = *m_VecZones[iz] ;

    // ecriture pour fichier texte sur cout
    cout << Zone.m_Name << ";" << Zone.m_Bottom << ";" ;
    for ( long nbc = 0 ; nbc < (long)Zone.m_VecPtsSmall.size() ; nbc++ )
        cout << "" << setprecision(9) << Zone.m_VecPtsSmall[nbc]->m_Lon << "," << Zone.m_VecPtsSmall[nbc]->m_Lat << ";" ;
    cout << endl ;

    // resolution zone
    double DelatLatLonZoneKm   = Zone.m_DeltaLatLonMax ;
    double ResolutionZoneMetre = Zone.m_ResolVario ;
    m_ResolutionMoyenneMetre  += ResolutionZoneMetre ;
    m_ResolutionMaxAllMetres   = MAX ( m_ResolutionMaxAllMetres , ResolutionZoneMetre ) ;
    m_DelatLatLonMaxZoneKm     = MAX( m_DelatLatLonMaxZoneKm , DelatLatLonZoneKm ) ;
    m_ErreurMaxiMetres         = MAX( Zone.m_ErreurMaxEnMetre , m_ErreurMaxiMetres ) ;
    m_ErreurMoyenneMetres     += Zone.m_ErreurMoyenneEnMetre ;

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

m_ResolutionMoyenneMetre /= m_VecZones.size() ;
m_ErreurMoyenneMetres /= m_VecZones.size() ;

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture des images GnuPlot pour comparaison des formes d'origine et
/// compressee.
long CGlobalVar::EcritureFichiersGnuPlot()
{
const long TailleBuffer = 10000 ;
char TmpChar[TailleBuffer] ;

// pour gnu plot
#ifndef WIN32
if ( m_OpenAir )
    cerr << "creation fichier gnuplot dans " << CHEMIN_GNUPLOT_OPENAIR << endl ;
else
    cerr << "creation fichier gnuplot dans " << CHEMIN_GNUPLOT_BAZILE << endl ;
for ( unsigned long iz = 0 ; iz < m_VecZones.size() ; iz++ )
    {
    CZone & Zone = *m_VecZones[iz] ;
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
    if ( m_OpenAir )
        sprintf(NomFichierGnuplotAvecChemin,"./%s/%03ld_avc_%s.txt",CHEMIN_GNUPLOT_OPENAIR,iz,NomZoneSansBlanc.c_str()) ;
    else
        sprintf(NomFichierGnuplotAvecChemin,"./%s/%03ld_avc_%s.txt",CHEMIN_GNUPLOT_BAZILE,iz,NomZoneSansBlanc.c_str()) ;
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
    if ( m_OpenAir )
        sprintf(NomFichierGnuplotAvecChemin,"./%s/%03ld_apc_%s.txt",CHEMIN_GNUPLOT_OPENAIR,iz,NomZoneSansBlanc.c_str()) ;
    else
        sprintf(NomFichierGnuplotAvecChemin,"./%s/%03ld_apc_%s.txt",CHEMIN_GNUPLOT_BAZILE,iz,NomZoneSansBlanc.c_str()) ;
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

    if ( m_OpenAir )
        sprintf(NomFichierGnuplotNomDeZoneSansBlanc,"./%s/%03ld_txt_%s.txt",CHEMIN_GNUPLOT_OPENAIR, iz, NomZoneSansBlanc.c_str() ) ;
    else
        sprintf(NomFichierGnuplotNomDeZoneSansBlanc,"./%s/%03ld_txt_%s.txt",CHEMIN_GNUPLOT_BAZILE, iz, NomZoneSansBlanc.c_str() ) ;
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
                        m_path.c_str(), ((m_OpenAir) ? CHEMIN_GNUPLOT_OPENAIR : CHEMIN_GNUPLOT_BAZILE) ,
                        NomFichierAvc.c_str(),
                        NomFichierApc.c_str(),
                        m_path.c_str(), ((m_OpenAir) ? CHEMIN_GNUPLOT_OPENAIR : CHEMIN_GNUPLOT_BAZILE), iz, NomZoneSansBlanc.c_str() ,
                        '"' , Zone.m_Name.c_str() , Zone.m_DeltaLatLonMax , Zone.m_Bottom ,
                        Zone.m_VecPtsBig.size() , Zone.m_VecPtsSmall.size() ,
                        Zone.m_ResolVario , Zone.m_ErreurMoyenneEnMetre, Zone.m_ErreurMaxEnMetre,
                        '"' ) ;
    //cerr << TmpChar << "\r" << flush ;
     if ( system( TmpChar ) )
        cerr << "impossible de faire zones_gnuplot/gnuplot.sh" << endl ;
    }
#endif

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture des fichiers kml pour googleearth
long CGlobalVar::EcritureFichiersKml()
{
const long TailleBuffer = 10000 ;
char TmpChar[TailleBuffer] ;

// creation des fichier kml
if ( g_GlobalVar.m_OpenAir )
    {
    cerr << "creation fichier kml dans " << CHEMIN_KML_OPENAIR << endl ;
    sprintf( TmpChar , "rm -rf %s/%s/*.kml", m_path.c_str() , CHEMIN_KML_OPENAIR ) ;
    if ( system( TmpChar ) )
        cerr << "impossible de faire le rm de " << CHEMIN_KML_OPENAIR << endl ;
    }
else
    {
    cerr << "creation fichier kml dans " << CHEMIN_KML_BAZIL << endl ;
    sprintf( TmpChar , "rm -rf %s/%s/*.kml", m_path.c_str() , CHEMIN_KML_BAZIL ) ;
    if ( system( TmpChar ) )
        cerr << "impossible de faire le rm de " << CHEMIN_KML_BAZIL << endl ;
    }

CKmlFile KmlFile ;
for ( unsigned long iz = 0 ; iz < m_VecZones.size() ; iz++ )
    {
    CZone & Zone = *m_VecZones[iz] ;
    // fichier kml de la zone au sol
    KmlFile.WriteFile(Zone,iz) ;
    }

return 0 ;
}

