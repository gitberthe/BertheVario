///////////////////////////////////////////////////////////////////////////////
/// \file GestHgt.cpp
/// \date 17/01/2026 : date de creation
///
/// \brief Detourage des fichiers france. Averifier avec qgis, couche raster.
///
/// \date 14/01/2026 : Derniere modification.
///

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <filesystem> // C++17 et plus
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

// cadre des fichier a garder
int LongGauche = -5 ;
int LongDroite =  7 ;
int LatHaut    = 50 ;
int LatBas     = 42 ;

enum  CmdLineOpt
    {
    OptDeleteNoFrance ,
    OptCreate ,
    OptVerify ,
    Error ,
    } ;

CmdLineOpt g_OptLineCmd = Error ;

///////////////////////////////////////////////////////////////////////////////
/// \brief
int FonctionDeleNoFrance( const char * NomRep )
{
// path courant
fs::path currentPath = fs::current_path();

// test si directori existante
if (!fs::exists(NomRep))
    {
    cerr << "dossier non present : " << NomRep << endl ;
    return 2 ;
    }

std::vector<std::string> ListeDesFichiersTrouve ;
for (const auto& entry : fs::directory_iterator(NomRep))
    {
    if (fs::is_directory(entry) )
        continue ;
    ListeDesFichiersTrouve.push_back( entry.path().filename().string() ) ;
    }

// liste des fichier autorise
std::vector<std::string> ListeDesFichiersAutorise ;
cout << "liste des fichiers autorise France:" << endl ;
char TmpChar[2048] ;
for ( int Longitude = LongGauche ; Longitude <= LongDroite ; Longitude++ )
    for ( int Latitude = LatBas ; Latitude <= LatHaut ; Latitude++ )
        {
        char CharLongitude = ( Longitude < 0 ) ? 'W' : 'E' ;
        sprintf( TmpChar , "%c%02d%c%03d.hgt" , 'N' , Latitude ,  CharLongitude , abs(Longitude) ) ;
        ListeDesFichiersAutorise.push_back( TmpChar ) ;
        cout << TmpChar << " " ;
        }
cout << endl ;

// liste des fichiers dans le repetoire
for ( unsigned int ifiled = 0 ; ifiled < ListeDesFichiersTrouve.size() ; ifiled++ )
    {
    bool ASupprimer = true ;
    std::string & NomFchTrouve = ListeDesFichiersTrouve[ifiled] ;
    // liste des fichier autorise
    for ( unsigned int ifilen = 0 ; ifilen < ListeDesFichiersAutorise.size() ; ifilen++ )
        {
        if ( strstr( ListeDesFichiersTrouve[ifiled].c_str() ,ListeDesFichiersAutorise[ifilen].c_str() ) )
            {
            ASupprimer = false ;
            break ;
            }
        }
    if ( ASupprimer )
        {
        std::string NomASupprimer = NomRep ;
        NomASupprimer += NomFchTrouve ;
        int returnCode = remove( NomASupprimer.c_str() );
        cout << "supression de : " << NomASupprimer << endl ;
        if ( returnCode != 0 )
            cerr << "Error: cannot remove " << NomFchTrouve << endl ;
        }
    }

return 0 ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
int FonctionCreateChks( const char * NomRep )
{
// path courant
fs::path currentPath = fs::current_path();

// test si directori existante
if (!fs::exists(NomRep))
    {
    cerr << "dossier non present : " << NomRep << endl ;
    return 2 ;
    }

std::vector<std::string> ListeDesFichiersTrouve ;

// fichier pour cheksum
for (const auto& entry : fs::directory_iterator(NomRep))
    {
    if (fs::is_directory(entry) )
        continue ;
    ListeDesFichiersTrouve.push_back( entry.path().filename().string() ) ;
    }

for ( unsigned int ifilec = 0 ; ifilec < ListeDesFichiersTrouve.size() ; ifilec++ )
    {
    std::string NomFch = ListeDesFichiersTrouve[ifilec] ;
    cout << ifilec << " calcul checksum pour : " << NomFch << endl ;

    // fichier in
    std::string NomFchComplet = NomRep ;
    NomFchComplet += NomFch ;

    std::string NomFchCsk = NomRep ;
    NomFchCsk += "/chks/" + NomFch ;
    NomFchCsk.resize(NomFchCsk.size()-4) ;
    NomFchCsk += ".chk" ;
    //cout << NomFchCsk << endl ;

    // ouverture en lecture pour fch in
    FILE * FchIn ;
    FchIn = fopen( NomFchComplet.c_str() , "r" ) ;
    if ( !FchIn )
        {
        cerr << "imposibilite d'ouvrir : " << NomFchComplet << endl ;
        continue ;
        }

    // buffer fch in et checksum
    unsigned short Buff ;
    unsigned short Chks = 0 ;

    // calcul de la checksum pour fch in
    int NbRead = 0 ;
    size_t bytesRead ;
    while( (bytesRead=fread( &Buff , 1 , sizeof(Buff) , FchIn )) > 0 )
        {
        Chks ^= Buff ;
        NbRead++ ;
        }

    // verification lecture totale de fch in
    if ( NbRead != 1201*1201 )
        {
        cerr << "**** erreur fichier tronque pour : " << NomFch << endl ;
        continue ;
        }

    // fermeture fch in
    fclose( FchIn ) ;

    // creation du fichier ckecksum
    FILE * FchChks ;
    FchChks = fopen( NomFchCsk.c_str() , "w+" ) ;
    if ( !FchChks )
        {
        cerr << "imposibilite de creer : " << NomFchCsk << endl ;
        continue ;
        }

    // ecriture checksum
    char TmpChar[10] ;
    sprintf( TmpChar , "%04X\n" , Chks ) ;
    fwrite( TmpChar , 1 , strlen(TmpChar) , FchChks ) ;

    // fermeture checksum
    fclose( FchChks ) ;
    }

return 0 ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
int FonctionVerifyChks( const char * NomRep )
{
// path courant
fs::path currentPath = fs::current_path();

// test si directori existante
if (!fs::exists(NomRep))
    {
    cerr << "dossier non present : " << NomRep << endl ;
    return 2 ;
    }

std::vector<std::string> ListeDesFichiersTrouve ;

// fichier pour cheksum
for (const auto& entry : fs::directory_iterator(NomRep))
    {
    if ( fs::is_directory(entry) )
        continue ;
    if ( strstr( entry.path().filename().string().c_str() , ".hgt" ) == NULL )
        continue ;
    ListeDesFichiersTrouve.push_back( entry.path().filename().string() ) ;
    }

if ( ListeDesFichiersTrouve.size() == 0 )
    {
    cerr << "**** pas de fichier .hgt trouve !!!" << endl ;
    return 3 ;
    }

int NbErrChks = 0 ;
for ( unsigned int ifilec = 0 ; ifilec < ListeDesFichiersTrouve.size() ; ifilec++ )
    {
    std::string NomFch = ListeDesFichiersTrouve[ifilec] ;

    // fichier in
    std::string NomFchComplet = NomRep ;
    NomFchComplet += NomFch ;

    std::string NomFchHgt = NomRep ;
    NomFchHgt += "/" + NomFch ;
    //cout << NomFchCsk << endl ;

    // ouverture en lecture pour fch in
    FILE * FchIn ;
    FchIn = fopen( NomFchHgt.c_str() , "r" ) ;
    if ( !FchIn )
        {
        NbErrChks++ ;
        cerr << "imposibilite de lire : " << NomFchHgt << endl ;
        continue ;
        }

    // buffer fch in et checksum
    unsigned short Buff ;
    unsigned short ChksCalc = 0 ;

    // calcul de la checksum pour fch in
    int NbRead = 0 ;
    size_t bytesRead ;
    while( (bytesRead=fread( &Buff , 1 , sizeof(Buff) , FchIn )) > 0 )
        {
        ChksCalc ^= Buff ;
        NbRead++ ;
        }

    // verification lecture totale de fch in
    if ( NbRead != 1201*1201 )
        {
        NbErrChks++ ;
        cerr << "**** erreur fichier tronque pour : " << NomFch << endl ;
        continue ;
        }

    // fermeture fch in
    fclose( FchIn ) ;

    // nom fichier checksum
    std::string NomFchCsk = NomRep ;
    NomFchCsk += "chks/" + NomFch ;
    NomFchCsk.resize(NomFchCsk.size()-4) ;
    NomFchCsk += ".chk" ;

    // lecture du fichier ckecksum
    FILE * FchChks ;
    FchChks = fopen( NomFchCsk.c_str() , "r" ) ;
    if ( !FchChks )
        {
        NbErrChks++ ;
        cerr << "impossibilite de d'ouvrir : " << NomFchCsk << endl ;
        continue ;
        }

    // verification checksum
    char TmpChar[10] ;
    unsigned int ChksFch2Verif = 0 ;
    if ( fread( &TmpChar , 4 , sizeof(char) , FchChks ) < 1 )
        {
        NbErrChks++ ;
        cerr << "impossibilite de lire : " << NomFchCsk <<  " " << TmpChar << endl ;
        continue ;
        }
    sscanf( TmpChar , "%X" , &ChksFch2Verif ) ;

    // fermeture checksum
    fclose( FchChks ) ;

    if ( ChksCalc != ChksFch2Verif )
        {
        NbErrChks++ ;
        cout << ifilec << "**** erreur de checksum pour : " << NomFchCsk << endl ;
        }
    else
        cout << ifilec << " checksum pour : " << NomFch << " ok" << endl ;
    }

if ( NbErrChks )
    cout << NbErrChks << " erreur cheksum !!!!" << endl ;
else
    cout << "toutes cheksum ok !" << endl ;

return 0 ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
int main( int argc , char ** argv )
{
// ligne de commande
if ( argc < 3 )
    {
    label_err :
    cerr << "version 20260120, utilisation : " << endl ;
    cerr << " " << argv[0] << " --d " << " directorie_hgt : delete no france" << endl ;
    cerr << " " << argv[0] << " --c " << " directorie_hgt : creation des checksums " << endl ;
    cerr << " " << argv[0] << " --v " << " directorie_hgt : verification des checksum" << endl ;
    return 1 ;
    }
else if ( !strcmp( "--d", argv[1] ) )
    g_OptLineCmd = OptDeleteNoFrance ;
else if ( !strcmp( "--c", argv[1] ) )
    g_OptLineCmd = OptCreate ;
else if ( !strcmp( "--v", argv[1] ) )
    g_OptLineCmd = OptVerify ;
else
    goto label_err ;


if ( g_OptLineCmd == OptDeleteNoFrance )
    FonctionDeleNoFrance( argv[2] ) ;
else if ( g_OptLineCmd == OptVerify )
    FonctionVerifyChks( argv[2] ) ;
else if ( g_OptLineCmd == OptCreate )
    FonctionCreateChks( argv[2] ) ;
else
    cerr << "**** probleme dans la ligne de commande" << endl ;



cout << "fin de suppression/calcul/verification fichier hgt et checksum."  << endl ;

return 0;
}
