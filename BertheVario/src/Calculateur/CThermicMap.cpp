////////////////////////////////////////////////////////////////////////////////
/// \file CThermicMap.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 03/04/2024
///

#include "../BertheVario.h"

CThermicMap g_TermicMap ;

int CVzSum::ms_x_cur ;    ///< index x position courante pour calcul et tri proximite
int CVzSum::ms_y_cur ;    ///< index y position courante pour calcul et tri proximite
int CVzSum::ms_z_cur ;    ///< index z position courante pour calcul et tri proximite
bool CVzSum::ms_TriDistance = true ; ///< tri distance ou Vz

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui positionne les index de la position courantes
/// pour calcul de proximite
void CVzSum::SetPosCourante()
{
st_key key ;
key.SetXYZRec( g_GlobalVar.m_TerrainPosCur.m_Lon ,
               g_GlobalVar.m_TerrainPosCur.m_Lat ,
               g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;
ms_x_cur = key.m_x ;
ms_y_cur = key.m_y ;
ms_z_cur = key.m_z ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui renvoie la distance au carre par rapport a la position courante
float CVzSum::GetDistPosCurSquare() const
{
float diff_x = m_key.m_x - ms_x_cur ;
float diff_y = m_key.m_y - ms_y_cur ;
float diff_z = m_key.m_z - ms_z_cur ;
float dist_carre = diff_x*diff_x*ResolutionEnMetreMapXY*ResolutionEnMetreMapXY +
                   diff_y*diff_y*ResolutionEnMetreMapXY*ResolutionEnMetreMapXY +
                   diff_z*diff_z*ResolutionEnMetreMapZ*ResolutionEnMetreMapZ ;
return dist_carre ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Operateur de comparaison distance 3D par rapport a la position courante.
bool CVzSum::operator < ( const CVzSum & Vz ) const
{
if ( ms_TriDistance )
    return GetDistPosCurSquare() < Vz.GetDistPosCurSquare() ;
return GetAverage() < Vz.GetAverage() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Operateur de comparaison distance 3D par rapport a la position courante.
bool CVzSum::operator > ( const CVzSum & Vz ) const
{
if ( ms_TriDistance )
    return GetDistPosCurSquare() > Vz.GetDistPosCurSquare() ;
return GetAverage() > Vz.GetAverage() ;
}

/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul des index pour generation de clee map. Suivant les resolutions
/// ResolutionEnMetreMapXY et ResolutionEnMetreMapZ origine point de decollage
void st_key::SetXYZRec( float LonDeg , float LatDeg, int Alti )
{
m_x = (int) ((LonDeg-g_GlobalVar.m_TerrainPosDeco.m_Lon) * 60. * UnMileEnMetres / ResolutionEnMetreMapXY) ;
m_y = (int) ((LatDeg-g_GlobalVar.m_TerrainPosDeco.m_Lat) * 60. * UnMileEnMetres / ResolutionEnMetreMapXY) ;
m_z = (int) ((Alti-g_GlobalVar.m_TerrainPosDeco.m_AltiBaro) / ResolutionEnMetreMapZ) ;
}

/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief lance la tache termic
void CThermicMap::LancerTacheTermic()
{
xTaskCreatePinnedToCore( TacheTermic, "TermicTask", TERMIC_STACK_SIZE, this  , TERMIC_PRIORITY , NULL, TERMIC_CORE );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief fonction static qui empile les points de termic à 4hz apres debut du vol.
void CThermicMap::TacheTermic(void* param)
{
/*g_GlobalVar.m_TerrainPosDeco.m_AltiBaro =
g_GlobalVar.m_TerrainPosDeco.m_Lon =
g_GlobalVar.m_TerrainPosDeco.m_Lat = 0 ;

g_GlobalVar.m_TerrainPosCur.m_AltiBaro = 0 ;
g_GlobalVar.m_TerrainPosCur.m_Lon = 0.1 ;
g_GlobalVar.m_TerrainPosCur.m_Lat = 0.1 ;
g_TermicMap.PushVz( 0.5 , g_GlobalVar.m_TerrainPosCur.m_Lat , g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;

g_GlobalVar.m_TerrainPosCur.m_AltiBaro = 0 ;
g_GlobalVar.m_TerrainPosCur.m_Lon = 0.2 ;
g_GlobalVar.m_TerrainPosCur.m_Lat = 0.2 ;
g_TermicMap.PushVz( 0.6 , g_GlobalVar.m_TerrainPosCur.m_Lat , g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;

g_GlobalVar.m_TerrainPosCur.m_AltiBaro = 0 ;
g_GlobalVar.m_TerrainPosCur.m_Lon = 0.3 ;
g_GlobalVar.m_TerrainPosCur.m_Lat = 0.3 ;
g_TermicMap.PushVz( 0.6 , g_GlobalVar.m_TerrainPosCur.m_Lat , g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;

g_GlobalVar.m_TerrainPosCur.m_Lon = 0.1 ;
g_GlobalVar.m_TerrainPosCur.m_Lat = 0.1 ;

g_TermicMap.CalcTermicProche() ;

return ;*/

int iboucle = 0 ;

while( g_GlobalVar.m_TaskArr[TERMIC_NUM_TASK].m_Run )
    {
    // 4 hz
    delay( 250 ) ;

    #ifndef TERMIC_DEBUG
    // si le vol n'a pas demmare
    if ( g_GlobalVar.m_DureeVolMin < 0 )
        continue ;
    #endif // TERMIC_DEBUG

    // si pas de calcul termic
    if ( !g_GlobalVar.m_Config.m_Termic )
        {
        g_TermicMap.clear() ;
        g_TermicMap.m_VecVzAutour.clear() ;
        g_TermicMap.m_VecVzAutour.shrink_to_fit() ;
        continue ;
        }

    // calcul du voisinage toutes les 30s
    iboucle++ ;
    if ( !(iboucle%120) )
        {
        g_TermicMap.CalcVoisinage() ;
        // liberation de la memoire avec les voisins
        g_TermicMap.CleanMap(true) ;
        }


    // empilement du point de vol
    g_TermicMap.PushVz( g_GlobalVar.m_VitVertMS , g_GlobalVar.m_TerrainPosCur.m_Lat , g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;
    }

g_GlobalVar.m_TaskArr[TERMIC_NUM_TASK].m_Stopped = true ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ajoute une Vz à la position donnée dans la map historique.
void CThermicMap::PushVz( float Vz , float LatDeg , float LonDeg , int Alti )
{
// nettoyage de la map
CleanMap() ;

// indexe en ResolutionEnMetreMapXY et Z
st_key key ;
key.SetXYZRec( LonDeg , LatDeg , Alti ) ;

//Serial.print( "push vz : " ) ;
//Serial.println( key.m_y ) ;

// ajout a la map
m_MutexMap.PrendreMutex() ;
 (*this)[key].AddVz( Vz , key ) ;
m_MutexMap.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Nettoie la map des Vz en dessous d'un certain seuil pour liberer
/// de la memoire. Avec taille minimale de memoire libre.
/// \param WithNeighbour [in] : prend en compte le voisinage.
void CThermicMap::CleanMap(bool WithNeighbour)
{
float VzMin = VzMin4Clear ;

// concurence PushVz()
m_MutexMap.PrendreMutex() ;

do  {
    // test des case des degueulante
    std::vector<st_key> VecKeyToDelete ;
    VecKeyToDelete.reserve( size() ) ;
    for ( const auto& Pos : (*this) )
        {
        // test de la vz et du voisinage
        if ( Pos.second.GetAverage() < VzMin )
            VecKeyToDelete.push_back( Pos.first ) ;

        if ( WithNeighbour && Pos.second.m_Voisinage < m_NombreVoisinMin )
            VecKeyToDelete.push_back( Pos.first ) ;
        }

    // supression de ces cases
    for( int iv = 0 ; iv < VecKeyToDelete.size() ; iv++ )
        erase( VecKeyToDelete[iv] ) ;

    // augmentation de la VzMin
    VzMin += 0.2 ;
    }
while ( esp_get_free_heap_size() < 1024 * 30 ) ;   // 30kb minimum de memoire libre

// concurence PushVz()
m_MutexMap.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul le voisnnage pour un effacement des case isolée
void CThermicMap::CalcVoisinage()
{
// en concurence avec CalcTermicProche()
m_MutexMap.PrendreMutex() ;

// raz du voisinage
//Serial.print("count:");
//Serial.println(size());
for ( auto& Pos : (*this) )
    Pos.second.m_Voisinage = 0 ;

for ( auto& Pos : (*this) )
    {
    CVzSum & VzCur = Pos.second ;
    const st_key key = VzCur.m_key ;

    // pour tout le volume
    bool VoisinsTrouve = false ;
    for ( int x = key.m_x - m_VolumeVoisinage ; x < key.m_x + m_VolumeVoisinage ; x++ )
        {
        for ( int y = key.m_y - m_VolumeVoisinage ; y < key.m_y + m_VolumeVoisinage ; y++ )
            {
            for ( int z = key.m_z - m_VolumeVoisinage ; z < key.m_z + m_VolumeVoisinage ; z++ )
                {
                st_key key_v ;
                key_v.m_x=x ;
                key_v.m_y=y ;
                key_v.m_z=z ;

                // si meme posiion
                if ( key_v == key )
                    continue ;

                // si voisin existe
                auto search = (*this).find(key_v) ;
                if ( search == (*this).end() )
                    continue ;

                // si voisin ascendance aussi
                const CVzSum & Vz_v = search->second ;
                if ( Vz_v.GetAverage() >= g_GlobalVar.m_Config.m_vz_seuil_haut )
                    VzCur.m_Voisinage++ ;

                if ( VzCur.m_Voisinage >= m_NombreVoisinMin )
                    {
                    VoisinsTrouve = true ;
                    break ;
                    }
                }
            if ( VoisinsTrouve )
                break ;
            }
        if ( VoisinsTrouve )
            break ;
        }
    }

m_MutexMap.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Donne le cap/distance du thermique le plus proche
void CThermicMap::CalcTermicProche()
{
// vecteur des Vz
m_VecVzAutour.clear() ;

// taille du vecteur
m_VecVzAutour.reserve( size() )  ;

// concurence PushVz()
m_MutexMap.PrendreMutex() ;

// pour toutes les positions memoires, recherche de Vz moyenne au dessus d'un seuil
for ( const auto& Pos : (*this) )
    {
    // clee dans la map
    const CVzSum & InstVz = Pos.second ;

    // moyenne des Vz de cette case
    float VzCur = InstVz.GetAverage() ;

    // si Vz minimum depassée enregistrement case
    float VzMin = g_GlobalVar.m_Config.m_vz_seuil_haut ;
    if ( VzCur >= VzMin )
        m_VecVzAutour.push_back( InstVz ) ;
    }

// concurence PushVz()
m_MutexMap.RelacherMutex() ;

// tri fonction de la distance la plus proche de la position courante
CVzSum::SetPosCourante() ;
CVzSum::SetTriDistance(true) ;
std::sort( m_VecVzAutour.begin() , m_VecVzAutour.end() ) ;

#ifndef TERMIC_DEBUG
// si tableau vide (debut de vol)
if ( m_VecVzAutour.size() == 0 )
    {
    // cap et distance
    m_GisementDeg = -1 ;
    m_DistanceMetres = -1 ;
    return ;
    }

// indexe en ResolutionEnMetreMapXY et Z de position courante
st_key key_cur ;
key_cur.SetXYZRec( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat , g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;

// calcul coordonnes du termic le plus proche
int Lat_y = m_VecVzAutour[0].m_key.m_y - key_cur.m_y ;
int Lon_x = m_VecVzAutour[0].m_key.m_x - key_cur.m_x ;
#endif

#ifdef TERMIC_DEBUG
int Lon_x  ;
int Lat_y  ;
static long ib = 0 ;
if ( ib == 0 )
    {
    Lon_x = 0 ;
    Lat_y =  1 ;
    }
else if ( ib == 1 )
    {
    Lon_x = 1 ;
    Lat_y = 1 ;
    }
else if ( ib == 2 )
    {
    Lon_x = 1 ;
    Lat_y = 0 ;
    }
else if ( ib == 3 )
    {
    Lon_x = 1 ;
    Lat_y = -1 ;
    }
else if ( ib == 4 )
    {
    Lon_x =  0 ;
    Lat_y = -1 ;
    }
else if ( ib == 5 )
    {
    Lon_x = -1 ;
    Lat_y = -1 ;
    }
else if ( ib == 6 )
    {
    Lon_x = -1 ;
    Lat_y =  0 ;
    }
else if ( ib == 7 )
    {
    Lon_x = -1 ;
    Lat_y =  1 ;
    }
ib++;
ib = ib%8 ;
#endif

// cap/distance du termic
float GisDeg =  180 - 180. / T_PI * atan2f( Lat_y , Lon_x ) - 90.  ;
m_DistanceMetres = sqrtf( Lat_y*Lat_y + Lon_x*Lon_x ) * ResolutionEnMetreMapXY ;
if ( GisDeg < 0. )
    GisDeg += 360. ;
if ( GisDeg >= 360. )
    GisDeg -= 360. ;
m_GisementDeg = GisDeg ;
}

/*
std::map<st_key, CPts> MyMap ;
CPts Pt1 = {1.,2} ;
CPts Pt2 = {10.,20} ;
st_key k1 ; k1.m_x = 3 ; k1.m_y = 4 ; k1.m_z = 5 ;
st_key k2 ; k2.m_x = 7 ; k2.m_y = 8 ; k2.m_z = 9 ;
st_key k3 ; k3.m_x = 70 ; k3.m_y = 80 ; k3.m_z = 90 ;
MyMap[k1] = Pt1 ;
MyMap[k2] = Pt2 ;

auto search = MyMap.find( k2 ) ;
Serial.println( search == MyMap.end() ) ;
for (const auto& n : MyMap )
    {
    Serial.print( n.first.m_x ) ;
    Serial.print( " " ) ;
    Serial.println( n.second.m_VzSum ) ;
    }
*/
