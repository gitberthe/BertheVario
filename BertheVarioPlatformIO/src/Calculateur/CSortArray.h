////////////////////////////////////////////////////////////////////////////////
/// \file CSortArray.h
///
/// \brief
///
/// \date creation     : 17/03/2024
/// \date modification : 17/03/2024
///

#ifndef _SORTARRAY_
#define _SORTARRAY_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe template de tri par Schell Metzner de pointeur.
template < class T >
class CSortArray
{
public :
    void TriSchellMetzner( bool TriCroissant = true ) ;

protected :
    T ** m_pArr = NULL ;
    int m_Size = 0 ;
} ;

//////////////////////////////////////////////////////////////////////
/// \brief Permet de trier les elements d'un tableau par la methode
/// Schell Metzner.
/// \param TriCroissant [in] : tri croissant si == true (par defaut)
template < class T >
inline void CSortArray< T >::TriSchellMetzner( bool TriCroissant /*= true*/)
{
int gap,i,j;
int n=m_Size ;
T * temp ;
T** Array = CSortArray<T>::m_pArr ;

if ( Array == NULL || m_Size == 0 )
    return ;

if ( TriCroissant )
    {
    for( gap = n/2 ; gap > 0 ; gap /= 2 )
        {
        for( i = gap ; i < n ; ++i )
            {
            for ( j = i-gap ;j >= 0 &&  *Array[j] > *Array[j+gap] ; j -= gap )
                {
                temp = Array[j] ;
                Array[j] = Array[j+gap] ;
                Array[j+gap] = temp ;
                }
            }
        }
    }
else
    {
    for( gap = n/2 ; gap > 0 ; gap /= 2 )
        {
        for( i = gap ; i < n ; ++i )
            {
            for ( j = i-gap ;j >= 0 &&  *Array[j] < *Array[j+gap] ; j -= gap )
                {
                temp = Array[j] ;
                Array[j] = Array[j+gap] ;
                Array[j+gap] = temp ;
                }
            }
        }
    }
}

#endif
