//////////////////////////////////////////////////////////////////////////////
/// \file  CLGArray_inl.h
/// \date  23/03/2002 : date de creation
///
/// \brief Fichier inclu dans MyArray.h
///
/// \date 30/12/2002 : Passage de fonctions en const.
/// \date 16/10/2004 : Correction d'un bug dans CopyWithoutAlloc.
/// \date 22/12/2004 : Tri et recherche par dichotomie.
/// \date 24/09/2005 : Memorisation du dernier tri.
/// \date 09/03/2017 : Implementation de dichotomie pour les float.
/// \date 19/10/2020 : NVIDIA_FUNC
/// \date 28/10/2020 : NVIDIA_FUNC
/// \date 25/06/2025 : Dans TriAdapte, remise en place du TriSchell.
/// \date 25/06/2025 : Derniere modification.
///

//////////////////////////////////////////////////////////////////////
/// \brief Constructeur, initialise la taille à 0.
template < class T >
inline CLGArray< T >::CLGArray()
{
CLGTObjets<T>::SetSize( 0 ) ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Constructeur, initialise la taille à 0.
template < class T >
inline CLGArray< T >::CLGArray( int Size , int GrowBy )
{
CLGTObjets<T>::SetSize( Size , GrowBy ) ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Operateur de duplication, utilise l'operateur de recopie.
/// \param Array [in] : tableau à dupliquer
template < class T >
inline CLGArray< T >::CLGArray( const CLGArray& Array )
{
(*this)= Array ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Operateur de recopie. Recopie la taille, puis toutes les données
/// une à une.
/// \param Array [in] : tableau à recopier
template < class T >
inline bool CLGArray< T >::operator == ( const CLGArray& Array ) const
{
if ( this == &Array )
    return true ;

if ( CLGArray< T >::GetSize() != Array.GetSize() )
    return false ;

for ( int i = 0 ; i < Array.GetSize() ; i++ )
    if ( (*this)[i] != Array[i] )
        return false ;

return true ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Operateur de recopie. Recopie la taille, puis toutes les données
/// une à une.
/// \param Array [in] : tableau à recopier
template < class T >
inline const CLGArray< T > & CLGArray< T >::operator = ( const CLGArray& Array )
{
if ( this == &Array )
    return *this ;

CLGTObjets<T>::SetSize( Array.GetSize() ) ;

T * pData = CLGTObjets<T>::m_pData ;
const T * pDataSrc = Array.m_pData ;
for( int i = 0 ; i < CLGTObjets<T>::GetSize() ; i ++ )
    (*pData++) = (*pDataSrc++) ;
    //(*pData++) = Array[i] ;

    // optimisation du 16 10 2004
    //(*this)[i] = Array[i] ;

return *this ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Permet le trie des elements d'un tableau par la methode du
/// tri a bulle.
/// \param TriCroissant [in] : tri croissant si =1 (par defaut)
template < class T >
inline void CLGArray< T >::TriBulle( bool TriCroissant /*= true*/)
{
T Temp ;
if ( TriCroissant )
    {
    for ( int i = CLGTObjets<T>::GetUpperBound()-1 ; i >=0 ; )
        {                                 // si superieur
        if ( (*this)[i] > (*this)[i+1] )
            {
            Temp = (*this)[i+1] ;       // on interverti les class T
            (*this)[i+1] = (*this)[i] ;
            (*this)[i] = Temp ;
            if ( (i+1) < CLGTObjets<T>::GetUpperBound() )
                i++ ;
            }
        else
            i-- ;
        }
    }
else
    {
    for ( int i = CLGTObjets<T>::GetUpperBound()-1 ; i >=0 ; )
        {                                 // si superieur
        if ( (*this)[i] < (*this)[i+1] )
            {
            Temp = (*this)[i+1] ;       // on interverti les class T
            (*this)[i+1] = (*this)[i] ;
            (*this)[i] = Temp ;
            if ( (i+1) < CLGTObjets<T>::GetUpperBound() )
                i++ ;
            }
        else
            i-- ;
        }
    }
}

#ifndef __CUDA_ARCH__

/*
//////////////////////////////////////////////////////////////////////
/// \brief Tri fusion de tableau.
template < class T >
inline void CLGArray< T >::TriQuickSort( bool TriCroissant )
{
CLGQuickSortArr<T>::quickSort( CLGTObjets<T>::m_pData , 0, CLGTObjets<T>::m_nSize -1 , TriCroissant ) ;
}
*/

#endif

//////////////////////////////////////////////////////////////////////
/// \brief Permet de trier les elements d'un tableau par la methode
/// Schell Metzner.
/// \param TriCroissant [in] : tri croissant si =1 (par defaut)
template < class T >
inline void CLGArray< T >::TriSchellMetzner( bool TriCroissant /*= true*/)
{
int gap,i,j;
int n=CLGTObjets<T>::GetSize() ;
T temp ;
if ( TriCroissant )
    {
    for( gap = n/2 ; gap > 0 ; gap /= 2 )
        {
        for( i = gap ; i < n ; ++i )
            {
            for ( j = i-gap ;j >= 0 && (*this)[j] > (*this)[j+gap] ; j -= gap )
                {
                temp = (*this)[j] ;
                (*this)[j] = (*this)[j+gap] ;
                (*this)[j+gap] = temp ;
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
            for ( j = i-gap ;j >= 0 && (*this)[j] < (*this)[j+gap] ; j -= gap )
                {
                temp = (*this)[j] ;
                (*this)[j] = (*this)[j+gap] ;
                (*this)[j+gap] = temp ;
                }
            }
        }

    }
}

//////////////////////////////////////////////////////////////////////
/// \brief Tri en quicksort/fusion suivant la taille.
template < class T >
void CLGArray< T >::TriAdapte( bool TriCroissant /*= true*/ )
{
//if ( CLGArray::GetSize() < LIMITE_TRI_EN_THREAD )
    TriSchellMetzner( TriCroissant ) ;
//else
//    TriQuickSortThread(TriCroissant) ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Initialise un tableau du début à la fin avec une valeur.
///
/// \param ACopierPartout [in] : valeur à recopier.
template < class T >
inline void CLGArray< T >::SetAll( const T & ACopierPartout )
{
int i ;
for ( i = 0 ; i < CLGTObjets<T>::GetSize() ; i++ )
    {
    CLGTObjets<T>::ElementAt(i) = ACopierPartout ;
    }
}

//////////////////////////////////////////////////////////////////////
/// \brief Empile la class T par le bas et decale tous les autres
/// objets vers le haut.
/// \param AEmpilerParleBas [in] : reference de l'objet à empiler
template < class T >
inline void CLGArray< T >::Push( const T & AEmpilerParleBas )
{

/*for ( int i = 0 ; i < CLGTObjets<T>::GetUpperBound() ; i++ )
    {
    // decalage vers le haut des objets
    (*this)[i] = (*this)[i+1] ;
    }*/
memmove( CLGTObjets<T>::m_pData , CLGTObjets<T>::m_pData+1 , CLGTObjets<T>::GetUpperBound() * sizeof(T) ) ;

(*this)[CLGTObjets<T>::GetUpperBound()] = AEmpilerParleBas ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Recherche un element dans le tableau.
/// \param element [in] : reference de l'objet à trouver
/// \return l'indice de l'element dans le tableau
/// \return -1 si il n'a pas été trouvé
template <class T>
inline int CLGArray< T >::Find( const T & element ) const
{
int i ;
for ( i = 0 ; i < CLGTObjets<T>::GetSize() ; i ++ )
    {
    if ( (*this)[i] == element )
        return i ;
    }
return -1 ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Recherche un element dans le tableau precedement trie par dichotomie.
/// \param element [in] : reference de l'objet à trouver
/// \return l'indice de l'element dans le tableau
/// \return -1 si il n'a pas été trouvé
template <class T>
inline int CLGArray< T >::FindWithDichotomy( const T & DichotomyToFind , CLGArray<int> & IndexArray ) const
{
IndexArray.SetSize(0);

if ( CLGTObjets<T>::GetSize() == 0 )
    return -1 ;

int retour = -1 ;
int iMax=CLGTObjets<T>::GetUpperBound() ;
int iMin=0 ;
int icur ;

// tant que l'ecart est different de zero
do
    {
    // calcul position mediane dans le tableau
    icur = (iMax-iMin)/2 + iMin ;

    // si on doit rechercher vers le haut
    if ( (*this)[icur] < DichotomyToFind )
        iMin = ++icur ;
    // si on doit rechercher vers le bas
    else if ( (*this)[icur] > DichotomyToFind )
        iMax = --icur ;
    // egalite avec l'index recherche
    else
        {
        ASSERT( (*this)[icur] == DichotomyToFind ) ;

        // on recherche la limite basse
        while( icur > 0 && (*this)[icur-1] == DichotomyToFind )
            icur-- ;

        // memorisation limite basse
        retour = icur ;
        ASSERT( (*this)[icur] == DichotomyToFind ) ;

        // on ajoute les objets correspondant au critere
        while( icur < CLGTObjets<T>::GetSize() && (*this)[icur] == DichotomyToFind )
            IndexArray.Add( icur++ ) ;

        return retour ;
        }
    }
while ( iMax >= iMin  ) ;

return -1 ;

}

//////////////////////////////////////////////////////////////////////////////
/// \brief Insert un tableau à la fin.
template <class T>
inline void CLGArray< T >::InsertAtEnd( const CLGArray<T> & NewArray )
{
if ( NewArray.GetSize() > 0 )
    {
    int StartIndex = CLGTObjets<T>::GetSize() ;
    CLGTObjets<T>::SetSize( CLGTObjets<T>::GetSize() + NewArray.GetSize() ) ;
    for ( int i = 0 ; i < NewArray.GetSize() ; i++ )
        *(CLGTObjets<T>::m_pData + StartIndex + i) = *(NewArray.CLGTObjets<T>::m_pData + i) ;
    }
}

/****************************************************************************/

//////////////////////////////////////////////////////////////////////
// Implementation des fonctions de la class CLGArrayPtr

//////////////////////////////////////////////////////////////////////
/// \brief Constructeur, initialise le m_DernierTri et le m_DeleteObject.
/// \param m_DeleteObject [in] : les objets sont à detruire par defaut
template < class T>
inline CLGArrayPtr< T >::CLGArrayPtr( bool DeleteObject /* = false */)
{
m_DernierTri   = AUCUN_TRI ;
m_DeleteObject = DeleteObject ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Operateur de duplication, utilise l'operateur de recopie.
/// \param Array [in] : tableau de pointeur à dupliquer
template < class T>
inline CLGArrayPtr< T >::CLGArrayPtr ( const CLGArrayPtr& Array )
{
//m_DeleteObject = true ;
(*this) = Array ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Operateur de recopie. Recopie un tableau de pointeur en faisant
/// toujours une creation d'objet.
/// \param Array [in] : tableau de pointeur à recopier
template < class T>
inline const CLGArrayPtr< T > & CLGArrayPtr< T >::operator = ( const CLGArrayPtr& Array )
{
if ( this == &Array )
    return *this ;

// destruction des objets
DeleteAll() ;
m_DeleteObject = true ;

// redimentionnement du tableau
SetSize( Array.GetSize() ) ;

// recopie des pointeur d'objets, objets qui sont dupliques
T * * pData = CLGArrayPtr< T >::m_pData ;
T * * pDataSrc = Array.m_pData ;
for( int i = 0 ; i < GetSize() ; i ++ )
    {
    // creation et recopie des objets
    //(*pData++) = new T( * Array[i] )  ;
    (*pData++) = new T( *(*pDataSrc++) )  ;
    }

// recopie du dernier type de tri
m_DernierTri = Array.m_DernierTri ;

return *this ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Recopie un tableau de pointeur en ne faisant jamais une
/// creation d'objet.
/// \param Array [in] : tableau de pointeur à dupliquer sans creation.
template < class T >
const CLGArrayPtr< T > & CLGArrayPtr< T >::CopyWithoutAlloc( const CLGArrayPtr& Array , bool DeleteObject )
{
if ( this == &Array )
    return *this ;

// destruction des objets
DeleteAll() ;
m_DeleteObject = DeleteObject ;

SetSize( Array.GetSize() ) ;

// si la taille est superieure a 0
// bug accet [0] si taille 0 du 16 10 2004
if ( GetSize() )
    memcpy( CLGArrayPtr<T>::m_pData , Array.m_pData , Array.GetSize() * sizeof( T * ) ) ;

// recopie du dernier type de tri
m_DernierTri = Array.m_DernierTri ;

return *this ;
}

//////////////////////////////////////////////////////////////////////
/// \brief fonction de melange aleatoite ou deterministe
template < class T>
inline void CLGArrayPtr< T >::Shuffle( bool Deterministe )
{
int n=GetSize() ;
T * temp ;

srand(time(NULL));

// pour tout le tableau
for ( int i=0 ; i<n ; i++)
    {
    // j est entre i et n
    int j ;
    if ( Deterministe )
        {
        float * p = (float*)(*this)[i] ;
        float Nb = fmodf( *p , 0.0125 ) ;
        j = i +  ((int)(Nb*101096.)) % (n-i) ;
        }
    else
        j = i + std::rand() % (n-i) ;
    // swap
    temp = (*this)[i] ;
    (*this)[i] = (*this)[j];
    (*this)[j] = temp;
    }

// memorisation du dernier type de tri
m_DernierTri = AUCUN_TRI ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Permet le trie des elements d'un tableau par la methode du
/// tri a bulle. Ne teste pas s'il y a deja eu un tri.
/// \param TriCroissant [in] : tri croissant si =1 (par defaut)
template < class T>
void CLGArrayPtr< T >::TriBulle( bool TriCroissant  /*= true*/ )
{
T* Temp ;
if ( TriCroissant )
    {
    for ( int i = GetUpperBound()-1 ; i >= 0 ; )
        {                                  // si superieur
        if ( *(*this)[i] > *(*this)[i+1] )
            {
            Temp = (*this)[i+1] ;       // on interverti les pointeurs des class T
            (*this)[i+1] = (*this)[i] ;
            (*this)[i] = Temp ;
            if ( (i+1) < GetUpperBound() )
                i++ ;
            }
        else
            i-- ;
        }
    }
else
    {
    for ( int i = GetUpperBound()-1 ; i >= 0 ; )
        {                                  // si superieur
        if ( *(*this)[i] < *(*this)[i+1] )
            {
            Temp = (*this)[i+1] ;       // on interverti les pointeurs des class T
            (*this)[i+1] = (*this)[i] ;
            (*this)[i] = Temp ;
            if ( (i+1) < GetUpperBound() )
                i++ ;
            }
        else
            i-- ;
        }
    }

// memorisation du dernier type de tri
m_DernierTri = (TriCroissant) ? TRI_CROISSANT : TRI_DECROISSANT ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Tri en quicksort/fusion suivant la taille.
template < class T >
void CLGArrayPtr< T >::TriAdapte( bool TriCroissant /*= true*/ )
{
//if ( CLGArrayPtr::GetSize() < LIMITE_TRI_EN_THREAD )
    TriSchellMetzner( TriCroissant ) ;
//else
//    TriQuickSortThread(TriCroissant) ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Permet de trier les elements d'un tableau par la methode
/// Schell Metzner. Ne teste pas s'il y a deja eu un tri.
/// \param TriCroissant [in] : tri croissant si =1 (par defaut)
template < class T >
inline void CLGArrayPtr< T >::TriSchellMetzner( bool TriCroissant /*= true*/)
{
int gap,i,j;
int n=GetSize() ;
T * temp ;
//T** Array = CLGTObjets<T>::m_pData ;
T** Array = this->m_pData ;

if ( TriCroissant )
    {
    for( gap = n/2 ; gap > 0 ; gap /= 2 )
        {
        for( i = gap ; i < n ; ++i )
            {
            for ( j = i-gap ;j >= 0 &&  *Array[j] > *Array[j+gap] ; j -= gap )
                {
                temp = (*this)[j] ;
                (*this)[j] = (*this)[j+gap] ;
                (*this)[j+gap] = temp ;
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
                temp = (*this)[j] ;
                (*this)[j] = (*this)[j+gap] ;
                (*this)[j+gap] = temp ;
                }
            }
        }
    }

// memorisation du dernier type de tri
m_DernierTri = (TriCroissant) ? TRI_CROISSANT : TRI_DECROISSANT ;
}

/*
//////////////////////////////////////////////////////////////////////
/// \brief
template < class T >
inline void CLGArrayPtr< T >::TriQuickSortThreadForDichoString()
{
CLGQuickSortThreadArrp<T>::quickSortThreadForDichoString( CLGTObjets<T*>::m_pData , 0 , CLGTObjets<T*>::m_nSize -1 ) ;

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_STRING ;
}

//////////////////////////////////////////////////////////////////////
/// \brief
template < class T >
inline void CLGArrayPtr< T >::TriQuickSortThreadForDichoValue()
{
CLGQuickSortThreadArrp<T>::quickSortThreadForDichoValue( CLGTObjets<T*>::m_pData , 0 , CLGTObjets<T*>::m_nSize -1 ) ;

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_VALUE ;
}

//////////////////////////////////////////////////////////////////////
/// \brief
template < class T >
inline void CLGArrayPtr< T >::TriQuickSortThreadForDichoIndex()
{
CLGQuickSortThreadArrp<T>::quickSortThreadForDichoIndex( CLGTObjets<T*>::m_pData , 0 , CLGTObjets<T*>::m_nSize -1 ) ;

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_INDEX ;
}
*/
//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction recherche par dichotomie des elements d'un tableau
/// precedemment trie par la fonction TriSchellMetznerForDichotomyString.
///
/// \param  DichotomySrtingToFind : chaine dichotomique recherchee.
/// \param  ArrayPtrResult        : tableau de pointeurs sur les elements trouves.
///                                 GetDichotomyString() == DichotomyStringToFind
///
/// \return la position du premier index retourne
///
template < class T >
inline int CLGArrayPtr< T >::FindWithDichotomyString( const char * DichotomyStringToFind , CLGArray< const T * > & ArrayPtrResult ) const
{
ArrayPtrResult.SetSize(0);

if ( GetSize() == 0 )
    return -1 ;

ASSERT( GetLastSort() == TRI_DICHOTOMIE_STRING ) ;

int retour = -1 ;
int iMax=GetUpperBound() ;
int iMin=0 ;
int icur ;

// tant que l'ecart est different de zero
do
    {
    // calcul position mediane dans le tableau
    icur = (iMax-iMin)/2 + iMin ;

    // si on doit rechercher vers le haut
    if ( strcmp((*this)[icur]->GetDichotomyString() , DichotomyStringToFind ) < 0 )
        iMin = ++icur ;
    // si on doit rechercher vers le bas
    else if ( strcmp( (*this)[icur]->GetDichotomyString() , DichotomyStringToFind ) > 0)
        iMax = --icur ;
    // egalite avec l'index recherche
    else
        {
        ASSERT( strcmp( (*this)[icur]->GetDichotomyString() , DichotomyStringToFind ) == 0 )  ;

        // on recherche la limite basse
        while( icur > 0 && strcmp( (*this)[icur-1]->GetDichotomyString() , DichotomyStringToFind ) == 0 )
            icur-- ;

        // memorisation limite basse
        retour = icur ;
        ASSERT( strcmp( (*this)[icur]->GetDichotomyString() , DichotomyStringToFind ) == 0 ) ;

        // on ajoute les objets correspondant au critere
        while( icur < GetSize() && strcmp( (*this)[icur]->GetDichotomyString() , DichotomyStringToFind ) == 0)
            ArrayPtrResult.Add( (*this)[icur++] ) ;

        return retour ;
        }
    }
while ( iMax >= iMin  ) ;

return -1 ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction recherche par dichotomie l'endroi ou inserer
/// une valeur dans le tableau pour rester en tri dichotomique.
///
/// \param Value [in] : valeur dichotomique a inserer.
///
/// \return la position du premier index retourne
///
template < class T >
int CLGArrayPtr< T >::FindWithDichotomyStringIndexToInsert( const char * String ) const
{
if ( GetSize() == 0 )
    return 0 ;

ASSERT( GetLastSort() == TRI_DICHOTOMIE_STRING ) ;

int iMax=GetUpperBound() ;
int iMin=0 ;
int icur = (iMax-iMin)/2 + iMin ;

// tant que l'ecart est different de zero
do
    {
    // si on doit rechercher vers le haut
    if ( strcmp((*this)[icur]->GetDichotomyString() , String ) < 0 )
        iMin = ++icur ;
    // si on doit rechercher vers le bas
    else if ( strcmp((*this)[icur]->GetDichotomyString() , String ) > 0 )
        iMax = --icur ;
    // on est dans la valeur recherche +- DeltaValue
    else
        return icur ;
    icur = (iMax-iMin)/2 + iMin ;
    }
while ( iMax >= iMin  ) ;

return icur ;
}


//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction recherche par dichotomie des elements d'un tableau
/// precedemment trie par la fonction TriSchellMetznerForDichotomyIndex.
///
/// \param  DichotomyIndexToFind : index dichotomique recherche.
/// \param  ArrayPtrResult       : tableau de pointeurs sur les elements trouves.
///                                GetDichotomyIndex() == DichotomyIndexToFind
///
/// \return la position du premier index retourne. -1 si non trouve.
///
template < class T >
int CLGArrayPtr< T >::FindWithDichotomyIndex( int DichotomyIndexToFind , CLGArray< const T *> & ArrayPtrResult ) const
{
ArrayPtrResult.SetSize(0);

if ( GetSize() == 0 )
    return -1 ;

ASSERT( GetLastSort() == TRI_DICHOTOMIE_INDEX ) ;

int retour = -1 ;
int iMax=GetUpperBound() ;
int iMin=0 ;
int icur ;

// tant que l'ecart est different de zero
do
    {
    // calcul position mediane dans le tableau
    icur = (iMax-iMin)/2 + iMin ;

    // si on doit rechercher vers le haut
    if ( (*this)[icur]->GetDichotomyIndex() < DichotomyIndexToFind )
        iMin = ++icur ;
    // si on doit rechercher vers le bas
    else if ( (*this)[icur]->GetDichotomyIndex() > DichotomyIndexToFind )
        iMax = --icur ;
    // egalite avec l'index recherche
    else
        {
        ASSERT( (*this)[icur]->GetDichotomyIndex() == DichotomyIndexToFind ) ;

        // on recherche la limite basse
        while( icur > 0 && (*this)[icur-1]->GetDichotomyIndex() == DichotomyIndexToFind )
            icur-- ;

        // memorisation limite basse
        retour = icur ;
        ASSERT( (*this)[icur]->GetDichotomyIndex() == DichotomyIndexToFind ) ;

        // on ajoute les objets correspondant au critere
        while( icur < GetSize() && (*this)[icur]->GetDichotomyIndex() == DichotomyIndexToFind )
            {
            ArrayPtrResult.Add( (*this)[icur++] ) ;
            if ( ArrayPtrResult.GetSize() / 10 + 20 > ArrayPtrResult.GetGrow() )
                ArrayPtrResult.SetGrow( ArrayPtrResult.GetSize() / 10 + 20 ) ;
            }

        return retour ;
        }
    }
while ( iMax >= iMin  ) ;

return -1 ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction recherche par dichotomie l'endroi ou inserer
/// une valeur dans le tableau pour rester en tri dichotomique.
///
/// \param Value [in] : valeur dichotomique a inserer.
///
/// \return la position du premier index retourne
///
template < class T >
int CLGArrayPtr< T >::FindWithDichotomyIndexIndexToInsert( int Value ) const
{
if ( GetSize() == 0 )
    return 0 ;

ASSERT( GetLastSort() == TRI_DICHOTOMIE_INDEX ) ;

int iMax=GetUpperBound() ;
int iMin=0 ;
int icur = (iMax-iMin)/2 + iMin ;

// tant que l'ecart est different de zero
do
    {
    int CurVal = (*this)[icur]->GetDichotomyIndex() ;
    // si on doit rechercher vers le haut
    if ( CurVal < Value )
        iMin = ++icur ;
    // si on doit rechercher vers le bas
    else if ( CurVal > Value )
        iMax = --icur ;
    // on est dans la valeur recherche +- DeltaValue
    else
        return icur ;
    icur = (iMax-iMin)/2 + iMin ;
    }
while ( iMax >= iMin  ) ;

return icur ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction recherche par dichotomie des elements du tableau
/// precedemment trie par la fonction TriSchellMetznerForDichotomyValue.
///
/// \param ValMinExclude    [in] : valeur dichotomique recherche minimale exclue.
/// \param ValMaxInclude    [in] : valeur recherche maximale inclue.
/// \param ArrayPtrResult   [out]: tableau de pointeurs sur les elements trouves.
///
/// \return la position du premier index retourne
///
template < class T >
int CLGArrayPtr< T >::FindWithDichotomyValue( float ValMinExclude , float ValMaxInclude , CLGArray< const T *> & ArrayPtrResult ) const
{
ArrayPtrResult.SetSize(0);

if ( GetSize() == 0 )
    return -1 ;

ASSERT( ValMaxInclude >= ValMinExclude ) ;
ASSERT( GetLastSort() == TRI_DICHOTOMIE_VALUE ) ;

int retour = -1 ;
int iMax=GetUpperBound() ;
int iMin=0 ;
int icur ;

float ValMax = ValMaxInclude ;
float ValMin = ValMinExclude ;

// tant que l'ecart est different de zero
do
    {
    // calcul position mediane dans le tableau
    icur = (iMax-iMin)/2 + iMin ;

    float CurVal = (*this)[icur]->GetDichotomyValue() ;
    // si on doit rechercher vers le haut
    if ( CurVal < ValMin )
        iMin = ++icur ;
    // si on doit rechercher vers le bas
    else if ( CurVal > ValMax )
        iMax = --icur ;
    // on est dans la valeur recherche +- DeltaValue
    else
        {
        ASSERT( CurVal <= ValMax ) ;
        ASSERT( CurVal >= ValMin ) ;

        // on recherche la limite basse
        while( icur > 0 && (*this)[icur-1]->GetDichotomyValue() > ValMin )
            icur-- ;

        // memorisation limite basse
        retour = icur ;
        ASSERT( (*this)[icur]->GetDichotomyValue() >= ValMin ) ;

        // on ajoute les objets correspondant au critere
        while( icur < GetSize() && (*this)[icur]->GetDichotomyValue() <= ValMax )
            ArrayPtrResult.Add( (*this)[icur++] ) ;

        return retour ;
        }
    }
while ( iMax >= iMin  ) ;

return -1 ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction recherche par deplacement des elements du tableau
/// precedemment trie par la fonction TriSchellMetznerForDichotomyValue (tri decroisant).
///
/// \param ValMinExclude    [in] : valeur dichotomique recherche minimale exclue.
/// \param ValMaxInclude    [in] : valeur recherche maximale inclue.
/// \param ArrayPtrResult   [out]: tableau de pointeurs sur les elements trouves.
/// \param StartIndex       [in] : index precedant proche de ValMinExclude/ValMaxInclude.
///
/// \return la position du premier index retourne
///
template < class T >
int CLGArrayPtr< T >::FindAroundDichotomyValue( float ValMinExclude , float ValMaxInclude , CLGArray< const T * > & ArrayPtrResult , int StartIndex ) const
{
int iMax = StartIndex ;
int iMin = StartIndex ;

ASSERT( GetLastSort() == TRI_DICHOTOMIE_VALUE ) ;

ArrayPtrResult.SetSize(0) ;

float Val = (*this)[iMin]->GetDichotomyValue() ;

// au dessous des valeurs
if ( Val < ValMinExclude )
    {
    // on recherche la limite basse
    while( iMin < CLGArrayPtr< T >::GetUpperBound() && (*this)[iMin]->GetDichotomyValue() < ValMinExclude )
        iMin++ ;

    // on recherche la limite haute
    while( iMax < CLGArrayPtr< T >::GetUpperBound() && (*this)[iMax]->GetDichotomyValue() <= ValMaxInclude )
        iMax++ ;
    iMax-- ;
    }
// au dessus des valeurs
else if ( Val > ValMaxInclude )
    {
    // on recherche la limite basse
    while( iMin > 0 && (*this)[iMin]->GetDichotomyValue() >= ValMinExclude )
        iMin-- ;
    iMin++ ;

    // on recherche la limite haute
    while( iMax > 0 && (*this)[iMax]->GetDichotomyValue() > ValMaxInclude )
        iMax-- ;
    }
// entre les valeurs
else
    {
    // on recherche la limite basse
    while( iMin > 0 && (*this)[iMin]->GetDichotomyValue() >= ValMinExclude )
        iMin-- ;
    iMin++ ;

    // on recherche la limite haute
    while( iMax < CLGArrayPtr< T >::GetUpperBound() && (*this)[iMax]->GetDichotomyValue() <= ValMaxInclude )
        iMax++ ;
    iMax-- ;
    }

for ( int it = iMin ; it <= iMax ; it++ )
    ArrayPtrResult.Add( (*this)[it] ) ;

return (iMax+iMin)/2 ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Cette fonction recherche par dichotomie l'endroi ou inserer
/// une valeur dans le tableau pour rester en tri dichotomique.
///
/// \param Value [in] : valeur dichotomique a inserer.
///
/// \return la position du premier index retourne
///
template < class T >
int CLGArrayPtr< T >::FindWithDichotomyValueIndexToInsert( float Value ) const
{
if ( GetSize() == 0 )
    return 0 ;

ASSERT( GetLastSort() == TRI_DICHOTOMIE_VALUE ) ;

int iMax=GetUpperBound() ;
int iMin=0 ;
int icur = (iMax-iMin)/2 + iMin ;

// tant que l'ecart est different de zero
do
    {
    float CurVal = (*this)[icur]->GetDichotomyValue() ;
    // si on doit rechercher vers le haut
    if ( CurVal < Value )
        iMin = ++icur ;
    // si on doit rechercher vers le bas
    else if ( CurVal > Value )
        iMax = --icur ;
    // on est dans la valeur recherche +- DeltaValue
    else
        return icur ;
    icur = (iMax-iMin)/2 + iMin ;
    }
while ( iMax >= iMin  ) ;

return icur ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Detruit tout les objets du tableau si m_DeleteObject.
template < class T>
inline void CLGArrayPtr< T >::DeleteAll()
{
// si les objet sont a detruire
if ( m_DeleteObject && GetSize() > 0 )
    {
    T ** pData    = CLGArrayPtr<T>::m_pData ;
    T ** pDataMax = CLGArrayPtr<T>::m_pData + GetSize() ;
    while( pData < pDataMax )
        { delete (*pData++) ; }


    /*//
    int i ;
    for ( i = 0 ; i < GetSize() ; i++ )
        delete (T*)(*this)[i] ;*/
    }

SetSize(0);
//m_DeleteObject = true ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Remove all index in IndexToRemoveArr.
///
/// \param IndexToRemove [in] [out] : Array of index to remove.
///
template < class T>
inline void CLGArrayPtr< T >::RemoveThisIndex( CLGArray<int> & IndexToRemovArr )
{
if ( IndexToRemovArr.GetSize() == 0 )
    return ;

if ( GetSize() == 0 )
    return ;

// pour tous les indexe a supprimer
for ( int iindex=0 ; iindex<IndexToRemovArr.GetSize() ; iindex++ )
    {
    int iThis = IndexToRemovArr[iindex] ;
    ASSERT( iThis < GetSize() ) ;
    // si deja fait
    if ( GetAt(iThis) == NULL )
        continue ;
    // destruction
    if ( m_DeleteObject )
        delete ElementAt(iThis) ;
    // mise a null
    ElementAt(iThis) = NULL ;
    }
// compresion des null
CompressNull() ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Remove all ptr that are NULL.
///
/// \param IndexToRemove [in] [out] : Array of index to remove.
///
template < class T>
inline void CLGArrayPtr< T >::CompressNull()
{
if ( CLGArrayPtr< T >::m_nSize == 0 )
    return ;

// variables temporaires
T ** pTmpDataArr  = new T * [CLGArrayPtr< T >::m_nSize] ;
T ** pTmpCopyNull = pTmpDataArr ;
T ** pDataFrom = CLGArrayPtr< T >::m_pData ;
int newSize = 0 ;

// pour tous le tableau actuel
for ( int i=0 ; i<GetSize(); i++)
    {
    // si indexe a ne pas recopier
    //if ( (*this)[i] != NULL )
    if ( *pDataFrom != NULL )
        {
        //*pTmpCopyNull++ = (*this)[i] ;
        *(pTmpCopyNull++) = *pDataFrom ;
        newSize++ ;
        }
    pDataFrom++ ;
    }

// recopy de TmpDataArr vers m_pData
for ( int i=0 ; i<newSize ; i++)
    *(CLGTObjets<T*>::m_pData+i)=*(pTmpDataArr+i) ;

// deallocation de tmpData
delete [] pTmpDataArr ;

ASSERT( newSize <= CLGArrayPtr< T >::m_nSize ) ;
CLGArrayPtr< T >::m_nSize = newSize ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Recherche un pointeur d'element dans le tableau.
///
/// \param pelement [in] : pointeur sur l'objet à trouver.
///
/// \return : l'indice de l'element ou -1 si il n'y est pas.
///
template <class T>
inline int CLGArrayPtr< T >::Find( const T * pelement ) const
{
for ( int i = 0 ; i < GetSize() ; i ++ )
    {
    if ( (*this)[i] == pelement )
        return i ;
    }
return -1 ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Insert un tableau a la fin sans nouvelles allocations.
///
/// \param NewArray [in] : tableau a inserer.
///
template <class T>
inline void CLGArrayPtr< T >::InsertAtEnd( const CLGArrayPtr & NewArray )
{
m_DernierTri = AUCUN_TRI ;

if ( NewArray.GetSize() > 0)
    {
    int nStartIndex = GetSize() ;
    //InsertAt(nStartIndex, NewArray[0] , NewArray.GetSize());
    SetSize(GetSize()+NewArray.GetSize()) ;
    for (int i = 0; i < NewArray.GetSize(); i++)
        //SetAt(nStartIndex + i, NewArray[i] );
        *(CLGTObjets<T*>::m_pData + nStartIndex + i) = *(NewArray.CLGTObjets<T*>::m_pData + i) ;
    }
}

/*
//////////////////////////////////////////////////////////////////////
/// \brief Tri fusion de tableau.
template < class T >
inline void CLGArray< T >::TriFusion( bool TriCroissant )
{
CLGMSMergeSortThreadedArr<T>::SortMerge( CLGTObjets<T>::m_pData , CLGTObjets<T>::m_nSize , TriCroissant ) ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Tri en quicksort/fusion suivant la taille.
template < class T >
void CLGArrayPtr< T >::TriAdapteForDichotomyIndex()
{
#ifdef __CUDA_ARCH__
TriSchellMetznerForDichotomyIndex() ;
#else
if ( GetSize() < LIMITE_TRI_EN_THREAD )
    TriSchellMetznerForDichotomyIndex() ;
else
    TriFusionForDichotomyIndex() ;
#endif
}

//////////////////////////////////////////////////////////////////////
/// \brief Tri fusion de tableau.
template < class T >
inline void CLGArrayPtr< T >::TriFusion( bool TriCroissant )
{
CLGMSMergeSortThreadedArrp<T>::SortMerge( CLGTObjets<T>::m_pData , CLGTObjets<T>::m_nSize , TriCroissant ) ;

// memorisation du dernier type de tri
m_DernierTri = (TriCroissant) ? TRI_CROISSANT : TRI_DECROISSANT ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Trie les elements d'un tableau par la methode
/// Schell Metzner suivant l'indexe dichotomique string (tri croissant).
/// Ne teste pas s'il y a deja eu un tri.
template < class T >
inline void CLGArrayPtr< T >::TriSchellMetznerForDichotomyString()
{
int gap,i,j;
int n=GetSize() ;
T * temp ;

for( gap = n/2 ; gap > 0 ; gap /= 2 )
    {
    for( i = gap ; i < n ; ++i )
        {
        for ( j = i-gap ;j >= 0 && ( strcmp((*this)[j]->GetDichotomyString(),(*this)[j+gap]->GetDichotomyString()) > 0 ) ; j -= gap )
            {
            temp = (*this)[j] ;
            (*this)[j] = (*this)[j+gap] ;
            (*this)[j+gap] = temp ;
            }
        }
    }

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_STRING ;
}

//////////////////////////////////////////////////////////////////////
/// \brief
template < class T >
inline void CLGArrayPtr< T >::TriQuickSortForDichotomyString()
{
CLGQuickSortArrp<T>::quickSortForDichotomyString( CLGTObjets<T>::m_pData , 0 , CLGTObjets<T>::m_nSize -1 ) ;

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_STRING ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Trie les elements d'un tableau par la methode
/// Schell Metzner suivant l'indexe dichotomique (tri croissant).
/// Ne teste pas s'il y a deja eu un tri.
template < class T >
inline void CLGArrayPtr< T >::TriSchellMetznerForDichotomyIndex()
{
int gap,i,j;
int n=GetSize() ;
T * temp ;

for( gap = n/2 ; gap > 0 ; gap /= 2 )
    {
    for( i = gap ; i < n ; ++i )
        {
        for ( j = i-gap ;j >= 0 && ( (*this)[j]->GetDichotomyIndex() > (*this)[j+gap]->GetDichotomyIndex() ) ; j -= gap )
            {
            temp = (*this)[j] ;
            (*this)[j] = (*this)[j+gap] ;
            (*this)[j+gap] = temp ;
            }
        }
    }

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_INDEX ;
}


//////////////////////////////////////////////////////////////////////
/// \brief
template < class T >
inline void CLGArrayPtr< T >::TriFusionForDichotomyIndex()
{
CLGMSMergeSortThreadedArrp<T>::SortMergeForDichotomyIndex( CLGTObjets<T>::m_pData , CLGTObjets<T>::m_nSize ) ;

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_INDEX ;
}

//////////////////////////////////////////////////////////////////////
/// \brief
template < class T >
inline void CLGArrayPtr< T >::TriQuickSortForDichotomyIndex()
{
CLGQuickSortArrp<T>::quickSortForDichotomyIndex( CLGTObjets<T>::m_pData , 0 , CLGTObjets<T>::m_nSize -1 ) ;

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_INDEX ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Trie les elements d'un tableau par la methode
/// Schell Metzner suivant la valeur dichotomique (tri croissant).
/// Ne teste pas s'il y a deja eu un tri.
template < class T >
inline void CLGArrayPtr< T >::TriSchellMetznerForDichotomyValue()
{
int gap,i,j;
int n=GetSize() ;
T * temp ;

for( gap = n/2 ; gap > 0 ; gap /= 2 )
    {
    for( i = gap ; i < n ; ++i )
        {
        for ( j = i-gap ;j >= 0 && ( (*this)[j]->GetDichotomyValue() > (*this)[j+gap]->GetDichotomyValue() ) ; j -= gap )
            {
            temp = (*this)[j] ;
            (*this)[j] = (*this)[j+gap] ;
            (*this)[j+gap] = temp ;
            }
        }
    }

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_VALUE ;
}

//////////////////////////////////////////////////////////////////////
/// \brief
template < class T >
inline void CLGArrayPtr< T >::TriQuickSortForDichotomyValue()
{
CLGQuickSortArrp<T>::quickSortForDichotomyValue( CLGTObjets<T>::m_pData , 0 , CLGTObjets<T>::m_nSize -1 ) ;

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_VALUE ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Permet de comparer 2 objets. Utilisée dans les tris.
/// \param pObjetA [in] : pointeur sur l'objet A
/// \param pObjetB [in] : pointeur sur l'objet B
/// \param APlusGrandQueB [in] : a>b si 1 (par defaut), ou a<b sinon.
/// \return le résultat de la comparaison.
template < class T >
inline bool CLGArrayPtr< T >::Compare( const T * pObjetA , const T * pObjetB , bool APlusGrandQueB ) const
{
if ( APlusGrandQueB )
    {
    if ( *pObjetA > *pObjetB )
        return true ;
    else
        return false ;
    }
else
    {
    if ( *pObjetA < *pObjetB )
        return true ;
    else
        return false ;
    }
}

//////////////////////////////////////////////////////////////////////
/// \brief Permet le trie des elements d'un tableau par la methode du
/// tri a bulle. Ne teste pas s'il y a deja eu un tri.
/// \param TriCroissant [in] : tri croissant si =1 (par defaut)
template < class T>
inline void CLGArrayPtr< T >::TriBulleForDichotomyString()
{
for ( int i = GetUpperBound()-1 ; i >= 0 ; )
    {
    const char * p1 = (*this)[i]->GetDichotomyString() ;
    const char * p2 = (*this)[i+1]->GetDichotomyString() ;

    if ( strcmp(p1,p2) > 0 ) // si superieur
        {
        T* Temp = (*this)[i+1] ;       // on interverti les pointeurs des class T
        (*this)[i+1] = (*this)[i] ;
        (*this)[i] = Temp ;
        if ( (i+1) < GetUpperBound() )
            i++ ;
        }
    else
        i-- ;
    }

// memorisation du dernier type de tri
m_DernierTri = TRI_DICHOTOMIE_STRING ;
}

//////////////////////////////////////////////////////////////////////
/// \brief Permet de comparer 2 objets. Utilisée dans les tris.
/// \param ObjetA [in] : objet A
/// \param ObjetB [in] : objet B
/// \param APlusGrandQueB [in] : a>b si 1 (par defaut), ou a<b sinon.
/// \return le résultat de la comparaison.
template < class T >
inline bool CLGArray< T >::Compare( const T & ObjetA , const T & ObjetB , bool APlusGrandQueB ) const
{
if ( APlusGrandQueB )
    {
    if ( ObjetA > ObjetB )
        return true ;
    else
        return false ;
    }
else
    {
    if ( ObjetA < ObjetB )
        return true ;
    else
        return false ;
    }
}
*/


