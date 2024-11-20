////////////////////////////////////////////////////////////////////////////////
/// \file CFinSol.cpp
///
/// \brief
///
/// \date creation     : 07/03/2024
/// \date modification : 20/11/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Init des tableau dist alti
void CFinSol::InitDistAltiArr( float Dist , float Alti )
{
if ( m_pDistArr == NULL || m_pAltiArr == NULL )
    {
    m_pAltiArr = new float[m_Size] ;
    m_pDistArr = new float[m_Size] ;

    for ( long i = 0 ; i < m_Size ; i++ )
        {
        m_pAltiArr[i] = Alti ;
        m_pDistArr[i] = Dist ;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul la finesse sol
void CFinSol::PushDistAlti( float Dist , float Alti )
{
InitDistAltiArr( Dist , Alti ) ;

/*static float AltiS = Alti ;
Dist = 4 ;
AltiS-=1 ;
Alti = AltiS ;*/

// decalage dans la pile
float DistanceTotale = 0 ;
for ( int i = (m_Size - 1)  ; i > 0  ; i-- )
    {
    m_pAltiArr[i] = m_pAltiArr[i-1] ;
    m_pDistArr[i] = m_pDistArr[i-1] ;
    DistanceTotale += m_pDistArr[i] ;
    }

// empilement dernierre valeur
m_pAltiArr[0] = Alti ;
m_pDistArr[0] = Dist;

// calcul finesse sol
float Finesse = m_FinesseSol ;
float DifAlti = m_pAltiArr[m_Size-1] - m_pAltiArr[0] ;
if ( DifAlti != 0. )
    Finesse = ((float)DistanceTotale) / DifAlti ;

m_FinesseSol = m_FinesseSol * m_CoefFiltrage + (1.-m_CoefFiltrage) * Finesse ;
//Serial.println( m_FinesseSol ) ;
}
