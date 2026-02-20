///////////////////////////////////////////////////////////////////////////////// \file   CLGTObjets_inl.h
/// \date   28/05/2001 : date de creation
///
/// \brief Ce fichier contient la classe generique d'un tableau d'objets.
///
/// \date 18/02/2016 : CLGTObjets() m_nGrowBy = 50.
/// \date 23/02/2016 : Verification retour de new.
/// \date 20/11/2018 : Optimization CLGTObjets::RemoveAt().
/// \date 25/12/2019 : Optimisation CLGTObjets::SetSize().
/// \date 20/08/2021 : Optimisation CLGTObjets::SetGrow().
/// \date 14/06/2025 : On enleve les NVIDIA_FUNC.
/// \date 14/06/2025 : Derniere modification.
///

//////////////////////////////////////////////////////////////////////
/// \brief Constructeur m_nGrowBy = 20 par defaut.
template <class T>
inline CLGTObjets< T >::CLGTObjets()
{
m_pData = NULL;
m_nSize = m_nMaxSize = 0;
m_nGrowBy = 20 ;
}

//////////////////////////////////////////////////////////////////////
/// \brief destructeur
template <class T>
inline CLGTObjets< T >::~CLGTObjets()
{
delete [] m_pData;
}

//////////////////////////////////////////////////////////////////////
/// \brief Fait un decalege circulaire aleatoire du tableau
template <class T>
void CLGTObjets< T >::ShuffleCircular()
{
srand(time(NULL));
int Decalage = std::rand()%m_nSize ;

T * TmpData = new T [m_nSize] ;
T*  pCircular = m_pData + Decalage ;

// decalage circulaire
for( int i = 0 ; i < m_nSize ; i++ )
    {
    TmpData[i] = *pCircular ;
    pCircular++ ;
    if ( pCircular >= m_pData + m_nSize )
        pCircular = m_pData ;
    }

// recopie du tableau
for( int i = 0 ; i < m_nSize ; i++ )
    *(m_pData+i) = *(TmpData+i) ;

delete [] TmpData ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Reorganise le tableau de façon aleatoire.
/// \param Deterministe [in] : si true fonction de la taille du tableau
template <class T>
void CLGTObjets< T >::Shuffle(bool Deterministe)
{
int n=GetSize() ;
T temp ;

srand(time(NULL));

// pour tout le tableau
for ( int i=0 ; i<n ; i++)
    {
    // j est entre i et n
    int j ;
    if ( Deterministe )
        {
        j = i + ((int)(*this)[i]*1096) % (n-i) ;
        }
    else
        j = i + std::rand() % (n-i) ;
    // swap
    temp = (*this)[i] ;
    (*this)[i] = (*this)[j];
    (*this)[j] = temp;
    }
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoi true si tous les elemenets sont egaux.
template <class T>
bool CLGTObjets<T>::IsAllEqualLong() const
{
static_assert( std::is_same<int,T>(),"necessite int" );

// taille nulle
if ( GetSize() == 0 )
    return false ;

// taille de 1
if ( GetSize() == 1 )
    return true ;

// test de l'egalite partout
int i = 1 ;
while( i<GetSize() )
    {
    if ( (*this)[i-1] != (*this)[i] )
        break ;
    i++ ;
    }

// si tableau egal partout
if ( i == GetSize() )
    return true ;

return false ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoi true si tous les elemenets sont egaux.
template <class T>
bool CLGTObjets<T>::IsAllEqualDouble() const
{
//static_assert( sizeof(T) == sizeof(float) );
//static_assert( std::is_same<float,T>(),"necessite float" );

// taille nulle
if ( GetSize() == 0 )
    return false ;

// taille de 1
if ( GetSize() == 1 )
    return true ;

// test de l'egalite partout
int i = 1 ;
while( i<GetSize() )
    {
    double A = (double)(*this)[i-1] ;
    double B = (double)(*this)[i] ;
    double diff = fabs(A-B) ;
    if ( diff > 0. )
        break ;
    i++ ;
    }

// si tableau egal partout
if ( i == GetSize() )
    return true ;

return false ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoi l'indexe de l'element maximum. Le parametre MaxVal
/// renvoie la valeur a cet index. Si le tableau est egal partout, renvoi
/// l'indexe -1.
template <class T>
int CLGTObjets<T>::GetMaxIndex(T& MaxVal) const
{
// taille nulle
if ( GetSize() == 0 )
    {
    MaxVal = 0. ;
    return -1 ;
    }

// taille de 1
if ( GetSize() == 1 )
    {
    MaxVal = (*this)[0] ;
    return 0 ;
    }

/*
// test de l'egalite partout, cast en float
if ( IsAllEqualDouble() )
    {
    MaxVal = (*this)[0] ;
    return -1 ;
    }
*/

// test de la valeur max
MaxVal = (*this)[0] - 1. ;
int iMax = -1 ;
for( int i=0;i<GetSize();i++)
    {
    if ( (*this)[i] > MaxVal )
        {
        MaxVal = (*this)[i] ;
        iMax   = i ;
        }
    }

/*
// test si 2 maximums egaux, cast en float
for ( int i=0 ; i<GetSize() ; i++ )
    {
    if ( i == iMax )
        continue ;

    float A = (float)(*this)[i] ;
    float B = (float)(*this)[iMax] ;
    float diff = fabs(A-B) ;
    if ( diff <= 0. )
        return -1 ;
    }
*/
return iMax ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoi l'indexe de l'element minimum. Le parametre MinVal
/// renvoie la valeur a cet index. Si le tableau est egal partout, renvoi
/// l'indexe -1.
template <class T>
int CLGTObjets<T>::GetMinIndex(T& MinVal) const
{
// taille nulle
if ( GetSize() == 0 )
    {
    MinVal = 0. ;
    return -1 ;
    }

// taille de 1
if ( GetSize() == 1 )
    {
    MinVal = (*this)[0] ;
    return 0 ;
    }

/*// test de l'egalite partout, cast en float
if ( IsAllEqualDouble() )
    {
    MinVal = (*this)[0] ;
    return -1 ;
    }*/

// test de la valeur min
MinVal = (*this)[0] + 1.;
int iMin = -1 ;
for( int i=0 ; i<GetSize() ; i++ )
    {
    if ( (*this)[i] < MinVal )
        {
        MinVal = (*this)[i] ;
        iMin   = i ;
        }
    }

/*
// test si 2 minimums egaux, cast en float
for ( int i=0 ; i<GetSize() ; i++ )
    {
    if ( i == iMin )
        continue ;

    float A = (float)(*this)[i] ;
    float B = (float)(*this)[iMin] ;
    float diff = fabs(A-B) ;
    if ( diff <= 0. )
        return -1 ;
    }
*/

return iMin ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Recherche si l'element passe en parametre est dans le tableau
/// \param Element [in] : element recherche.
/// \return true ou false
template <class T>
bool CLGTObjets< T >::IsIn( const T & Element ) const
{
for(int ie=0 ; ie < GetSize(); ie++ )
    {
    if ( Element == GetAt(ie) )
        return true ;
    }
return false ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Definit la taille du tableau et sa croissance.
/// \param nNewSize |in] : nouvelle taille.
/// \param nGrowBy  [in] : pas de la croissance.
template <class T>
void CLGTObjets< T >::SetSize(int nNewSize, int nGrowBy /* = -1 */)
{
ASSERT(nNewSize >= 0);

if (nGrowBy != -1)
    SetGrow(nGrowBy);    // set new size

if (nNewSize == 0)
    {
    // shrink to nothing
    //delete [] m_pData;
    //m_pData = NULL;
    //m_nSize = m_nMaxSize = 0;
    m_nSize = 0 ;
    }
else if (m_pData == NULL)
    {
    //m_pData = new T [nNewSize+m_nGrowBy];
    m_pData = new T [nNewSize];

    if (m_pData==NULL)
        printf("Erreur allocation CLGTObjets\n") ;

    //m_nSize = m_nMaxSize = nNewSize;
    m_nSize    = nNewSize ;
    //m_nMaxSize = nNewSize+m_nGrowBy ;
    m_nMaxSize = nNewSize ;
    }
else if (nNewSize <= m_nMaxSize)
    {
    m_nSize = nNewSize;
    }
else
    {
    // Otherwise grow array
    int nNewMax;
    if (nNewSize < m_nMaxSize + m_nGrowBy)
        nNewMax = m_nMaxSize + m_nGrowBy;   // granularity
    else
        nNewMax = nNewSize; // no slush

    T* pNewData = new T[nNewMax];

    if (pNewData==NULL)
        printf("Erreur allocation CLGTObjets\n") ;

    // copy new data from old
    for ( int i=0 ; i < m_nSize ; i++ )
        pNewData[i] = m_pData[i] ;

    // construct remaining elements
    ASSERT(nNewSize > m_nSize);

    // get rid of old stuff (note: no destructors called)
    delete [] m_pData;
    m_pData = pNewData;
    m_nSize = nNewSize;
    m_nMaxSize = nNewMax;
    }
}


//////////////////////////////////////////////////////////////////////
/// \brief Enleve le surplus de taille et realloue/recopie les elements.
template <class T>
void CLGTObjets< T >::FreeExtra()
{
if (m_nSize != m_nMaxSize)
    {
    if ( m_nSize > 0 )
        {
        // shrink to desired size
        T* pNewData = new T[m_nSize];

        if (pNewData==NULL)
            printf("Erreur allocation CLGTObjets\n") ;

        // copy new data from old
        for ( int i=0 ; i < m_nSize ; i++ )
            pNewData[i] = m_pData[i] ;

        // get rid of old stuff (note: no destructors called)
        delete [] m_pData;
        m_pData = pNewData;
        m_nMaxSize = m_nSize;
        }
    else
        {
        delete [] m_pData;
        m_pData = NULL ;
        m_nMaxSize = m_nSize = 0 ;
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// \brief Insere un element a l'index desire, et fait grandir le tableau
/// si necessaire.
/// \param nIndex     [in] : index de l'element
/// \param newElement [in] : element a inserer
template <class T>
void CLGTObjets< T >::SetAtGrow(int nIndex, const T & newElement)
{
ASSERT(nIndex >= 0);

#ifdef _LG_DEBUG_
 if ( GetSize() / 20 > m_nGrowBy )
    cout << "********** size : " << GetSize() << ", grow : " << m_nGrowBy << " " << __FILE__ << " " << __LINE__ << endl ;
#endif

if (nIndex >= m_nSize)
    SetSize(nIndex+1);
m_pData[nIndex] = newElement;
}

//////////////////////////////////////////////////////////////////////
/// \brief Insere un n fois un element et fais grandir le tableau si
/// necessaire.
/// \param nIndex     [in] : index de l'element (GetSize() pour la fin du tableau)
/// \param newElement [in] : element a inserer
/// \param nCount     [in] : nombre de fois a inserer
///
template <class T>
void CLGTObjets< T >::InsertAt(int nIndex, const T & newElement, int nCount /*=1*/)
{
ASSERT(nIndex >= 0);    // will expand to meet need
ASSERT(nCount > 0);     // zero or negative size not allowed

if (nIndex >= m_nSize)
    {
    // adding after the end of the array
    SetSize(nIndex + nCount);       // grow so nIndex is valid
    }
else
    {
    // inserting in the middle of the array
    int nOldSize = m_nSize;
    SetSize(m_nSize + nCount); // grow it to new size

    // shift old data up to fill gap
    for ( int i = (nOldSize-nIndex-1) ; i >=0  ; i-- )
        m_pData[nIndex+nCount+i]=m_pData[nIndex+i] ;
    }

ASSERT(nIndex + nCount <= m_nSize);

// insert new value in the gap
while (nCount--)
    m_pData[nIndex++] = newElement;
}

//////////////////////////////////////////////////////////////////////
/// \brief Enleve nCount elements depuis nIndex. Mais pas de FreeExtra().
template <class T>
void CLGTObjets< T >::RemoveAt(int nIndex, int nCount /* = 1 */)
{
ASSERT(nIndex >= 0);
ASSERT(nCount >= 0);
ASSERT(nIndex + nCount <= m_nSize);

if ( nCount == 0 || m_nSize == 0 )
    return ;

// just remove a range
T* pDest = & m_pData[nIndex] ;
T* pSrc  = & m_pData[nIndex+nCount] ;
T* pFin  = & m_pData[m_nSize] ;
while ( pSrc < pFin )
    {
    *pDest = *pSrc ;
    pDest++ ;
    pSrc++ ;
    }
/*
int nMoveCount = m_nSize - (nIndex + nCount);
if (nMoveCount)
    {
    int i ;
    for ( i=0 ; i < nMoveCount ; i++ )
        m_pData[nIndex+i] = m_pData[nIndex + nCount + i] ;
    }*/

m_nSize -= nCount;
}

//////////////////////////////////////////////////////////////////////
/// \brief Inser un tableau NewArray a partir de nStartIndex.
/// Prendre nStartIndex=GetSize() pour un ajout à la fin.
template <class T>
void CLGTObjets< T >::InsertAt(int nStartIndex, const CLGTObjets & NewArray)
{
ASSERT(nStartIndex >= 0);

if (NewArray.GetSize() > 0)
    {
    InsertAt(nStartIndex, NewArray[0] , NewArray.GetSize());
    for (int i = 0; i < NewArray.GetSize(); i++)
        SetAt(nStartIndex + i, NewArray[i] );
    }
}

//////////////////////////////////////////////////////////////////////
/// \brief Positionne la taille de croissance.
/// \param nGrowBy [in] : taille de croissance.
template <class T>
inline void CLGTObjets< T >::SetGrow(int nGrowBy)
{
ASSERT( nGrowBy >= 0 ) ;
ASSERT( nGrowBy < 2000000000 ) ;
m_nGrowBy = nGrowBy ;
//FreeExtra() ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Ecrit su std::cout
template <class T>
void CLGTObjets< T >::Print() const
{
for( int i = 0 ; i < this->GetSize() ; i++ )
    cout << fixed << setprecision(2) << (*this)[i] << " " ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Encode le 1 de Class dans un tableau de longueur NumClass.
/// \param Class == -1 => tout a zero.
template <class T>
void CLGTObjets< T >::EncodeOneHot(int Class,int NumClass)
{
ASSERT( Class < NumClass ) ;
ASSERT( Class >= -1 ) ;
this->SetSize(NumClass) ;
for( int i = 0 ; i < this->GetSize() ; i++ )
    if ( i == Class )
        (*this)[i] = 1 ;
    else
        (*this)[i] = 0 ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoi l'indice de la valeur maximale.
/// \return -1 si egal partout.
template <class T>
int CLGTObjets< T >::DecodeOneHot() const
{
T MaxVal ;
return GetMaxIndex(MaxVal) ;
}

