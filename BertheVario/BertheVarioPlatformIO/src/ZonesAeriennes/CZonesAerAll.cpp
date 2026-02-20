////////////////////////////////////////////////////////////////////////////////
/// \file CZonesAerAll.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date 08/07/2025 : suppression xc_track et limite zone xy alti
/// \date 26/10/2025 : tableau de st_coord_poly et plus *.
/// \date 27/10/2025 : Remplacement de m_RayonMetre par CRectIn.
/// \date 06/01/2026 : modification pour free zone periodique
/// \date 07/01/2026 : modification pour zone active non compresse
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
/// \brief
CZonesAerAll::CZonesAerAll()
{
SetDeleteObjet(true) ;
m_VecZoneInArea.SetDeleteObjet(false) ;
m_VecZonesRet.SetDeleteObjet(false) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Detruit toutes les zones; libere de la place pour le file manager qui
/// bloque autrement.
void CZonesAerAll::DeleteAll()
{
/*for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    CZoneAer * pZoneAer = (*this)[iz] ;
    delete pZoneAer ;
    }

GetSize() = 0 ;
delete [] (*this) ;
(*this) = NULL ;*/
CLGArrayPtr<CZoneAer>::DeleteAll() ;

m_VecZoneInArea.DeleteAll() ;
m_VecZoneInArea.FreeExtra() ;
m_VecZonesRet.DeleteAll() ;
m_VecZonesRet.FreeExtra() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi si l'on compresse les zone par rapport a la memoire
bool CZonesAerAll::IsEnoughMemory()
{
int FreeMem = esp_get_free_heap_size() ;
if ( FreeMem > NB_KILO_FREE_MEM * 1024 )
    return true ;
return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne l'altitude des zones suivant leur periode et la date courante.
/// Positionne aussi le type de zone genrique, corent, saint-yan
void CZonesAerAll::SetDatePeriode()
{
// si date pas encore acquise
if ( g_GlobalVar.m_Annee < 2010 )
    return ;

SetDateTU( g_GlobalVar.m_Annee, g_GlobalVar.m_Mois, g_GlobalVar.m_Jour, 0. ) ;
m_IsWeekEnd = IsWeekEnd() ;
float JDCur = GetJD_TU() ;

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

// pour toutes les zones
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    CZoneAer & ZoneAer = *(*this)[iz] ;

    // type de zone
    ZoneAer.SetTypeZone() ;

    // si zone sans periode
    if ( ZoneAer.m_pDerogFfvl == NULL )
        continue ;

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

    // test si dans periode
    ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_BASSE ;
    CDate DateDebut( g_GlobalVar.m_Annee , ZoneAer.m_pDerogFfvl->m_PeriodeDebutMois , ZoneAer.m_pDerogFfvl->m_PeriodeDebutJour , 0. ) ;
    CDate DateDefin( g_GlobalVar.m_Annee , ZoneAer.m_pDerogFfvl->m_PeriodeFinMois ,   ZoneAer.m_pDerogFfvl->m_PeriodeFinJour , 24. ) ;
    if ( (DateDebut.GetJD_TU() <= JDCur) && (JDCur <= DateDefin.GetJD_TU()) )
        {
        // positionnement alti fonction periode
        if ( m_IsWeekEnd )
            ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_PERIODE_WEEKEND ;
        else
            ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_PERIODE_SEMAINE ;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des 3 fichier, zones, activation et periode.
void CZonesAerAll::LectureFichiers()
{
// ouverture fichier out nom zone
m_FileNomZoneOut = SD.open(NOM_FCH_OUT_NOM_ZONE, FILE_WRITE , true );
if (!m_FileNomZoneOut)
    Serial.print( "erreur creation fichier NOM_FCH_OUT_NOM_ZONE" ) ;
m_FileNomZoneOut.seek(0) ;

// traitement des zones
LectureFichierZonesAer() ;

// fermeture fichier nom zone
m_FileNomZoneOut.close() ;

LectureFichierZonesActive() ;
LectureFichierZonesPeriode() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier zonesaer.txt
void CZonesAerAll::LectureFichierZonesAer()
{
const int TaillMaxChar = 20000 ; // taille buffer en concurence memoire avec g_Termic
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
File FileZone = SD.open(NOM_FCH_ZONE_AER);
if ( !FileZone )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(FileZone.available())
    {
    char c = FileZone.read() ;
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

        // traitement du buffer si memoire 30k free mem
        if ( esp_get_free_heap_size() > 30*1024 )
            {
            CZoneAer * pZone = TraiteBufferZoneAer( TmpChar ) ;
            if ( pZone != NULL )
                pZone->CompressZone() ;
            }
        else
            g_GlobalVar.BeepError() ;

        ic = 0 ;
        }
    }

// pour la derniere zone
CZoneAer * pZone = TraiteBufferZoneAer( TmpChar ) ;
if ( pZone != NULL )
    pZone->CompressZone() ;

FileZone.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction traite une ligne texte de zone et la met sous forme binaire
/// dans le tableau (*this).
/// Reduction de zone par CVecReduce.
/// Calcul aussi le barycentre et le rayon max de la zone pour une recheche rapide.
CZoneAer * CZonesAerAll::TraiteBufferZoneAer( char * buff )
{

// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' )
    {
    *buff = 0 ;
    return NULL ;
    }

// on ne prend pas en compte les zone 1.20, 5.20, 4.20...
//if ( strstr( pChar , ".20" ) )
//    return ;

// nouvelle zone
CZoneAer * pZone = new CZoneAer ;

// allocation nouveau tableau de zones plus grand et recopie de l'ancien contenu
/*if ( (*this) == NULL )
    {
    (*this) = new CZoneAer * [1] ;
    m_NbZones = 1 ;
    }
else
    {
    CZoneAer ** pOldArr = (*this) ;
    (*this) = new CZoneAer * [++m_NbZones] ;
    memcpy( (*this) , pOldArr , (m_NbZones-1) * sizeof(CZoneAer *) ) ;
    delete [] pOldArr ;
    }
// ajout de la nouvelle zone au tableau
(*this)[m_NbZones-1] = pZone ; */
Add( pZone ) ;

// zone protege PROTECT ou FFVL-Prot dans chaine
bool IsProtect = ( (*pChar == 'P') && (strstr( pChar , "PROTECT" ) != NULL)) ||
                 ( (*pChar == 'F') && (strstr( pChar , "FFVL-Prot" ) != NULL)) ;
if ( IsProtect )
    {
    // si m/sol trouve
    char * pCharFin = strstr( pChar , "m/sol" ) ;
    if ( pCharFin != NULL )
        {
        char TmpBuff[500] ;
        strcpy( TmpBuff , pChar ) ;
        *pCharFin = 0 ;
        // on recule jusqu'a l'espace
        while( *pCharFin != ' ' && *pCharFin != '(' && pCharFin != pChar )
            pCharFin-- ;
        // on ravance d'un coup
        pCharFin++ ;
        // hauteur sol
        pZone->SetHauteurSolZoneProtege( atoi( pCharFin ) ) ;
        // si probleme de conversion hauteur max
        if ( pZone->GetHauteurSolZoneProtege() == 0 )
            pZone->SetHauteurSolZoneProtege( 1000 ) ;
        }
    // 1000 metres par defaut
    else
        pZone->SetHauteurSolZoneProtege( 1000 ) ;
    }

// pour controle nom zone
CLGString NomOri = pChar ;

// zone protegee on enleve des chaines
int iespacemax = 3 ;
char NomRND[]  = "Reserve naturelle nationale de " ;
char NomRNDV[] = "Reserve naturelle nationale de la Vallee de " ;
char NomProtect[] = "PROTECT " ;
char NomFFVL[] = "FFVL-Prot " ;
// zone "FFVL"
if ( (pChar[0] == 'F') && strstr( pChar , NomFFVL ) )
    pChar += strlen( NomFFVL ) ;
// zone "PROTECT"
if ( (pChar[0] == 'P') && strstr( pChar , NomProtect ) )
    pChar += strlen( NomProtect ) ;
// reserve naturelle nationale de la vallee
char * pTmpChar = strstr( pChar , NomRNDV ) ;
if ( pTmpChar != NULL )
    {
    pChar += strlen( NomRNDV ) ;
    iespacemax = 1 ;
    }
// reserve naturelle nationale de
pTmpChar = strstr( pChar , NomRND ) ;
if ( pTmpChar != NULL )
    {
    pChar += strlen( NomRND ) ;
    iespacemax = 1 ;
    }
CLGString NomAff = pChar ;

// formattage nom de zone 3 champs
int iespace = 0 ;
for ( int ic = 0 ; ic < NomAff.GetLength() ; ic++ )
    {
    // premier (
    if ( NomAff[ic] == '(' )
        {
        NomAff.SetAt( ic , 0 ) ;
        break ;
        }
    // trois champs max
    else if ( (NomAff[ic] == ' ' || NomAff[ic] == '-') && ++iespace >= iespacemax )
        {
        NomAff.SetAt( ic , 0 ) ;
        break ;
        }
    }
// on eneleve le dernier blanc
if ( NomAff[NomAff.GetLength()-1] == ' ' )
    NomAff.SetAt(NomAff.GetLength()-1,0) ;

pZone->m_pNomAff = new char [ NomAff.GetLength() + 1 ] ;
strcpy( pZone->m_pNomAff , NomAff ) ;

// affichage nom zone pour controle
m_FileNomZoneOut.print( NomAff ) ;
m_FileNomZoneOut.print( " <= " ) ;
m_FileNomZoneOut.println( NomOri ) ;

// altitude de basse zone
pChar = strtok( NULL , ";" ) ;
pZone->m_AltiBasse = atoi( pChar ) ;

// ajout des points du polygone
pChar = strtok( NULL , ";," ) ;
CLGArrayPtr<CZoneAer::st_coord_poly> VecPoly ;
VecPoly.SetDeleteObjet(true) ;
// reserve pour une grande zone
VecPoly.SetGrow( 350 ) ;
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
    VecPoly.Add( ppts ) ;

    //Serial.println( GetSize() ) ;
    //Serial.println( esp_get_free_heap_size() ) ;
    }

// recopie du vecteur de points vers le tableau de points
CZoneAer::st_coord_poly * PolygoneArr = new CZoneAer::st_coord_poly [ VecPoly.GetSize() ] ;
for ( int iz = 0 ; iz < VecPoly.GetSize() ; iz++ )
    PolygoneArr[iz] = * VecPoly[iz] ;
//memcpy( pZone->m_PolygoneArr , & VecPoly[0] , VecPoly.GetSize() * sizeof( CZoneAer::st_coord_poly*) ) ;
pZone->SetPolygoneArr( PolygoneArr , VecPoly.GetSize() ) ;

// destruction  des points
VecPoly.DeleteAll() ;

// calcul de la surface et du barycentre
//pZone->m_Area = CPolygone::GetAreaSize( pZone->m_PolygoneArr , pZone->m_NbPts , pZone->m_Barycentre ) ;
//CPolygone::CalcBarycentre( pZone->m_PolygoneArr , pZone->m_NbPts , pZone->m_Barycentre ) ;

/*
// calcul du rayon max
pZone->m_RayonMetre = 0. ;
for ( int is = 0 ; is < pZone->m_NbPts ; is++ )
    {
    const CZoneAer::st_coord_poly & PtsCour = pZone->m_PolygoneArr[is] ;
    float dist = sqrtf( powf(pZone->m_Barycentre.m_Lat-PtsCour.m_Lat,2) + powf(pZone->m_Barycentre.m_Lon-PtsCour.m_Lon,2) ) * 60. * UnMileEnMetres  ;
    if ( dist > pZone->m_RayonMetre )
        pZone->m_RayonMetre = dist ;
    }
*/

// pour prochain appel, buffer vide
*buff = 0 ;

return pZone ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier periode de zone
void CZonesAerAll::LectureFichierZonesPeriode()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
File FilePeriode = SD.open(NOM_FCH_ZONE_PER);
if ( !FilePeriode )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(FilePeriode.available())
    {
    char c = FilePeriode.read() ;
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

FilePeriode.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne periode de zone
void CZonesAerAll::TraiteBufferZonePeriode( char * buff )
{
// comptage des champs pour le plantage strtok
int countpv = 0 ;
int len = strlen( buff ) ;
for ( int ic = 0 ; buff != NULL && ic < len ; ic++ )
    {
    if ( buff[ic] == ';' )
        countpv++ ;
    }

// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' || countpv < 5 )
    {
    *buff = 0 ;
    return ;
    }

CLGString NomOri = pChar ;
//NomOri += ' ' ;     // caractere de fin de champ pour recherche sans bug
CLGString NomAff = strtok( NULL ,";");
CLGString PeriodeDeb = strtok( NULL ,";");
CLGString PeriodeFin = strtok( NULL ,";");
CLGString AltiSemaine = strtok( NULL ,";");
CLGString AltiWeekEnd = strtok( NULL ,";");

CLGString JourDeb = "" ;
CLGString MoisDeb = "" ;
if ( PeriodeDeb != "-" && PeriodeDeb != "" )
    {
    JourDeb = strtok( PeriodeDeb.GetBuffer() , "-" ) ;
    MoisDeb = strtok( NULL , "-" ) ;
    }

CLGString JourFin = "" ;
CLGString MoisFin = "" ;
CLGString AltiSemaineStr = "" ;
CLGString AltiWeekEndStr = "" ;
if ( PeriodeFin != "-" && PeriodeFin != "" )
    {
    JourFin = strtok( PeriodeFin.GetBuffer() , "-" ) ;
    MoisFin = strtok( NULL , "-" ) ;
    AltiSemaineStr = strtok( AltiSemaine.GetBuffer() , "-" ) ;
    AltiWeekEndStr = strtok( AltiWeekEnd.GetBuffer() , "-" ) ;
    }

// pour toutes les zones comparaison du nom
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    CZoneAer * pZone = (*this)[iz] ;

    // si pas bon nom de zone
    if ( strcmp( pZone->m_pNomAff , NomOri ) )
        continue ;

    // recopie nom pour affiche
    if ( NomAff != "-" && NomAff != "" )
        {
        delete [] pZone->m_pNomAff ;
        pZone->m_pNomAff = new char [ NomAff.GetLength() + 1 ] ;
        strcpy( pZone->m_pNomAff , NomAff ) ;
        }
    /*Serial.print(pZone->m_NomOri) ;
    Serial.print("|") ;
    Serial.println(pZone->m_pNomAff) ;*/

    // periode debut
    if ( JourDeb != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_PeriodeDebutJour = atoi( JourDeb ) ;
        pZone->m_pDerogFfvl->m_PeriodeDebutMois = atoi( MoisDeb ) ;
        }

    // periode fin
    if ( JourFin != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_PeriodeFinJour = atoi( JourFin ) ;
        pZone->m_pDerogFfvl->m_PeriodeFinMois = atoi( MoisFin ) ;
        }

    // alti semaine
    if ( AltiSemaineStr != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_AltiBassePeriodeSemaine = atoi( AltiSemaineStr ) ;
        }

    // alti week-end
    if ( AltiWeekEndStr != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_AltiBassePeriodeWeekEnd = atoi( AltiWeekEnd ) ;
        }
    }

// pour prochain appel
*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture fichier activation de zone. nom de zone 2 champs.
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

CLGArray<CLGString> VecNomDejaEcrit ;

// ecriture des zone
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    // zone en memoire
    const CZoneAer * pZone = (*this)[iz] ;

    // zone dans fichier d'activation
    if ( !pZone->m_DansFchActivation )
        continue ;

    // verif si pas deja fait exemple Chastreix
    bool DejaFait = false ;
    for ( int iv = 0 ; iv < VecNomDejaEcrit.GetSize() ; iv++ )
        {
        if ( VecNomDejaEcrit[iv] == pZone->GetNom4Activation() )
            {
            DejaFait = true ;
            break ;
            }
        }

    // si deja ecrit dans le fichier
    if ( DejaFait )
        continue ;

    // empilement pour suivit deja fait
    VecNomDejaEcrit.Add( pZone->GetNom4Activation() ) ;

    // ecriture fichier nom et activation
    Fch.print( pZone->GetNom4Activation() ) ;
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
/// \brief
void CZonesAerAll::GestActivationZone(CZoneAer* pZone, bool Active)
{
// si zone pas dans ficier activation
if ( ! pZone->m_DansFchActivation )
    return ;

// pour toutes les zones de meme nom court 2 champs
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    CZoneAer * pZoneOther = (*this)[iz] ;
    // changement activation
    if ( pZone->GetNom4Activation() == pZoneOther->GetNom4Activation() )
        pZoneOther->m_Activee = Active ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier activation de zone
void CZonesAerAll::LectureFichierZonesActive()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
File FileAct = SD.open(NOM_FCH_ZONE_ACT);
if ( !FileAct )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(FileAct.available())
    {
    char c = FileAct.read() ;
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

// fermeture fichier
FileAct.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne activation zone. nom court 2 champs ou nom long.
void CZonesAerAll::TraiteBufferZoneActive( char * buff )
{
// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' )
    return ;

// champ nom de zone, nom 2 champs
CZoneAer ZoneNomCourt ;
ZoneNomCourt.m_pNomAff = pChar ;
CLGString NomOriCourt = ZoneNomCourt.GetNom4Activation() ;
ZoneNomCourt.m_pNomAff = NULL ;

// champ activation de zone
char * pActivation = strtok( NULL ,";") ;
if ( pActivation == NULL )
    return ;
CLGString Activation = pActivation ;

// pour toutes les zones
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    const CZoneAer & Zone = *(*this)[iz] ;
    if ( NomOriCourt == Zone.GetNom4Activation() )
        {
        CZoneAer * pZone = (*this)[iz] ;

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
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    const CZoneAer & Zone = *(*this)[iz] ;
    if ( ! strcmp( Zone.m_pNomAff , NomOri ) )
        return (*this)[iz] ;
    }

return NULL ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri croissant de nom pour ecran gestion zone
void CZonesAerAll::TriZonesNom()
{
//CSortArray::m_pArr = (*this) ;
//CSortArray::m_Size = GetSize() ;
CZoneAer::TrierPar( CZoneAer::TriParNom ) ;
//TriSchellMetzner() ;
TriAdapte(true) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction de tri de vector.
bool MySortFunction(const CZoneAer *p1,const CZoneAer *p2)
{
return ((*p1)<(*p2));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par altitude croissante des zones dans le vecteur indifferement
/// d'autres parametres
void CZonesAerAll::TriZonesParAltitude( CLGArrayPtr<const CZoneAer> & VecZones )
{
/*CSortArray::m_pArr = & VecZones[0] ;
CSortArray::m_Size = VecZones.size() ;
CZoneAer::TriParNom( false ) ;
TriSchellMetzner(true) ; */
CZoneAer::TrierPar( CZoneAer::TriParAltitude ) ;
//std::sort(VecZones.begin(), VecZones.end(),MySortFunction);
//std::reverse(VecZones.begin(),VecZones.end());
VecZones.TriAdapte(true) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur aux zones necessaires en prenant en compte la derogation
/// et l'activabilite de la zone
void CZonesAerAll::ClasseZoneRetenueAltitude( CLGArrayPtr<const CZoneAer> & VecZonesIn )
{
m_VecZonesRet.SetSize(0) ;

// tri par altitude
TriZonesParAltitude( VecZonesIn ) ;

// tri suivant la derogation
for ( size_t iz = 0 ; iz < VecZonesIn.GetSize()  ; iz++ )
    {
    // zone activable pris en compte apres
    if ( VecZonesIn[iz]->m_DansFchActivation )
        continue ;
    // si zone avec derogation, on efface les autres avant
    if ( VecZonesIn[iz]->HavePeriod() )
        m_VecZonesRet.SetSize(0) ;
    // ajout de la zone
    m_VecZonesRet.Add( VecZonesIn[iz] ) ;
    }

// rajout des zones activables
for ( size_t iz = 0 ; iz < VecZonesIn.GetSize() ; iz++ )
    if ( VecZonesIn[iz]->m_DansFchActivation )
        m_VecZonesRet.Add( VecZonesIn[iz] ) ;

// recopie des zones
VecZonesIn.CopyWithoutAlloc( m_VecZonesRet );

// tri par altitude
TriZonesParAltitude( VecZonesIn ) ;
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
CLGString  RetStrIn = "" ;
int        RetNbrIn = ZONE_EN_DEHORS ;
//CLGString RetStrLimite = "" ;
//int         RetNbrLimite = ZONE_EN_DEHORS ;
char        TmpChar[100] ;

// test dans la zone, position courante
CZoneAer::st_coord_poly PtsEnCours ;
PtsEnCours.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
PtsEnCours.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

#ifdef TMA_DEBUG
SetDate( 2024 , 3, 31 ) ;
//SetDate( 2024 , 2, 25 ) ;
g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = 1500 ;
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

// si test wifi positionnement altitude sol
if ( g_GlobalVar.m_ModeHttp )
    g_GlobalVar.m_AltitudeSolHgt = g_GlobalVar.m_Hgt2Agl.GetGroundZ( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat ) ;

#ifdef TIME_ZONE
unsigned long time_zone = millis() ;
#endif // TIME_ZONE

// pour toutes les zones, determination si dans surface
m_VecZoneInArea.SetSize(0) ;
const CZoneAer * pZoneProtegee = NULL ;             // zone protege
for ( int iz = 0 ; iz < GetSize() ; iz++ )
    {
    CZoneAer & Zone = *(*this)[iz] ;

    // si la zone n'est pas active
    if ( ! Zone.m_Activee )
        {
        Zone.CompressZone() ;
        continue ;
        }

    // si pas dans les bornes lat/lon
    if ( ! Zone.IsInRect(PtsEnCours) )
        {
        Zone.CompressZone() ;
        continue ;
        }

    // decompression de la zone
    Zone.UnCompressZone() ;

    // dedans la surface
    bool IsInArea = CPolygone::IsIn( Zone.GetPolygoneArr() , Zone.GetNbPts() , PtsEnCours ) ;

    // liberation des float si pas assez de meoire
    if ( ! IsEnoughMemory() )
        Zone.CompressZone() ;

    // si dedans la surface
    if ( IsInArea )
        {
        // zone protege
        if ( Zone.IsProtect() )
            pZoneProtegee = & Zone ;
        // zone normale
        else
            m_VecZoneInArea.Add( & Zone ) ;
        }
    }

// classement reduction zone
ClasseZoneRetenueAltitude( m_VecZoneInArea ) ;

/*Serial.println( "debut de tri" ) ;
for ( int iz = 0 ; iz < m_VecZoneInAreaNorm.size() ; iz++ )
    Serial.println( m_VecZoneInAreaNorm[iz]->GetAltiBasse() ) ;*/

// cas des zones normales
const CZoneAer * pZoneIn = NULL ;
if ( m_VecZoneInArea.GetSize() == 0 )
    {
    RetStrIn = "aucune_zone" ;
    RetNbrIn = ZONE_EN_DEHORS ;
    }
else
    {
    // zone la plus basse
    pZoneIn = m_VecZoneInArea[0] ;

    // variables de zone
    int  PlafondZone = pZoneIn->GetAltiBasse() ;

    ////////////
    // si corent
    if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneCorent )
        {
        m_Plafond4Valid = PlafondZone ;

        // si dedans
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec > m_Plafond4Valid) &&
             (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone())) )
            {
            sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // dessous
        else
            {
            sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }

        /*
        // si limite alti
        if ( ((g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec+g_GlobalVar.m_Config.m_AltiMargin) > m_Plafond4Valid) &&
             ((g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone())) )
            {
            sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrLimite = ZONE_LIMITE_ALTI ;
            RetStrLimite = TmpChar ;
            }
        */
        }
    ///////////////
    // si saint yan
    else if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneStYan )
        {
        m_Plafond4Valid = PlafondZone ;

        // si l'on est en pas en week end
        if ( ! m_IsWeekEnd )
            {
            // si dedans
            if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone()) )
                {
                sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
                RetNbrIn = ZONE_DEDANS ;
                RetStrIn = TmpChar ;
                }
            // dessous
            else
                {
                sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
                RetNbrIn = ZONE_DESSOUS ;
                RetStrIn = TmpChar ;
                }

            /*
            // si limite alti
            if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone()) )
                {
                sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
                RetNbrLimite = ZONE_LIMITE_ALTI ;
                RetStrLimite = TmpChar ;
                }
            */
            }
        }
    ////////////////////
    // si zone generique
    else
        {
        // si dedans
        if ( PlafondZone < g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // en dessous
        else
            {
            m_Plafond4Valid = pZoneIn->GetAltiBasse() ;
            sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }

        /*
        // si limite altitude
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec+g_GlobalVar.m_Config.m_AltiMargin) > PlafondZone )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrLimite = ZONE_LIMITE_ALTI ;
            RetStrLimite = TmpChar ;
            }
        */
        }
    }

// liberation memoire
m_VecZoneInArea.SetSize(0) ;
//m_VecZoneInArea.shrink_to_fit() ;

// si zone protege et pas deja dedans une comme TMA ou activable verification hauteur sol
int HauteurSolZoneProtegee = 1000 ;
int PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;
if ( pZoneProtegee != NULL && RetNbrIn != ZONE_DEDANS )
    {
    // hauteur zone protegee
    HauteurSolZoneProtegee = pZoneProtegee->GetHauteurSolZoneProtege() ;

    // actualisation plafond zone protege
    PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;

    // si dedans
    if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec < PlafondZoneProtegee )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrIn = ZONE_DEDANS ;
        sprintf( TmpChar , "Zp %s al:%4dm" , (*pZoneProtegee).m_pNomAff , m_Plafond4Valid ) ;
        RetStrIn = TmpChar ;
        }
    /*// si limite
    else if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec < (PlafondZoneProtegee + g_GlobalVar.m_Config.m_AltiMargin) )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrLimite = ZONE_LIMITE_ALTI ;
        sprintf( TmpChar , "Al %s al:%4dm" , (*pZoneProtegee).m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        }*/
    }

// variables de sortie zone en cours
m_Mutex.PrendreMutex() ;
 m_DansDessousUneZone = RetNbrIn ;
 m_NomZoneDansDessous = RetStrIn ;
m_Mutex.RelacherMutex() ;

#ifdef TIME_ZONE
unsigned long ecart = millis() - time_zone ;
Serial.print( "time calc zone ms : " ) ;
Serial.println( ecart ) ;
#endif

/*
// raz des variables distance prochaine zone et mise a jour distance altitude
if ( pZoneIn != NULL )
    {
    int PlafondZone = pZoneIn->GetAltiBasse() ;
    int DistAltCurZone = PlafondZone - g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;

    //////////////////////////////////////////////
    // si corent prise en compte de l'altitude sol
    if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneCorent )
        {
        int AltitudeSolPlus300 = g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone() ;
        int DistAltSol = AltitudeSolPlus300 - g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;
        m_DistAltCurZone = std::max( DistAltSol , DistAltCurZone ) ;
        }
    //////////////////////////////////////////////
    // si st yan prise en compte de l'altitude sol
    else if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneStYan )
        {
        int AltitudeSolPlus610 = g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone() ;
        int DistAltSol = AltitudeSolPlus610 - g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;
        m_DistAltCurZone = DistAltSol ;
        }
    else
        m_DistAltCurZone = DistAltCurZone ;
    }
else
    m_DistAltCurZone = 9999 ;

// determination des zones proches et à l'altitude de croisement dans les 1.5 km
std::vector<CZoneAer *> VecZoneProchesXY ;   // zones proches
for ( int iz = 0 ; iz < GetSize(); iz++ )
    {
    CZoneAer * pZoneXY = (*this)[iz] ;

    // si la zone n'est pas active
    if ( !pZoneXY->m_Activee )
        continue ;

    // pas la zone en cours
    if ( pZoneIn == pZoneXY )
        continue ;

    // si zone st yan le week end
    if ( pZoneXY->GetTypeZone() == CZoneAer::ZoneStYan && m_IsWeekEnd )
        continue ;

    // dans le rayon de 1.5 km
    float DistBaryMetres = sqrtf( powf(pZoneXY->m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(pZoneXY->m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayonProche = DistBaryMetres < ( pZoneXY->m_RayonMetre + DISTANCE_PROCHE_XY ) ;

    // si pas dans le rayon proche
    if ( !DansLeRayonProche )
        continue ;

    // prise en compte de l'altitude de la zone proche
    bool IsNearFrontAlti = false ;
    if ( pZoneXY->IsProtect() )
        // zone proche protege
        IsNearFrontAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec <= (PlafondZoneProtegee+g_GlobalVar.m_Config.m_AltiMargin) ;
    else
        {
        // zone proche generique
        IsNearFrontAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec >= (pZoneXY->GetAltiBasse()-g_GlobalVar.m_Config.m_AltiMargin) ;
        // si saint sandoux proche CTR Corent alors prise en compte hauteur sol
        if ( pZoneXY->GetTypeZone() == CZoneAer::ZoneCorent )
            // altitude majoree marge alti > altitude sol plus 300m
            IsNearFrontAlti &= (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt + pZoneXY->GetAltiSolZone())  ;
        }

    // si a l'altitude de croisement
    if ( IsNearFrontAlti )
        {
        VecZoneProchesXY.push_back( pZoneXY ) ;
        pZoneXY->UnCompressZone() ;
        // distance et point frontiere proche
        pZoneXY->m_DistanceFrontiere = CDistFront::IsNearFront( pZoneXY->m_PolygoneArr , pZoneXY->m_NbPts , PtsEnCours , pZoneXY->m_CapFrontProche ) ;
        pZoneXY->FreeFloat() ;
        }
    }

// tri distance minimale des zones
CZoneAer::TrierPar( CZoneAer::TriParDistance ) ;
std::sort(VecZoneProchesXY.begin(), VecZoneProchesXY.end(),MySortFunction);

// si zone la plus proche
m_DistXYNextZone = 9999 ;
m_CapFrontProche = -1 ;
if ( VecZoneProchesXY.size() )
    {
    const CZoneAer * pZoneProche = VecZoneProchesXY[0] ;

    // distance et point prochaine zone
    m_DistXYNextZone = pZoneProche->m_DistanceFrontiere ;
    m_CapFrontProche = pZoneProche->m_CapFrontProche ;

    // si marge distance atteinte
    if ( pZoneProche->m_DistanceFrontiere <= g_GlobalVar.m_Config.m_XYMargin )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom + altitude
        if ( pZoneProche->IsProtect() )
            m_Plafond4Valid = HauteurSolZoneProtegee ;
        else
            m_Plafond4Valid = pZoneProche->GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , pZoneProche->m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        }
    }
*/

/*
// pour toutes les zones recherche frontiere XY proche
for ( int iz = 0 ; iz < GetSize() && RetNbrLimite != ZONE_LIMITE_ALTI && RetNbrIn != ZONE_DEDANS && g_GlobalVar.m_Config.m_XYMargin > 0 ; iz++ )
    {
    CZoneAer * pZoneXY = (*this)[iz] ;

    // si la zone n'est pas active
    if ( !pZoneXY->m_Activee )
        continue ;

    // dans le du rayon + marge
    float DistBaryMetres = sqrtf( powf(pZoneXY->m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(pZoneXY->m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayonMarge = DistBaryMetres < (pZoneXY->m_RayonMetre+g_GlobalVar.m_Config.m_XYMargin) ;

    // si pas dans le rayon + marge
    if ( !DansLeRayonMarge )
        continue ;

    // decompression de la zone
    pZoneXY->UnCompressZone() ;

    // dedans la surface
    bool IsInArea = (pZoneIn == pZoneXY) ;
    // proche de la frontiere de zone
    bool IsNearFront = !IsInArea && CDistFront::IsNearFront( pZoneXY->m_PolygoneArr , pZoneXY->m_NbPts , PtsEnCours ) ;
    // determionation zone protege
    bool IsZoneProtege = IsNearFront && pZoneXY->IsProtect() ;

    // liberation des float
    pZoneXY->FreeFloat() ;

    // prise en compte de l'altitude
    bool IsNearFrontAlti ;
    if ( IsZoneProtege )
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec <= (PlafondZoneProtegee+g_GlobalVar.m_Config.m_AltiMargin)) ;
    else
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec >= (pZoneXY->GetAltiBasse()-g_GlobalVar.m_Config.m_AltiMargin)) ;

    // positionnement retour
    if ( IsNearFrontAlti )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom + altitude
        if ( IsZoneProtege )
            m_Plafond4Valid = HauteurSolZoneProtegee ;
        else
            m_Plafond4Valid = pZoneXY->GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , pZoneXY->m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        break ;
        }
    } */

/*
// variables de sortie fontiere
m_Mutex.PrendreMutex() ;
 m_LimiteZone      = RetNbrLimite ;
 m_NomZoneEnLimite = RetStrLimite ;
m_Mutex.RelacherMutex() ;
*/
}
