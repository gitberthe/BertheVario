////////////////////////////////////////////////////////////////////////////////
/// \file CDetTrans.cpp
///
/// \brief
///
/// \date creation     : 07/03/2024
/// \date modification : 12/04/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi si le cap n'a pas fait 40deg en m_Size secondes.
bool CDetTrans::IsInTrans() const
{
if ( m_Size == 0 )
    return false ;

// cumul des differences de caps sur x secondes
float Angle = 0 ;
for ( int ic1 = 0 ; ic1 < m_Size-1 ; ic1++ )
    {
    float Angle1Rad = m_pCapArr[ic1] * T_PI / 180. ;
    float Angle2Rad = m_pCapArr[ic1+1] * T_PI / 180. ;
    CVecteur2D VecA( cosf(Angle1Rad) , sinf(Angle1Rad) ) ;
    CVecteur2D VecB( cosf(Angle2Rad) , sinf(Angle2Rad) ) ;

    Angle += VecA.GetAngleDeg(VecB) ;
    }

// si en virage tout le lon
if ( fabsf(Angle) <= g_GlobalVar.m_Config.m_droit_deg )
    return true ;

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Empile un cap dans le buffer circulaire
void CDetTrans::PushCap()
{
float cap = g_GlobalVar.m_Mpu9250.m_CapMagnetique ;

// init tableau en courbe douce
if ( m_Size == 0 )
    {
    m_Size = g_GlobalVar.m_Config.m_droit_sec * 2 ; // x2 car lecture a 2 hz dans CGps::TacheGpsTempsVol()
    m_pCapArr = new float [m_Size] ;
    for ( long i = 0 ; i < m_Size ; i++ )
        {
        m_pCapArr[i] = cap ;
        cap += g_GlobalVar.m_Config.m_droit_deg/2. ;
        }
    }

// empilement dans le tableau
for( int ip = m_Size-1 ; ip > 0 ; ip-- )
    m_pCapArr[ip] = m_pCapArr[ip-1] ;
m_pCapArr[0] = cap ;
}
