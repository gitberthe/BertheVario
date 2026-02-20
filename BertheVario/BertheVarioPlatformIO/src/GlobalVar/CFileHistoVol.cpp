////////////////////////////////////////////////////////////////////////////////
/// \file CFilesHistoVols.cpp
///
/// \brief
///
/// \date creation   : 22/03/2024
/// \date 17/10/2025 : ajout ZMin.
/// \date 21/10/2025 : nombre de fix altibaro, debut de vol
/// \date 22/10/2025 : Fichier HistoVol toutes les 60s
/// \date 24/10/2025 : OnceDir et nom fichier histo
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier histo au debut du programme
void CFilesHistoVols::LectureFichiers()
{
const int SiseMax = 500 ;
char * TmpChar = new char [SiseMax+1] ;

// dextruction dernier histo
m_HistoDir.SetSize(0) ;

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
    while(m_File.available() && ic < SiseMax )
        TmpChar[ic++] = m_File.read();
    TmpChar[ic++] = 0 ;

    // fermeture fichier
    m_File.close() ;

    // decoupage en ligne
    CLGArrayPtr<char> VecLigne ;
    VecLigne.SetDeleteObjet(true) ;
    char * pChar = strtok( TmpChar , "\n" ) ;
    int SizeLine = (pChar!=NULL) ? strlen(pChar)+1 : 1 ;
    while ( pChar != NULL && VecLigne.GetSize() < 20 )
        {
        // recopie
        char * pLigne = new char[SizeLine] ;
        strcpy( pLigne , pChar ) ;
        VecLigne.Add( pLigne ) ;
        pChar = strtok( NULL , "\n" ) ;
        SizeLine = (pChar!=NULL) ? strlen(pChar)+1 : 1 ;
        }

    // analyse des champs
    CHistoVol HistoVol ;
    char Separ[] = " #\t" ;
    for ( int i = 0 ; i < VecLigne.GetSize() ; i++ )
        {
        char * pLigne = VecLigne[i] ;
        if ( pLigne == NULL )
            continue ;
        char * pNomParam = strtok( pLigne , Separ ) ;
        if ( pNomParam == NULL )
            continue ;
        char * pValParam = strtok( NULL , Separ ) ;
        if ( pValParam == NULL )
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
        else if ( !strcmp( pNomParam , "[z_min]" ) )
            HistoVol.m_ZMin = atoi(pValParam) ;
        else if ( !strcmp( pNomParam , "[deb_v]" ) )
            HistoVol.m_DebutDeVol = atoi(pValParam) ;
        }

    // ajout du fichier histo
    m_HistoDir.Add( HistoVol ) ;

    // liberation memoire
    VecLigne.DeleteAll() ;
    }

HistoDir.close() ;

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionnement du nom de fichier courant
void CFilesHistoVols::SetNomFichierHisto(const char* NomFchIgc )
{
char TmpChar[30] ;
char HistoFileName[30] = HISTO_DIR ;
strcpy( TmpChar , NomFchIgc ) ;
strtok( TmpChar , "." ) ;
strcat( TmpChar , ".his" ) ;
strcat( HistoFileName , TmpChar ) ;

m_NomFchIgc   = NomFchIgc ;
m_NomFchHisto = HistoFileName ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture toutes les 60s du fichier d'historique pour igc courant.
/// Temps d'ecriture de 60millis
void CFilesHistoVols::EcritureFichier()
{
// creation directorie histo
static bool OnceDir = false ;
if ( !OnceDir )
    {
    SD.mkdir(HISTO_DIR);
    OnceDir = true ;
    }

sprintf( m_BuffWrite , "[IgcNom]    %s\n\
[IgcAnnee]  %d\n\
[VzMax]     %.1f\n\
[VzMin]     %.1f\n\
[VsMax]     %.1f\n\
[temps_vol] %.1f\n\
[distance]  %.1f\n\
[z_deco]    %d\n\
[z_max]     %d\n\
[z_min]     %d\n\
[deb_v]     %d\n",
(const char*)m_NomFchIgc,
g_GlobalVar.m_Annee,
m_VzMax,
m_VzMin,
m_VsMax,
m_TempsDeVol,
m_DistanceKm,
m_ZDeco,
m_ZMax,
m_ZMin,
m_DebutDeVol
) ;

// ouverture fichier
m_File = SD.open((const char*)m_NomFchHisto, FILE_WRITE , true );
if (!m_File)
    {
    #ifdef _LG_DEBUG_
     Serial.print( "erreur creation fichier NOM_FCH_HISTO_VOL" ) ;
    #endif
    return ;
    }

// debut de fichier
m_File.seek(0) ;

m_File.write( (const uint8_t*) m_BuffWrite , strlen(m_BuffWrite)  ) ;

//Serial.println( strlen(m_BuffWrite) ) ;
/*// ecriture fichier
m_File.printf("[IgcNom]    %s\n",    (const char*)m_NomFchIgc ) ;
m_File.printf("[IgcAnnee]  %d\n",    g_GlobalVar.m_Annee ) ;
m_File.printf("[VzMax]     %.1f\n",  m_VzMax ) ;
m_File.printf("[VzMin]     %.1f\n",  m_VzMin ) ;
m_File.printf("[VsMax]     %.1f\n",  m_VsMax ) ;
m_File.printf("[temps_vol] %.1f\n",  m_TempsDeVol ) ;
m_File.printf("[distance]  %.1f\n",  m_DistanceKm ) ;
m_File.printf("[z_deco]    %d\n",    m_ZDeco ) ;
m_File.printf("[z_max]     %d\n",    m_ZMax ) ;
m_File.printf("[z_min]     %d\n",    m_ZMin ) ;
m_File.printf("[deb_v]     %d\n",    m_DebutDeVol ) ;*/

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
float AltiPression = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;
if ( m_ZMax < AltiPression )
    m_ZMax = AltiPression ;

// altitude min
if ( m_ZMin > AltiPression )
    m_ZMin = AltiPression ;

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
float DistanceMetresLat = ( g_GlobalVar.m_TerrainPosCur.m_Lat - m_LastLat ) * 60. * UnMileEnMetres ;
float DistanceMetresLon = ( g_GlobalVar.m_TerrainPosCur.m_Lon - m_LastLon ) * 60. * UnMileEnMetres ;
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
m_HistoDir.SetSize(0) ;
m_HistoDir.FreeExtra() ;

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

