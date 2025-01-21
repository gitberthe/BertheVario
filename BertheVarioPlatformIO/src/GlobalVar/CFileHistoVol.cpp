////////////////////////////////////////////////////////////////////////////////
/// \file CFilesHistoVols.cpp
///
/// \brief
///
/// \date creation     : 22/03/2024
/// \date modification : 21/01/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier histo au debut du programme
void CFilesHistoVols::LectureFichiers()
{
char * TmpChar = new char [5000] ;

// dextruction dernier histo
m_HistoDir.clear() ;

// ouverture directorie histo
File HistoDir = SD.open(HISTO_DIR) ;
if ( !HistoDir )
    {
    Serial.println("Failed to open histo dir");
    delete [] TmpChar ;
    return;
    }

// pour toute la directorie
while( true )
    {
    // ouverture fichier
    m_File = HistoDir.openNextFile() ;

    // si fin de liste
    if ( ! m_File )
        break ;

    // si directorie
    if ( m_File.isDirectory() )
        continue ;

    // lecture fichier
    int ic = 0 ;
    while(m_File.available())
        TmpChar[ic++] = m_File.read();
    TmpChar[ic++] = 0 ;

    // fermeture fichier
    m_File.close() ;

    // decoupage en ligne
    std::vector<char*> VecLigne ;
    char * pChar = strtok( TmpChar , "\n" ) ;
    while ( pChar != NULL )
        {
        // recopie
        char * pLigne = new char[100] ;
        strcpy( pLigne , pChar ) ;
        VecLigne.push_back( pLigne ) ;
        pChar = strtok( NULL , "\n" ) ;
        }

    // analyse des champs
    CHistoVol HistoVol ;
    char Separ[] = " #\t" ;
    for ( int i = 0 ; i < VecLigne.size() ; i++ )
        {
        char * pLigne = VecLigne[i] ;
        char * pNomParam = strtok( pLigne , Separ ) ;
        char * pValParam = strtok( NULL , Separ ) ;
        if ( pNomParam == NULL || pValParam == NULL )
            continue ;
        if ( !strcmp( pNomParam , "[IgcNom]" ) )
            strcpy( HistoVol.m_NomIgc , pValParam ) ;
        else if ( !strcmp( pNomParam , "[VzMax]" ) )
            HistoVol.m_VzMax = atof(pValParam) ;
        else if ( !strcmp( pNomParam , "[VzMin]" ) )
            HistoVol.m_VzMin = atof(pValParam) ;
        else if ( !strcmp( pNomParam , "[VsMax]" ) )
            HistoVol.m_VsMax = atof(pValParam) ;
        else if ( !strcmp( pNomParam , "[temps_vol]" ) )
            HistoVol.m_TempsDeVol = atof(pValParam) ;
        else if ( !strcmp( pNomParam , "[IgcAnnee]" ) )
            HistoVol.m_AnneeDuVol = atoi(pValParam) ;
        else if ( !strcmp( pNomParam , "[distance]" ) )
            HistoVol.m_DistanceKm = atof(pValParam) ;
        else if ( !strcmp( pNomParam , "[z_deco]" ) )
            HistoVol.m_ZDeco = atoi(pValParam) ;
        else if ( !strcmp( pNomParam , "[z_max]" ) )
            HistoVol.m_ZMax = atoi(pValParam) ;
        }

    // ajout du fichier histo
    m_HistoDir.push_back( HistoVol ) ;

    // liberation memoire
    for ( int i = 0 ; i < VecLigne.size() ; i++ )
        delete [] VecLigne[i] ;
    }

HistoDir.close() ;

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture toutes les 5s du fichier d'historique pour igc cournt.
void CFilesHistoVols::EcritureFichier( const char * NomFichierIgc )
{
// creation directorie histo
SD.mkdir(HISTO_DIR);

// creation fichier nom fichier histo
char TmpChar[30] ;
char HistoFileName[30] = HISTO_DIR ;
strcpy( TmpChar , NomFichierIgc ) ;
strtok( TmpChar , "." ) ;
strcat( TmpChar , ".his" ) ;
strcat( HistoFileName , TmpChar ) ;

// ouverture fichier
m_File = SD.open(HistoFileName, FILE_WRITE , true );
if (!m_File)
    {
    #ifdef _LG_DEBUG_
     Serial.print( "erreur creation fichier NOM_FCH_HISTO_VOL" ) ;
    #endif
    return ;
    }

// debut de fichier
m_File.seek(0) ;

// ecritude fichier
m_File.print("[IgcNom] ") ;   m_File.println( NomFichierIgc ) ;
m_File.print("[IgcAnnee] ") ; m_File.println( g_GlobalVar.m_Annee ) ;
m_File.print("[VzMax] ") ;    m_File.println( m_VzMax ) ;
m_File.print("[VzMin] ") ;    m_File.println( m_VzMin ) ;
m_File.print("[VsMax] ") ;    m_File.println( m_VsMax ) ;
m_File.print("[temps_vol] "); m_File.println( m_TempsDeVol , 1 ) ;
m_File.print("[distance] ") ; m_File.println( m_DistanceKm ) ;
m_File.print("[z_deco] ") ;   m_File.println( m_ZDeco ) ;
m_File.print("[z_max] ") ;    m_File.println( m_ZMax ) ;
m_File.println(" ") ;

// fermeture fichier
m_File.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul les VzMax, VzMin, et renvoi la distance...
float CFilesHistoVols::PushHistoVol()
{
// temps de vol
m_TempsDeVol = g_GlobalVar.m_DureeVolMin ;

// altitude max
float AltiPression = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
if ( m_ZMax < AltiPression )
    m_ZMax = AltiPression ;

// Vz Max
float Vz = g_GlobalVar.m_VitVertMS ;
if ( m_VzMax < Vz )
    m_VzMax = Vz ;

// Vz Min
if ( m_VzMin > Vz )
    m_VzMin = Vz ;

// Vs Max
float Vs = g_GlobalVar.m_VitesseKmh ;
if ( m_VsMax < Vs )
    m_VsMax = Vs ;

// distance
float DistanceMetresLat = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lat - m_LastLat ) * 60. * UnMileEnMetres ;
float DistanceMetresLon = fabsf( g_GlobalVar.m_TerrainPosCur.m_Lon - m_LastLon ) * 60. * UnMileEnMetres ;
float DistanceDeplacementMetres = sqrtf( powf(DistanceMetresLat,2) + powf(DistanceMetresLon,2) ) ;
m_DistanceKm += DistanceDeplacementMetres / 1000.  ;
m_LastLat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
m_LastLon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

// pour le calcul de la finesse
return DistanceDeplacementMetres ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Detruit tous les fichiers histo
void CFilesHistoVols::DeleteHisto()
{
m_HistoDir.clear() ;

// ouverture directorie histo
File HistoDir = SD.open(HISTO_DIR) ;
if ( !HistoDir )
    {
    Serial.println("Failed to open histo dir");
    return;
    }

// pour toute la directorie
while( true )
    {
    // ouverture fichier
    m_File = HistoDir.openNextFile() ;

    if ( ! m_File )
        break ;

    char TmpChar[50] = HISTO_DIR ;
    strcat( TmpChar , "/" ) ;
    strcat( TmpChar , m_File.name() ) ;

    SD.remove( TmpChar ) ;

    m_File.close() ;
    }

HistoDir.close() ;
}

