///////////////////////////////////////////////////////////////////////////////
/// \file IgcRead.cpp
/// \date 21/01/2026 : date de creation
///
/// \brief Detourage des fichiers france. Averifier avec qgis, couche raster.
///
/// \date 21/01/2026 : Derniere modification.
///

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <filesystem> // C++17 et plus
#include <string>
#include <vector>
#include <fstream>

using namespace std;

string DateStr ;
string PiloteStr ;
string GliderStr ;

///////////////////////////////////////////////////////////////////////////////
/// \brief un point du fichier igc
class CDataLine
{
public :
    long m_HeureSecondes ;
    long m_Heure ;
    long m_Minutes ;
    long m_Secondes ;
    double m_Lat ;
    double m_Lon ;
    double m_Alti ;

    double m_VitesseVertFiltree = 0. ;
    double m_VitesseHorFiltree = 12. ;

    double GetDistanceMetre( const CDataLine & DataLine ) const
        { return 1852. * 60 * sqrt( pow(m_Lat-DataLine.m_Lat,2.) + pow(m_Lon-DataLine.m_Lon,2.) ) ; } ;
} ;

// vecteur de tous les points du fichier
std::vector< CDataLine > g_DataVector ;

/*
    // champ                 hh   mm   ss  LLlllll
    sprintf( m_pBuff , "B%02d%02d%02d%s%c%s%cA%05d%05d\n" ,
    (int) (secondes_date/3600) ,    // heure
    (int)((secondes_date/60)%60) ,  // minutes
    (int)(secondes_date%60) ,       // secondes
    LocalLat , ((SigneLat>=0.) ? 'N' : 'S') ,  // lat
    LocalLon , ((SigneLon>=0.) ? 'E' : 'W') ,  // lon
    (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ,
    //(int)g_GlobalVar.m_AltiGps
    (int)0
    ) ;
    */

///////////////////////////////////////////////////////////////////////////////
/// \brief Traite une ligne de fichier igc
void TraiteLigne( char * Ligne )
{
char * pChar ;
CDataLine dataline ;
// heure
dataline.m_Heure   = (Ligne[1]-'0')*10+(Ligne[2]-'0') ;
dataline.m_Minutes = (Ligne[3]-'0')*10+(Ligne[4]-'0') ;
dataline.m_Secondes = (Ligne[5]-'0')*10+(Ligne[6]-'0') ;
dataline.m_HeureSecondes = dataline.m_Heure*3600 + dataline.m_Minutes * 60 + dataline.m_Secondes ;

// altitude
Ligne[30] = 0 ;
pChar = &Ligne[25] ;
sscanf( pChar ,"%lf" , & dataline.m_Alti ) ;

//longitude
pChar = & Ligne[15] ;
float Lon = 0. ;
Lon += ((float)(pChar[0]-'0')) * 100 ;       // 100 ene de lon
Lon += ((float)(pChar[1]-'0')) * 10 ;        // 10 ene de lon
Lon += ((float)(pChar[2]-'0')) ;             // lon deg
Lon += ((float)(pChar[3]-'0')) * 10. / 60. ; // lon 10 ene de minutes
Lon += ((float)(pChar[4]-'0')) / 60. ;       // lon minutes

Lon += ((float)(pChar[5]-'0')) / 60. / 10. ;  // lon 10ieme minutes
Lon += ((float)(pChar[6]-'0')) / 60. / 100. ; // lon 100ieme minutes
Lon += ((float)(pChar[7]-'0')) / 60. / 1000. ; // lon 1000ieme minutes
dataline.m_Lon = Lon ;
//if ( Ligne[23] != 'E' )
if ( pChar[8] != 'E' )
    dataline.m_Lon *= -1 ;

// latitude
pChar = & Ligne[7] ;
float Lat = 0. ;
Lat += ((float)(pChar[0]-'0')) * 10 ;        // 10 ene de lat
Lat += ((float)(pChar[1]-'0')) ;             // lat deg
Lat += ((float)(pChar[2]-'0')) * 10. / 60. ; // lat 10 ene de minutes
Lat += ((float)(pChar[3]-'0')) / 60. ;       // lat minutes

Lat += ((float)(pChar[4]-'0')) / 60. / 10. ;  // lat 10ieme minutes
Lat += ((float)(pChar[5]-'0')) / 60. / 100. ; // lat 100ieme minutes
Lat += ((float)(pChar[6]-'0')) / 60. / 1000. ; // lat 1000ieme minutes
//Lat += ((float)(pChar[7]-'0')) / 60. / 10000. ; // lat 10000ieme minutes
dataline.m_Lat = Lat ;
//if ( Ligne[14] != 'N' )
if ( pChar[7] != 'N' )
    dataline.m_Lat *= -1 ;


// ajout de la ligne
g_DataVector.push_back( dataline ) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Filtre et lisse les vitesses.
void CalcFiltrage()
{
unsigned long NbFiltre = 4 ;
for ( unsigned long ivv = NbFiltre ; ivv < g_DataVector.size() ; ivv++ )
    {
    // vitesse verticale
    double DeltaAti = g_DataVector[ivv].m_Alti - g_DataVector[ivv-NbFiltre].m_Alti ;
    double DeltaT = g_DataVector[ivv].m_HeureSecondes - g_DataVector[ivv-NbFiltre].m_HeureSecondes ;
    double VitVert = 0. ;
    if ( DeltaT > 0. )
        {
        VitVert = DeltaAti/DeltaT ;
        g_DataVector[ivv].m_VitesseVertFiltree = VitVert ;
        }

    // vitesse horizontale
    if ( DeltaT > 0. )
        {
        double VitHor = 0 ;
        for ( unsigned long ivh = 0 ; ivh < NbFiltre -1  ; ivh++ )
            VitHor += g_DataVector[ivv-ivh].GetDistanceMetre( g_DataVector[ivv-ivh-1] ) ;
        if ( VitHor > 0. )
            g_DataVector[ivv].m_VitesseHorFiltree = VitHor/DeltaT * 3.6  ;
        }

    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
int main( int argc , char * argv[] )
{
// si pas d'argument
if ( argc < 2 || !strcmp(argv[1],"--help") )
    {
    cerr << " version du 07-02-2026 b" << endl ;
    cerr << argv[0] << " nom_igc" << endl ;
    return 1 ;
    }

// ouverture du fichier igc
std::string NomFchIgc ;
NomFchIgc = argv[1] ;

cout << "Fichier Igc : " << NomFchIgc << endl << endl ;

std::ifstream FchIn( NomFchIgc.c_str() ) ;
if ( ! FchIn.is_open() )
    {
    cerr << "impossibilite d'ouvrir le fichier : " << NomFchIgc << endl ;
    return 2 ;
    }

// vecteur de mots
vector <string> tokens;
std::string line ;
while (getline(FchIn, line))
    {
    // si ligne vide
    char * pChar = &line[0] ;
    if ( *pChar == 0 )
        continue ;

    // decoupage en mots
    tokens.clear() ;
    pChar = strtok( pChar , " :") ;
    while ( pChar != NULL )
        {
        tokens.push_back( pChar ) ;
        pChar = strtok( NULL , " :\r\n")  ;
        }

    // si entete du fichier
    if ( tokens.size() == 0 || tokens[0] == "" )
        continue ;
    else if ( tokens[0] == "HFDTEDATE" )
        DateStr = tokens[1] ;
    else if ( tokens[0] == "HFPLTPILOTINCHARGE" )
        for ( unsigned int i = 1 ; i < tokens.size() ; i++ )
            {
            PiloteStr += tokens[i] ;
            if ( i < tokens.size() )
                PiloteStr += " " ;
            }
    else if ( tokens[0] == "HFGTYGLIDERTYPE" )
        {
        for ( unsigned int i = 1 ; i < tokens.size() ; i++ )
            {
            GliderStr += tokens[i] ;
            if ( i < tokens.size() )
                GliderStr += " " ;
            }
        }
    // si point du fichier
    else if ( tokens[0][0] == 'B' )
        TraiteLigne( &tokens[0][0] ) ;
    }

FchIn.close() ;

// si fichier vide
if ( g_DataVector.size() == 0 )
    {
    cerr << "pas de donnees igc" << endl ;
    return 3 ;
    }

// date et temps de vol
cout << "date   : 20" << DateStr[4] << DateStr[5] << "-" << DateStr[2] << DateStr[3] << "-" << DateStr[0] << DateStr[1] << endl ;
cout << "pilote : " << PiloteStr << endl ;
cout << "voile  : " << GliderStr << endl << endl;

cout << "debut du vol : " << g_DataVector[0].m_Heure << "h " << g_DataVector[0].m_Minutes << "m " << g_DataVector[0].m_Secondes << "s" << endl ;
cout << "fin du vol   : " << g_DataVector[g_DataVector.size()-1].m_Heure << "h " << g_DataVector[g_DataVector.size()-1].m_Minutes << "m "
                            << g_DataVector[g_DataVector.size()-1].m_Secondes << "s" << endl ;
long SecondesVol = g_DataVector[g_DataVector.size()-1].m_HeureSecondes - g_DataVector[0].m_HeureSecondes ;
cout << "duree du vol : " << setprecision(3) << SecondesVol/3600. << "h, ou " << (SecondesVol) / 60. << "m, ou " << SecondesVol << "sec "
        << endl << endl ;

CalcFiltrage() ;

// vitesse verticale sur Nb echantillon
double VitVertMax = 0 ;
double VitVertMin = 100 ;
for ( unsigned long is = 0 ; is < g_DataVector.size() ; is ++ )
    {
    double VitVert = g_DataVector[is].m_VitesseVertFiltree ;
    VitVertMax = std::max( VitVertMax , VitVert ) ;
    VitVertMin = std::min( VitVertMin , VitVert ) ;
    //cout << setprecision(8) << g_DataVector[is].m_Lon << " " << g_DataVector[is].m_Lat << endl ;
    }

cout << "vitesse vertical max : " << VitVertMax << " m/s" << endl ;
cout << "vitesse vertical min : " << VitVertMin << " m/s" << endl << endl ;

// altitudes decollage/atterissage
long AltitudeMax = 0 ;
for ( unsigned long is = 0 ; is < g_DataVector.size() ; is ++ )
    AltitudeMax = std::max( AltitudeMax , (long)g_DataVector[is].m_Alti ) ;
float DeltaZ = (int)(g_DataVector[g_DataVector.size()-1].m_Alti-g_DataVector[0].m_Alti) ;
cout << "Z decollage         : " << setw(6) << (int)g_DataVector[0].m_Alti << "m" << endl ;
cout << "Z atterrissage      : " << setw(6) << (int)g_DataVector[g_DataVector.size()-1].m_Alti << "m" << endl ;
cout << "delta Z deco/attero : " << setw(6) << -(int)(DeltaZ) << "m" << endl ;
cout << "altitude max        : " << setw(6) << AltitudeMax << "m" << endl ;
cout << "gain max            : " << setw(6) << AltitudeMax - g_DataVector[0].m_Alti << "m" << endl << endl ;

// distance parcourue
double distance = 0 ;
double VitesseHorMax = 0. ;
double VitesseHorMin = 10000. ;
double VitesseHorMoyenne ;
for ( unsigned long is = 0 ; is < g_DataVector.size() - 1 ; is ++ )
    distance += g_DataVector[is].GetDistanceMetre( g_DataVector[is+1] ) ;
VitesseHorMoyenne = distance / (g_DataVector[g_DataVector.size() - 1].m_HeureSecondes - g_DataVector[0].m_HeureSecondes) * 3.6 ;

// vitesse horizontale min max
for ( unsigned long is = 0 ; is < g_DataVector.size() ; is ++ )
    {
    VitesseHorMax = std::max( VitesseHorMax , g_DataVector[is].m_VitesseHorFiltree ) ;
    VitesseHorMin = std::min( VitesseHorMin , g_DataVector[is].m_VitesseHorFiltree ) ;
    //cout << setprecision(10) << g_DataVector[is].m_VitesseHorFiltree << endl ;
    }


cout << "vitesse horizontale moyenne : " << setw(6) << VitesseHorMoyenne << "km/h" << endl ;
cout << "vitesse horizontale max     : " << setw(6) << VitesseHorMax << "km/h" << endl ;
cout << "vitesse horizontale min     : " << setw(6) << VitesseHorMin << "km/h" << endl << endl ;

// excursion en distance
double ExcursionMax = 0. ;
for ( unsigned long ia = 0 ; ia < g_DataVector.size() ; ia++ )
    for ( unsigned long ib = ia + 1 ; ib < g_DataVector.size() ; ib++ )
        {
        ExcursionMax = std::max( ExcursionMax , g_DataVector[ia].GetDistanceMetre(g_DataVector[ib]) ) ;
        }
cout << "distance parcourue : " << setprecision(3) << (distance/1000) << "km" << endl ;
cout << "excursion maximum  : " << (long)ExcursionMax << "m" << endl << endl ;

// creation fichier kml
std::string NomFchKml = NomFchIgc + ".kml" ;

std::ofstream out(NomFchKml);

out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<kml xmlns=\"http://www.opengis.net/kml/2.2\"\
     xmlns:gx=\"http://www.google.com/kml/ext/2.2\"\
     xmlns:kml=\"http://www.opengis.net/kml/2.2\"\
     xmlns:atom=\"http://www.w3.org/2005/Atom\">" << endl ;

out << "<Document>\n";
out << "<name>" << NomFchIgc << "</name>\n";

for (size_t i = 1 ; i < g_DataVector.size(); ++i)
    {
    double climbRate = g_DataVector[i].m_VitesseVertFiltree; // m/s

    // Color based on climb rate
    std::string color ;
    if ( climbRate > 4. )
        color = "ff0000ff" ; // rouge
    else if ( climbRate > 3. )
        color = "ff3399ff" ;    // orange
    else if ( climbRate > 2. )
        color = "ff00ffff" ;    // jaune
    else if ( climbRate > 1. )
        color = "ffb2ff66" ;    // vert clair
    else if ( climbRate > 0.5 )
        color = "ffffffff" ;    // blanc
    else if ( climbRate > 0 )
        color = "ffa0a0a0" ;    // gris
    else if ( climbRate > -0.5 )
        color = "ffffff00" ;    // blue clair
    else if ( climbRate > -1.5 )
        color = "ffff0000" ;    // blue fonce
    else if ( climbRate > -2.5 )
        color = "ffff00ff" ;    // violet
    else
        color = "ff000000" ; // noire

    // ecriture du point
    out << "<Placemark>\n";
    out << "<Style><LineStyle><color>" << color << "</color><width>2</width></LineStyle></Style>\n";
    out << "\t<LineString>" << endl ;
    //out << "<tessellate>1</tessellate>" << endl ;
		out << "\t\t" << "<coordinates>"
                        << std::setprecision(10) << g_DataVector[i - 1].m_Lon << "," << g_DataVector[i - 1].m_Lat << "," << std::setprecision(0) << ((long)g_DataVector[i - 1].m_Alti)
                        << " "
		                << std::setprecision(10) << g_DataVector[i].m_Lon     << "," << g_DataVector[i].m_Lat     << "," << std::setprecision(0) << ((long)g_DataVector[i].m_Alti)
		    << "</coordinates>" << endl ;
		out << "\t\t" << "<altitudeMode>absolute</altitudeMode>" << endl ;
    out << "\t</LineString>" << endl ;
    out << "</Placemark>\n";
}

// fermeture du fichier
out << "</Document>\n</kml>\n";
out.close();

std::cout << "KML genere: " << NomFchKml << endl << endl ;

return 0;
}
