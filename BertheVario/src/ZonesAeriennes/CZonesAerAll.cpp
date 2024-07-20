////////////////////////////////////////////////////////////////////////////////
/// \file CZonesAerAll.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 21/07/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CZonesAerAll::CZonesAerAll()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Detruit toutes les zones; libere de la place pour le file manager qui
/// bloque autrement.
void CZonesAerAll::DeleteAll()
{
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer * pZoneAer = m_ZonesArr[iz] ;
    delete pZoneAer ;
    }

m_NbZones = 0 ;
delete [] m_ZonesArr ;
m_ZonesArr = NULL ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne l'altitude des zones suivant leur periode et la date courante.
void CZonesAerAll::SetDatePeriode()
{
SetDateTU( g_GlobalVar.m_Annee, g_GlobalVar.m_Mois, g_GlobalVar.m_Jour, 0. ) ;
bool WeekEnd = IsWeekEnd() ;
float JDCur = GetJD_TU() ;

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

// pour toutes les zones
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer & ZoneAer = *m_ZonesArr[iz] ;

    // si zone sans periode
    ZoneAer.m_AltiAPrendreEnCompte = ALTI_BASSE ;
    if ( ZoneAer.m_AltiBassePeriodeSemaine ==-1 ||
         ZoneAer.m_AltiBassePeriodeWeekEnd ==-1 ||
         ZoneAer.m_PeriodeDebutJour == -1 ||
         ZoneAer.m_PeriodeFinJour   == -1 ||
         ZoneAer.m_PeriodeDebutMois == -1 ||
         ZoneAer.m_PeriodeFinMois   == -1 )
        {
        continue ;
        }

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

    // test si dans periode
    CDate DateDebut( g_GlobalVar.m_Annee , ZoneAer.m_PeriodeDebutMois , ZoneAer.m_PeriodeDebutJour , 0. ) ;
    CDate DateDefin( g_GlobalVar.m_Annee , ZoneAer.m_PeriodeFinMois , ZoneAer.m_PeriodeFinJour , 24. ) ;
    if ( (DateDebut.GetJD_TU() <= JDCur) && (JDCur <= DateDefin.GetJD_TU()) )
        {
        // positionnement alti fonction periode
        if ( WeekEnd )
            ZoneAer.m_AltiAPrendreEnCompte = ALTI_PERIODE_WEEKEND ;
        else
            ZoneAer.m_AltiAPrendreEnCompte = ALTI_PERIODE_SEMAINE ;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des 3 fichier, zones, activation et periode.
void CZonesAerAll::LectureFichiers()
{
LectureFichierZonesAer() ;
LectureFichierZonesActive() ;
LectureFichierZonesPeriode() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier zonesaer.txt
void CZonesAerAll::LectureFichierZonesAer()
{
const int TaillMaxChar = 35000 ; // taille buffer en concurence memoire avec g_Termic
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
m_File = SD.open(NOM_FCH_ZONE_AER);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(m_File.available())
    {
    char c = m_File.read() ;
    if ( c != '\n' )
        {
        // si taille chaine maximum on empile plus dans le buffer
        if ( ic < TaillMaxChar )
            TmpChar[ic++] = c ;
        }
    else
        {
        // si fin de ligne on traite le buffer
        TmpChar[ic++] = 0 ;
        TraiteBufferZoneAer( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZoneAer( TmpChar ) ;

m_File.close();

// tri de la plus petite surface/altitude vers la plus grande
TriZonesSurface() ;

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction traite une ligne texte de zone et la met sous forme binaire
/// dans le tableau m_ZonesArr.
/// Reduction de zone a 300 pts MAX_PTS_4_ZONE par CVecReduce.
/// Calcul aussi le barycentre et le rayon max de la zone pour une recheche rapide.
void CZonesAerAll::TraiteBufferZoneAer( char * buff )
{

// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' )
    {
    *buff = 0 ;
    return ;
    }

// on ne prend pas en compte les zone 1.20, 5.20, 4.20...
//if ( strstr( pChar , ".20" ) )
//    return ;

// nouvelle zone
CZoneAer * pZone = new CZoneAer ;

// allocation nouveau tableau de zones plus grand et recopie de l'ancien contenu
if ( m_ZonesArr == NULL )
    {
    m_ZonesArr = new CZoneAer * [1] ;
    m_NbZones = 1 ;
    }
else
    {
    CZoneAer ** pOldArr = m_ZonesArr ;
    m_ZonesArr = new CZoneAer * [++m_NbZones] ;
    memcpy( m_ZonesArr , pOldArr , (m_NbZones-1) * sizeof(CZoneAer *) ) ;
    delete [] pOldArr ;
    }
// ajout de la nouvelle zone au tableau
m_ZonesArr[m_NbZones-1] = pZone ;

// recopie nom de zone
pZone->m_NomOri = pChar ;
pZone->m_NomAff = pChar ;

// zone protegee on enleve "reserve naturelle nationale de la vallee de"
int iespacemax = 3 ;
char NomRND[]  = "Reserve naturelle nationale de " ;
char NomRNDV[] = "Reserve naturelle nationale de la Vallee de " ;
char NomProtect[] = "PROTECT " ;
if ( strstr( pChar , NomProtect ) )
    {
    iespacemax = 1 ;
    pChar += strlen( NomProtect ) ;
    char * pTmpChar = strstr( pChar , NomRNDV ) ;
    if ( pTmpChar != NULL )
       pChar += strlen( NomRNDV ) ;
    else
        {
        pTmpChar = strstr( pChar , NomRND ) ;
        if ( pTmpChar != NULL )
            pChar += strlen( NomRND ) ;
        }
    pZone->m_NomAff = pChar ;
    }

// formattage nom de zone 3 champs
int iespace = 0 ;
for ( int ic = 0 ; ic < pZone->m_NomAff.size() ; ic++ )
    {
    if ( pZone->m_NomAff[ic] == '(' )   // premier (
        {
        pZone->m_NomAff.resize( ic ) ;
        break ;
        }
    else if ( (pZone->m_NomAff[ic] == ' ' || pZone->m_NomAff[ic] == '-') && ++iespace >= iespacemax ) // trois champs max
        {
        pZone->m_NomAff.resize( ic ) ;
        break ;
        }
    }
pZone->m_NomOri.shrink_to_fit();
pZone->m_NomAff.shrink_to_fit();

// altitude de basse zone
pChar = strtok( NULL , ";" ) ;
pZone->m_AltiBasse = atoi( pChar ) ;

// ajout des points du polygone
pChar = strtok( NULL , ";," ) ;
std::vector<CZoneAer::st_coord_poly*> VecPoly ;
// reserve pour une grande zone
VecPoly.reserve( 2000 ) ;
while ( pChar != NULL )
    {
    // nouveau point
    CZoneAer::st_coord_poly * ppts = new CZoneAer::st_coord_poly ;

    // longitude
    ppts->m_Lon = atof( pChar ) ;
    pChar = strtok( NULL , ";," ) ;

    // latitude
    ppts->m_Lat = atof( pChar ) ;
    pChar = strtok( NULL , ";," ) ;

    // ajout au vecteur
    VecPoly.push_back( ppts ) ;

    //Serial.println( m_NbZones ) ;
    //Serial.println( esp_get_free_heap_size() ) ;
    }


// reduction du nombre de points : ceux plus proche du barycentre enlevés
// distance minimale de DIST_METRE_4_ZONE entyre les points
/*// pour gnuplot
Serial.println( "--------------------------"  ) ;
for ( long iv = 0 ; iv < VecPoly.size() ; iv ++ )
    {
    Serial.print(VecPoly[iv]->m_Lon, 5) ;
    Serial.print( " " ) ;
    Serial.println(VecPoly[iv]->m_Lat, 5) ;
    }
Serial.println( "--------------------------"   ) ; //*/

CVecReduce VecReduce ;
VecReduce.Set( VecPoly ) ;
VecReduce.ReduceTo( DIST_METRE_4_ZONE ) ;

/*// pour gnuplot
Serial.println( "********************" ) ;
for ( long iv = 0 ; iv < VecPoly.size() ; iv ++ )
    {
    Serial.print(VecPoly[iv]->m_Lon , 5 ) ;
    Serial.print( " " ) ;
    Serial.println(VecPoly[iv]->m_Lat , 5 ) ;
    }
Serial.println( "********************" ) ; //*/

// recopie du vecteur de points vers le tableau de points
pZone->m_PolygoneArr = new CZoneAer::st_coord_poly * [ VecPoly.size() ] ;
memcpy( pZone->m_PolygoneArr , & VecPoly[0] , VecPoly.size() * sizeof( CZoneAer::st_coord_poly*) ) ;
pZone->m_NbPts = VecPoly.size() ;

// calcul de la surface et du barycentre
pZone->m_Area = CPolygone::GetAreaSize( pZone->m_PolygoneArr , pZone->m_NbPts , pZone->m_Barycentre ) ;

// calcul du rayon max
pZone->m_RayonMetre = 0. ;
for ( int is = 0 ; is < pZone->m_NbPts ; is++ )
    {
    const CZoneAer::st_coord_poly & PtsCour = *pZone->m_PolygoneArr[is] ;
    float dist = sqrtf( powf(pZone->m_Barycentre.m_Lat-PtsCour.m_Lat,2) + powf(pZone->m_Barycentre.m_Lon-PtsCour.m_Lon,2) ) * 60. * UnMileEnMetres  ;
    if ( dist > pZone->m_RayonMetre )
        pZone->m_RayonMetre = dist ;
    }

// pour prochain appel, buffer vide
*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier periode de zone
void CZonesAerAll::LectureFichierZonesPeriode()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
m_File = SD.open(NOM_FCH_ZONE_PER);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(m_File.available())
    {
    char c = m_File.read() ;
    if ( c != '\n' )
        {
        // si taille chaine maximum on empile plus dans le buffer
        if ( ic < TaillMaxChar )
            TmpChar[ic++] = c ;
        }
    else
        {
        // si fin de ligne on traite le buffer
        TmpChar[ic++] = 0 ;
        TraiteBufferZonePeriode( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZonePeriode( TmpChar ) ;

m_File.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne periode de zone
void CZonesAerAll::TraiteBufferZonePeriode( char * buff )
{
// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' )
    {
    *buff = 0 ;
    return ;
    }

std::string NomOri = pChar ;
NomOri += ' ' ;     // caractere de fin de champ pour recherche sans bug
std::string NomAff = strtok( NULL ,";");
std::string PeriodeDeb = strtok( NULL ,";");
std::string PeriodeFin = strtok( NULL ,";");
std::string AltiSemaine = strtok( NULL ,";");
std::string AltiWeekEnd = strtok( NULL ,";");

CZoneAer * pZone = Find( NomOri.c_str() ) ;

// si pas de zone
if ( pZone == NULL )
    {
    Serial.println("zone aerienne non trouvée periode") ;
    *buff = 0 ;
    return ;
    }

// recopie nom pour affiche
if ( NomAff != "-" && NomAff != "" )
    pZone->m_NomAff = NomAff ;
/*Serial.print(pZone->m_NomOri.c_str()) ;
Serial.print("|") ;
Serial.println(pZone->m_NomAff.c_str()) ;*/

// periode debut
if ( PeriodeDeb != "-" && PeriodeDeb != "" )
    {
    std::string Jour = strtok( & PeriodeDeb[0] , "-" ) ;
    std::string Mois = strtok( NULL , "-" ) ;
    pZone->m_PeriodeDebutJour = atoi( Jour.c_str() ) ;
    pZone->m_PeriodeDebutMois = atoi( Mois.c_str() ) ;
    //pZone->m_PeriodeDebut = -1. ;
    }

// periode fin
if ( PeriodeFin != "-" && PeriodeFin != "" )
    {
    std::string Jour = strtok( & PeriodeFin[0] , "-" ) ;
    std::string Mois = strtok( NULL , "-" ) ;
    pZone->m_PeriodeFinJour = atoi( Jour.c_str() ) ;
    pZone->m_PeriodeFinMois = atoi( Mois.c_str() ) ;
    //pZone->m_PeriodeFin = -1 ;
    }

// alti semaine
if ( AltiSemaine != "-" && AltiSemaine != "" )
    {
    std::string Alti = strtok( & AltiSemaine[0] , "-" ) ;
    pZone->m_AltiBassePeriodeSemaine = atoi( Alti.c_str() ) ;
    }

// alti week-end
if ( AltiWeekEnd != "-" && AltiWeekEnd != "" )
    {
    std::string Alti = strtok( & AltiWeekEnd[0] , "-" ) ;
    pZone->m_AltiBassePeriodeWeekEnd = atoi( Alti.c_str() ) ;
    }

// pour prochain appel
*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture fichier activation de zone
void CZonesAerAll::EcritureFichierZonesActive()
{

File Fch ;

// ouverture fichier
Fch = SD.open(NOM_FCH_ZONE_ACT, FILE_WRITE , true );
if (!Fch)
    {
    Serial.print( "erreur creation fichier zone actice" ) ;
    return ;
    }

// debut de fichier
Fch.seek(0) ;

std::vector<std::string> VecNomDejaEcrit ;

// ecriture des zone
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    // zone en memoire
    const CZoneAer * pZone = m_ZonesArr[iz] ;
    if ( !pZone->m_DansFchActivation )
        continue ;

    // verif si pas deja fait
    bool DejaFait = false ;
    for ( long iv = 0 ; iv < VecNomDejaEcrit.size() ; iv++ )
        {
        if ( VecNomDejaEcrit[iv] == pZone->m_NomAff )
            {
            DejaFait = true ;
            break ;
            }
        }


    // si deja ecrit dans le fichier
    if ( DejaFait )
        continue ;

    // empilement pour suivit deja fait
    VecNomDejaEcrit.push_back( pZone->m_NomAff ) ;

    // ecriture fichier
    Fch.print( pZone->m_NomAff.c_str() ) ;
    Fch.print( ";" ) ;
    if ( pZone->m_Activee )
        Fch.print( "1" ) ;
    else
        Fch.print( "0" ) ;
    Fch.println( ";" ) ;
    }

Fch.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier activation de zone
void CZonesAerAll::LectureFichierZonesActive()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
m_File = SD.open(NOM_FCH_ZONE_ACT);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(m_File.available())
    {
    char c = m_File.read() ;
    if ( c != '\n' )
        {
        // si taille chaine maximum on empile plus dans le buffer
        if ( ic < TaillMaxChar )
            TmpChar[ic++] = c ;
        }
    else
        {
        // si fin de ligne on traite le buffer
        TmpChar[ic++] = 0 ;
        TraiteBufferZoneActive( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZoneActive( TmpChar ) ;

m_File.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne activation zone.
void CZonesAerAll::TraiteBufferZoneActive( char * buff )
{
// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' )
    return ;

// nom de zone
std::string NomOri = pChar ;

// activation de zone
char * pActivation = strtok( NULL ,";") ;
if ( pActivation == NULL )
    return ;
std::string Activation = pActivation ;

// pour toutes les zones
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;
    if ( strstr( Zone.m_NomOri.c_str() , NomOri.c_str() ) )
        {
        CZoneAer * pZone = m_ZonesArr[iz] ;

        // activation
        if ( Activation == "0" )
            pZone->m_Activee = false ;
        else
            pZone->m_Activee = true ;

        pZone->m_DansFchActivation = true ;
        }
    }

*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cherche la zone dans le tableau par rapport a son nom d'origine 3 champs.
CZoneAer * CZonesAerAll::Find( const char * NomOri )
{
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;
    if ( strstr( Zone.m_NomOri.c_str() , NomOri ) )
        return m_ZonesArr[iz] ;
    }

return NULL ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri croissant de nom pour ecran gestion zone
void CZonesAerAll::TriZonesNom()
{
CSortArray::m_pArr = m_ZonesArr ;
CSortArray::m_Size = m_NbZones ;
CZoneAer::TriParNom( true ) ;
TriSchellMetzner() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri surface croissant pour determination
void CZonesAerAll::TriZonesSurface()
{
CSortArray::m_pArr = m_ZonesArr ;
CSortArray::m_Size = m_NbZones ;
CZoneAer::TriParNom( false ) ;
TriSchellMetzner(true) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul si l'on est dans une zone avec le nom de la zone et son altitude
/// \param m_NomZone [out] : nom de la zone en presence ou proche
/// \param m_DansDessousUneZone [out] : indique si on est dans ou au dessous d'une zone
/// \param m_NomZoneDansDessous [out] :  nom de la zone de laquelle on est dedans ou endessous
/// \param m_LimiteZone [out] : si en limite de zone
/// \param m_NomZoneEnLimite [out] : nom de la zone en limite
void CZonesAerAll::CalcZone()
{
m_Plafond4Valid = 100000 ;
std::string RetStrIn = "" ;
int         RetNbrIn = ZONE_EN_DEHORS ;
std::string RetStrLimite = "" ;
int         RetNbrLimite = ZONE_EN_DEHORS ;

// test dans la zone, position courante
CZoneAer::st_coord_poly PtsEnCours ;
PtsEnCours.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
PtsEnCours.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

#ifdef TMA_DEBUG
SetDate( 2024 , 3, 31 ) ;
//SetDate( 2024 , 2, 25 ) ;
g_GlobalVar.m_TerrainPosCur.m_AltiBaro = 1500 ;
g_GlobalVar.m_Config.m_XYMargin = 3000. ;

//PtsEnCours.m_Lat = 45.773785;   // pdd
//PtsEnCours.m_Lon = 2.96898;   // pdd
//PtsEnCours.m_Lat = 45.537966;   // saurier
//PtsEnCours.m_Lon = 3.04627;   // saurier
PtsEnCours.m_Lat = 45.331671;   // anzat-le-luguet
PtsEnCours.m_Lon = 3.041811;   // anzat-le-luguet

//PtsEnCours.m_Lat = 45.52730;   // sancy zone protege
//PtsEnCours.m_Lon = 2.81738;   // sancy zone protege

#endif

// tri des zones par surface croissante, rapide si deja trié
TriZonesSurface() ;

// si test wifi positionnement altitude sol
if ( g_GlobalVar.m_ModeHttp )
    {
    g_GlobalVar.m_AltiSolHgt = g_GlobalVar.m_Hgt2Agl.GetGroundZ( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat ) ;
    //Serial.print( "g_GlobalVar.m_AltiSolHgt:" ) ;
    //Serial.println( g_GlobalVar.m_AltiSolHgt ) ;
    }

// pour toutes les zones recherche si dessous
const CZoneAer * pZonePrecedente = NULL ;
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !Zone.m_Activee )
        continue ;

    // dans le du rayon
    float DistBaryMetres = sqrtf( powf(Zone.m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(Zone.m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayon = DistBaryMetres < Zone.m_RayonMetre ;

    // dedans la surface
    bool IsInArea = DansLeRayon && CPolygone::IsIn( Zone.m_PolygoneArr , Zone.m_NbPts , PtsEnCours ) ;

    // positionnement retour
    if ( IsInArea )
        {
        int  PlafondZone = Zone.GetAltiBasse() ;
        bool Corent = ( PlafondZone == 822 ) ;
        bool ZoneProtegee = strstr( Zone.m_NomOri.c_str() , "PROTECT" ) != NULL ;

        char TmpChar[50] ;
        ////////////
        // si corent
        if ( Corent )
            {
            m_Plafond4Valid = PlafondZone ;
            // si
            if ( ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) < m_Plafond4Valid) ||
                 ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) < (g_GlobalVar.m_AltiSolHgt+300) ) )
                {
                sprintf( TmpChar , "Be %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
                RetNbrIn = ZONE_DESSOUS ;
                RetStrIn = TmpChar ;
                break ;
                }
            else if ( ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro) < m_Plafond4Valid) ||
                      ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro) < (g_GlobalVar.m_AltiSolHgt+300) ) )
                {
                sprintf( TmpChar , "Al %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
                RetNbrIn = ZONE_LIMITE_ALTI ;
                RetStrIn = TmpChar ;
                break ;
                }
            else
                {
                sprintf( TmpChar , "In %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
                RetNbrIn = ZONE_DEDANS ;
                RetStrIn = TmpChar ;
                break ;
                }
            continue ;
            }

        //////////////////
        // si zone protege
        if ( ZoneProtegee )
            {
            int PlafondZoneProtegee = g_GlobalVar.m_AltiSolHgt+m_PlafondZoneProtegee ;
            // dans la zone protegee
            if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < PlafondZoneProtegee )
                {
                m_Plafond4Valid = m_PlafondZoneProtegee ;
                RetNbrIn = ZONE_DEDANS ;
                sprintf( TmpChar , "Zp %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
                RetStrIn = TmpChar ;
                break ;
                }
            // limite alti zone protege
            else if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < (PlafondZoneProtegee + g_GlobalVar.m_Config.m_AltiMargin) )
                {
                m_Plafond4Valid = m_PlafondZoneProtegee ;
                RetNbrIn = ZONE_LIMITE_ALTI ;
                sprintf( TmpChar , "Al %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
                RetStrIn = TmpChar ;
                }
            continue ;
            }

        ////////////////////////////
        // si dans la zone du dessus
        if ( PlafondZone < g_GlobalVar.m_TerrainPosCur.m_AltiBaro )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "In %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            break ;
            }

        /////////////////////////////////////////
        // si a la limite altitude par le dessous
        if ( (PlafondZone-g_GlobalVar.m_Config.m_AltiMargin) < g_GlobalVar.m_TerrainPosCur.m_AltiBaro )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "Al %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_LIMITE_ALTI ;
            RetStrIn = TmpChar ;
            break ;
            }

        /////////////////////////////////////////
        // si en dessous la zone du dessus
        if ( PlafondZone < m_Plafond4Valid )
            {
            // si zone precedente au dessus et derogation ffvl/date
            if ( pZonePrecedente != NULL && pZonePrecedente->m_PeriodeDebutMois != -1 )
                continue ;
            pZonePrecedente = &Zone ;
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "Be %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }
        }
    }

// pour toutes les zones activables recherche dedans si pas deja dedans celle du dessus
for ( long iz = 0 ; iz < m_NbZones && RetNbrIn != ZONE_DEDANS ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;

    // si la zone n'est pas active ou pas activable
    if ( !Zone.m_Activee || ! Zone.m_DansFchActivation )
        continue ;

    // dans le du rayon
    float DistBaryMetres = sqrtf( powf(Zone.m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(Zone.m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayon = DistBaryMetres < Zone.m_RayonMetre ;
    bool ZoneProtegee = strstr( Zone.m_NomOri.c_str() , "PROTECT" ) != NULL ;

    // dedans la surface
    bool IsInArea = DansLeRayon && CPolygone::IsIn( Zone.m_PolygoneArr , Zone.m_NbPts , PtsEnCours ) ;
    // dedans le volume
    bool IsInZone = IsInArea && (g_GlobalVar.m_TerrainPosCur.m_AltiBaro >= Zone.GetAltiBasse() ) ;

    // si dans zone
    if ( IsInZone  )
        {
        if ( ZoneProtegee )
            {
            }
        else
            {
            RetNbrIn = ZONE_DEDANS ;
            // construction nom + altitude
            char TmpChar[50] ;
            m_Plafond4Valid=Zone.GetAltiBasse() ;
            sprintf( TmpChar , "In %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
            RetStrIn = TmpChar ;
            }
        break ;
        }
    }

m_Mutex.PrendreMutex() ;
 m_DansDessousUneZone = RetNbrIn ;
 m_NomZoneDansDessous = RetStrIn ;
m_Mutex.RelacherMutex() ;

// pour toutes les zones recherche frontiere XY
for ( long iz = 0 ; iz < m_NbZones && RetNbrIn != ZONE_LIMITE_ALTI ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !Zone.m_Activee )
        continue ;

    // dans le du rayon + marge
    float DistBaryMetres = sqrtf( powf(Zone.m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(Zone.m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayon = DistBaryMetres < Zone.m_RayonMetre ;

    // dedans la surface
    bool IsInArea = DansLeRayon && CPolygone::IsIn( Zone.m_PolygoneArr , Zone.m_NbPts , PtsEnCours ) ;
    // proche de la frontiere de zone
    bool IsNearFront = !IsInArea && CDistFront::IsNearFront( Zone.m_PolygoneArr , Zone.m_NbPts , PtsEnCours ) &&
                       ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro-g_GlobalVar.m_Config.m_AltiMargin) >= Zone.GetAltiBasse() ) ;

    // positionnement retour
    if ( IsNearFront )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom + altitude
        char TmpChar[50] ;
        m_Plafond4Valid=Zone.GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , Zone.m_NomAff.c_str() , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        break ;
        }
    }

// variables de sortie fontiere
m_Mutex.PrendreMutex() ;
 m_LimiteZone      = RetNbrLimite ;
 m_NomZoneEnLimite = RetStrLimite ;
m_Mutex.RelacherMutex() ;
}
