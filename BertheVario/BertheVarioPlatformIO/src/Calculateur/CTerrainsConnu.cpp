////////////////////////////////////////////////////////////////////////////////
/// \file CTerrainsConnu.cpp
///
/// \brief
///
/// \date creation   : 17/03/2024
/// \date 23/10/2025 : fabsf dans CalcFinesse()
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
CTerrainsConnu::~CTerrainsConnu()
{
// le deco n'est pas un terrain new
if ( GetSize() > 0 )
    {
    for ( int it = 0 ; it < GetSize() ; it++ )
        if ( (*this)[it] == & g_GlobalVar.m_TerrainPosDeco )
            {
            RemoveAt(it) ;
            break ;
            }
    }
DeleteAll() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne la finesse par rapport a la position courante.
/// Ainsi que du gisement.distance.
void CLocTerrain::CalcFinesse()
{
float Lat_y = m_Lat - g_GlobalVar.m_TerrainPosCur.m_Lat ;
float Lon_x = m_Lon - g_GlobalVar.m_TerrainPosCur.m_Lon ;

float DeltaLat = ( Lat_y ) * 60. * UnMileEnMetres ;
float DeltaLon = ( Lon_x ) * 60. * UnMileEnMetres ;
float DeltaAlt = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec - m_AltiBaroRec ;

m_DistanceMetres = sqrtf( powf(DeltaLat,2.) + powf(DeltaLon,2.) ) ;
//m_GisementDeg    = 180 - 180. / T_PI * atan2f( Lat_y , Lon_x ) - 90.  ;

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
/// \brief
CTerrainsConnu::CTerrainsConnu()
{
SetDeleteObjet(true) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CTerrainsConnu::DeleteAll()
{
for ( int i = 0 ; i < GetSize() ; i++ )
    {
    if ( GetAt(i) == & g_GlobalVar.m_TerrainPosDeco )
        continue ;
    delete ElementAt(i) ;
    }
SetSize(0) ;
FreeExtra() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier de terrains et ajout position de debut de vol.
void CTerrainsConnu::LireFichierTerrains(const char * NameFch)
{
char * TmpChar = new char [10000] ;
int ic = 0 ;

// ouverture fichier
File file = SD.open(NameFch);
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

// lignes du fichier
CLGArrayPtr<CLGString> VecLigne ;
VecLigne.SetDeleteObjet(true) ;

// decoupage en ligne
char * pChar = strtok( TmpChar , "\n" ) ;
while ( pChar != NULL )
    {
    // recopie
    CLGString * pLigne = new CLGString ;
    *pLigne = pChar ;
    VecLigne.Add( pLigne ) ;
    pChar = strtok( NULL , "\n" ) ;
    }

// analyse des champs
char Separ[] = " \t" ;
for ( int i = 0 ; i < VecLigne.GetSize() ; i++ )
    {
    char * pLigne = VecLigne[i]->GetBuffer() ;
    char * pChar = strtok( pLigne , Separ ) ;

    // si commentaire ou ligne vide
    if ( pChar == NULL || *pChar == 0 || *pChar == '#' )
        continue ;

   // ajout terrain
    CLocTerrain * pTerrain = new CLocTerrain ;
    Add( pTerrain ) ;
    for ( int ipar = 0 ; ipar < 4 && pChar != NULL ; ipar++ )
        {
        if ( ipar == 0 )
            pTerrain->m_Nom = pChar ;
        else if ( ipar == 1 )
            pTerrain->m_AltiBaroRec = atof( pChar ) ;
        else if ( ipar == 2 )
            pTerrain->m_Lat = atof( pChar ) ;
        else if ( ipar == 3 )
            pTerrain->m_Lon = atof( pChar ) ;

        pChar = strtok( NULL , Separ ) ;
        }
    }

// ajout du terrains de decollage
Add( & g_GlobalVar.m_TerrainPosDeco ) ;

// liberation memoire des lignes
VecLigne.DeleteAll() ;

// liberation du buffer de lecture
delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie le terrain avec la plus petite finesse air calme.
void CTerrainsConnu::CalcTerrainPlusProche()
{
//CSortArray::m_pArr = m_pTerrainsArr ;

// init des finesses
for ( int i = 0 ; i < GetSize() ; i++ )
    (*this)[i]->CalcFinesse() ;

// tri la plus petite finesse
TriAdapte( true ) ;

// raz finesse
m_Finesse = FINESSE_IMPOSSIBLE ;

// calcul de finesse si possible
if ( GetSize() == 0 )
    return ;

// finesse la plus petite
m_Finesse = (*this)[0]->m_Finesse ;

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
if ( GetSize() > 0 )
    pTerrain = (*this)[0] ;

// finesse positive et 1 terrain
if ( pTerrain != NULL && m_Finesse < FINESSE_IMPOSSIBLE )
    return pTerrain ;

return NULL ;
}
