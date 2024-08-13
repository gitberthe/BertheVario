////////////////////////////////////////////////////////////////////////////////
/// \file CPolygone.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 13/08/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Test si on est dans le polygone de la zone. Methode par droites secantes.
/// \return true si points dans polygone
bool CPolygone::IsIn( CZoneAer::st_coord_poly ** PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsToTest )
{
// test si le point est un des sommets du polygone
for ( int ip = 0 ; ip < NbPts ; ip++ )
    if ( PtsArr[ip]->m_Lat == PtsToTest.m_Lat &&
         PtsArr[ip]->m_Lon == PtsToTest.m_Lon )
        return true ;

// comptage du nombre d'intersections avec le polygone
int NbInter = 0 ;
struct Point ptA1 ;
struct Point ptB1 ;
// un point tres loin
static struct Point ptLoin = { 0. , 170. } ;
struct Point ptTest = {PtsToTest.m_Lat,PtsToTest.m_Lon} ;
for ( int ip = 0 ; ip < NbPts ; ip++ )
    {
    // points du segmant de zone
    if ( ip < (NbPts-1) )
        {
        ptA1.x = PtsArr[ip]->m_Lat ;
        ptA1.y = PtsArr[ip]->m_Lon ;
        ptB1.x = PtsArr[ip+1]->m_Lat ;
        ptB1.y = PtsArr[ip+1]->m_Lon ;
        }
    else
        {
        ptA1.x = PtsArr[ip]->m_Lat ;
        ptA1.y = PtsArr[ip]->m_Lon ;
        ptB1.x = PtsArr[0]->m_Lat ;
        ptB1.y = PtsArr[0]->m_Lon ;
        }

    if ( doIntersect( ptA1 , ptB1 , ptTest , ptLoin ) )
        NbInter++ ;
    }

// si nombre d'intersection impair, alors on est dedans le polygone
if ( NbInter%2 )
    return true ;

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Test si on est dans le polygone de la zone. Methode somme des angles.
/// \return true si points dans polygone
bool CPolygone::IsInOld( CZoneAer::st_coord_poly ** PtsArr , int NbPts , const CZoneAer::st_coord_poly & PtsToTest )
{
//Serial.print( "nb pts:" ) ;
//Serial.println( NbPts ) ;

// pour tous les segments
float SommeAngle = 0. ;
for ( int is = 0 ; is < NbPts ; is++ )
    {
    int ideb = is ;
    int ifin = is+1 ;
    if ( is == NbPts - 1 )
        {
        ideb = is ;
        ifin  = 0 ;
        }
    const CZoneAer::st_coord_poly & PtsCourDebut = *PtsArr[ideb] ;
    const CZoneAer::st_coord_poly & PtsCourFin   = *PtsArr[ifin] ;
    CVecteur3D VecteurDebut( PtsCourDebut.m_Lon - PtsToTest.m_Lon , PtsCourDebut.m_Lat - PtsToTest.m_Lat , 0. ) ;
    CVecteur3D VecteurFin(   PtsCourFin.m_Lon   - PtsToTest.m_Lon , PtsCourFin.m_Lat   - PtsToTest.m_Lat , 0. ) ;

    // vecteur normal du segment courant
    CVecteur3D VecPerpendiculaireCur = VecteurDebut.GetProduitVectoriel( VecteurFin ) ;

    // si signe de m_z different du dernier segment
    float Angle = VecteurDebut.GetAngleDeg( VecteurFin ) ;
    if ( VecPerpendiculaireCur.m_z < 0. )
        SommeAngle += Angle ;
    else
        SommeAngle -= Angle ;
    }

//Serial.println( SommeAngle ) ;
if ( fabsf( SommeAngle ) >= 359. )
    return true ;
return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi l'aire du polygone
float CPolygone::GetAreaSize( CZoneAer::st_coord_poly ** PtsArr , int NbPts , CZoneAer::st_coord_poly & PtsBarycentre )
{
float SommeTriangle = 0 ;

// calcul du baricentre
PtsBarycentre.m_Lat = 0 ;
PtsBarycentre.m_Lon = 0 ;
for ( int is = 0 ; is < NbPts ; is++ )
    {
    const CZoneAer::st_coord_poly & PtsCour = *PtsArr[is] ;
    PtsBarycentre.m_Lat += PtsCour.m_Lat ;
    PtsBarycentre.m_Lon += PtsCour.m_Lon ;
    }
PtsBarycentre.m_Lat /= NbPts ;
PtsBarycentre.m_Lon /= NbPts ;

// pour tous les segments
for ( int is = 0 ; is < NbPts ; is++ )
    {
    int ideb = is ;
    int ifin = is+1 ;
    if ( is == NbPts - 1 )
        {
        ideb = is ;
        ifin  = 0 ;
        }
    const CZoneAer::st_coord_poly & PtsCourDebut = *PtsArr[ideb] ;
    const CZoneAer::st_coord_poly & PtsCourFin   = *PtsArr[ifin] ;
    CVecteur3D VecteurDebut( PtsCourDebut.m_Lon - PtsBarycentre.m_Lon , PtsCourDebut.m_Lat - PtsBarycentre.m_Lat , 0. ) ;
    CVecteur3D VecteurFin(   PtsCourFin.m_Lon   - PtsBarycentre.m_Lon , PtsCourFin.m_Lat   - PtsBarycentre.m_Lat , 0. ) ;

    // vecteur normal du segment courant
    CVecteur3D VecPerpendiculaireCur = VecteurDebut.GetProduitVectoriel( VecteurFin ) ;

    // si signe de m_z different du dernier segment
    SommeTriangle += VecPerpendiculaireCur.GetNorm() * 0.5 ;
    }

return SommeTriangle ;
}
