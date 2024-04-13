////////////////////////////////////////////////////////////////////////////////
/// \file CTerrainsConnu.cpp
///
/// \brief
///
/// \date creation     : 17/03/2024
/// \date modification : 01/04/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne la finesse par rapport a la position courante.
/// Ainsi que du gisement.distance.
void CLocTerrain::CalcFinesse()
{
float Lat_y = m_Lat - g_GlobalVar.m_TerrainPosCur.m_Lat ;
float Lon_x = m_Lon - g_GlobalVar.m_TerrainPosCur.m_Lon ;

float DeltaLat = fabsf( Lat_y ) * 60. * UnMileEnMetres ;
float DeltaLon = fabsf( Lon_x ) * 60. * UnMileEnMetres ;
float DeltaAlt = g_GlobalVar.m_TerrainPosCur.m_AltiBaro - m_AltiBaro ;

m_DistanceMetres = sqrtf( powf(DeltaLat,2.) + powf(DeltaLon,2.) ) ;
m_GisementDeg    = 180 - 180. / T_PI * atan2f( Lat_y , Lon_x ) - 90.  ;

if ( DeltaAlt > 0. )
    m_Finesse = m_DistanceMetres / DeltaAlt ;
else
    m_Finesse = FINESSE_IMPOSSIBLE ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Comparaison de la finesse.
bool CLocTerrain::operator > ( const CLocTerrain & PosPt ) const
{
if ( m_Finesse > PosPt.m_Finesse )
    return true ;
return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Comparaison de la finesse.
bool CLocTerrain::operator < ( const CLocTerrain & PosPt ) const
{
if ( m_Finesse < PosPt.m_Finesse )
    return true ;
return false ;
}

/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier de terrains et ajout position de debut de vol.
void CTerrainsConnu::LireFichierTerrains()
{
char * TmpChar = new char [10000] ;
int ic = 0 ;

// ouverture fichier
File file = SD.open(TERRAIN_FCH);
if ( !file )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(file.available())
    {
    TmpChar[ic++] = file.read();
    }
TmpChar[ic++] = 0 ;

file.close();

std::vector<char*> VecLigne ;
std::vector<CLocTerrain*> VecTerrain ;

// decoupage en ligne
char * pChar = strtok( TmpChar , "\n" ) ;
while ( pChar != NULL )
    {
    // recopie
    char * pLigne = new char[1000] ;
    strcpy( pLigne , pChar ) ;
    VecLigne.push_back( pLigne ) ;
    pChar = strtok( NULL , "\n" ) ;
    }

// analyse des champs
char Separ[] = " \t" ;
for ( int i = 0 ; i < VecLigne.size() ; i++ )
    {
    char * pLigne = VecLigne[i] ;
    char * pChar = strtok( pLigne , Separ ) ;

    // si commentaire ou ligne vide
    if ( pChar == NULL || *pChar == 0 || *pChar == '#' )
        continue ;

   // ajout terrain
    CLocTerrain * pTerrain = new CLocTerrain ;
    VecTerrain.push_back( pTerrain ) ;
    for ( int ipar = 0 ; ipar < 4 && pChar != NULL ; ipar++ )
        {
        if ( ipar == 0 )
            pTerrain->m_Nom = pChar ;
        else if ( ipar == 1 )
            pTerrain->m_AltiBaro = atof( pChar ) ;
        else if ( ipar == 2 )
            pTerrain->m_Lat = atof( pChar ) ;
        else if ( ipar == 3 )
            pTerrain->m_Lon = atof( pChar ) ;

        pChar = strtok( NULL , Separ ) ;
        }
    }

// ajout des terrains au tableau local
m_pTerrainsArr = new CLocTerrain* [VecTerrain.size() + 1] ;
int iter = 0 ;
for ( ; iter < VecTerrain.size() ; iter++ )
    m_pTerrainsArr[iter] = VecTerrain[iter] ;
m_pTerrainsArr[iter++] = & g_GlobalVar.m_TerrainPosDeco ;
CSortArray::m_Size = iter ;

// liberation memoire des lignes
for ( int i = 0 ; i < VecLigne.size() ; i++ )
    delete [] VecLigne[i] ;

// liberation du buffer de lecture
delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie le terrain avec la plus petite finesse air calme.
void CTerrainsConnu::CalcTerrainPlusProche()
{
CSortArray::m_pArr = m_pTerrainsArr ;

// init des finesses
for ( int i = 0 ; i < m_Size ; i++ )
    m_pTerrainsArr[i]->CalcFinesse() ;

// tri la plus petite finesse
CSortArray::TriSchellMetzner( true ) ;

// raz finesse
m_Finesse = FINESSE_IMPOSSIBLE ;

// calcul de finesse si possible
if ( m_Size == 0 || m_pTerrainsArr == NULL )
    return ;

// finesse la plus petite
m_Finesse = m_pTerrainsArr[0]->m_Finesse ;

/*Serial.println( m_pTerrainsArr[0]->m_Lat ) ;
Serial.println( Lat ) ;
Serial.println( m_Finesse ) ;*/
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie le terrain avec la plus petite finesse.
const CLocTerrain * CTerrainsConnu::GetTerrainProche( float & Finesse )
{
Finesse = m_Finesse ;

// finesse trop grande
if ( Finesse > 99 )
    Finesse = 99 ;

const CLocTerrain * pTerrain = NULL ;
if ( m_pTerrainsArr != NULL && m_Size > 0 )
    pTerrain = m_pTerrainsArr[0] ;

// finesse positive et 1 terrain
if ( m_pTerrainsArr != NULL && m_Finesse < FINESSE_IMPOSSIBLE )
    return pTerrain ;

return NULL ;
}
