//////////////////////////////////////////////////////////////////////////////
/// \file CLGArray.h
/// \date 23/03/2002 : date de creation
/// \addtogroup LibGeneral
/// \brief CLGArray et CLGArrayPtr.
///
/// \date 30/12/2002 : Passage de fonctions en const.
/// \date 11/01/2003 : Ajout de CopyWithoutAlloc() dans CLGArrayPtr.
/// \date 22/12/2004 : Tri et recherche par dichotomie.
/// \date 24/09/2005 : Memorisation du dernier tri.
/// \date 09/03/2017 : Implementation de dichotomie pour les float
/// \date 24/01/2019 : Ajout du tri fusion
/// \date 19/10/2020 : NVIDIA_FUNC
/// \date 28/10/2020 : NVIDIA_FUNC.
/// \date 21/10/2021 : Ajout de CLGQuickSortThreadArrp.
/// \date 06/02/2023 : Derniere modification.
///

#ifndef _CLG_ARRAY_H_
#define _CLG_ARRAY_H_

// memorisation des tri pour la dichotomie
#define TRI_DECROISSANT         ((int)0)
#define TRI_CROISSANT           ((int)1)
#define TRI_DICHOTOMIE_INDEX    ((int)2)
#define TRI_DICHOTOMIE_VALUE    ((int)3)
#define TRI_DICHOTOMIE_STRING   ((int)4)
#define AUCUN_TRI               ((int)5)

template <class T>
class CLGLocalArr ;

template <class T>
class CLGLocalArrp ;

//////////////////////////////////////////////////////////////////////
/// \brief Classe de base pour un tableau d'objets.
///
#ifdef __CUDA_ARCH__
template <class T>
class CLGArray : public CLGTObjets< T >
#else
template <class T>
class CLGArray : public CLGTObjets< T > ,  public CLGQuickSortArr< T > // public CLGQuickSortThreadArr< T >
#endif
{
public :
    // constructeur
    CLGArray() ;
    CLGArray( int Size , int GrowBy = 0 ) ;
    CLGArray( const CLGArray & ) ;
    ~CLGArray() {} ;

    // fonctions
    void TriAdapte( bool TriCroissant = true ) ;
    void Push( const T & AEmpilerParleBas ) ;
    void SetAll( const T & ACopierPartout ) ;
    int Find( const T & element ) const ;
    int FindWithDichotomy( const T & DichotomyToFind , CLGArray<int> & IndexArray ) const ;
    void InsertAtEnd( const CLGArray<T> & NewArray ) ;

    // operateurs
    const CLGArray& operator = ( const CLGArray& Array ) ;
    bool     operator == ( const CLGArray<T> & Other ) const ;

    friend class CLGLocalArr<T> ;

protected :

private :
    // void TriQuickSortThread( bool TriCroissant = true ) ;
    void TriBulle( bool TriCroissant = true ) ;
    void TriSchellMetzner( bool TriCroissant = true ) ;
    // bool Compare( const T & ObjetA , const T & ObjetB , bool APlusGrandQueB = true ) const ;
} ;

/*****************************************************************************/

//////////////////////////////////////////////////////////////////////
/// \brief Permet la recherche par dichotomie dans un CLGArrayPtr
/// (apres un tri pour dichotomie string).
/// Fonction virtuelle GetDichotomyString() a implementée dans la classe fille.
///
class CLGForDichotSearchString
{
public :
    virtual const char * GetDichotomyString() const = 0 ; // { return 0 ; } ;
} ;

//////////////////////////////////////////////////////////////////////
/// \brief Permet la recherche par dichotomie dans un CLGArrayPtr
/// (apres un tri pour dichotomie value).
/// Fonction virtuelle GetDichotomyValue() a implementée dans la classe fille.
///
class CLGForDichotSearchValue
{
public :
    virtual float GetDichotomyValue() const = 0 ; // { return 0 ; } ;
} ;

//////////////////////////////////////////////////////////////////////
/// \brief Permet la recherche par dichotomie dans un CLGArrayPtr
/// (apres un tri pour dichotomie index).
/// Fonction virtuelle GetDichotomyIndex() implementée dans la classe fille.
///
class CLGForDichotSearchIndex
{
public :
    virtual int GetDichotomyIndex() const = 0 ; // { return 0 ; } ;
} ;

/*****************************************************************************/

//////////////////////////////////////////////////////////////////////
/// \brief Classe de base pour un tableau de pointeur d'objets (voir aussi
/// la classe CLGForDichotSearchIndex).
#ifdef __CUDA_ARCH__
template <class T>
class CLGArrayPtr : public CLGTObjets< T * >
#else
template <class T>
class CLGArrayPtr : public CLGTObjets< T * > /*, public CLGMSMergeSortThreadedArrp<T> */ , public CLGQuickSortArrp< T > // , public CLGQuickSortThreadArrp<T>
#endif
{
public :
    // constructeur destructeurs
    CLGArrayPtr( bool DeleteObject = false ) ;
    CLGArrayPtr( const CLGArrayPtr & ) ;
    virtual ~CLGArrayPtr() {DeleteAll();} ;

    // fonctions
    void DeleteAll() ;
    int GetLastSort() const
                        { return m_DernierTri ; } ;
    void SetNoSort()
                        {m_DernierTri=AUCUN_TRI;};
    void Shuffle( bool Deterministe = false ) ;
    void TriAdapte( bool TriCroissant = true ) ;

    // fonctions pour la recherche par string de dichotomie
    //void TriQuickSortThreadForDichoString() ;
    int FindWithDichotomyString( const char * DichotomyStringToFind , CLGArray< const T * > & ArrayPtrResult ) const ;
    int FindWithDichotomyStringIndexToInsert( const char * String ) const ;

    // fonctions pour la recherche par valeur de dichotomie
    //void TriQuickSortThreadForDichoValue() ;
    int FindWithDichotomyValue( float ValMinExclude , float ValMaxInclude , CLGArray< const T * > & ArrayPtrResult ) const ;
    int FindWithDichotomyValueIndexToInsert( float Value ) const ;
    int FindAroundDichotomyValue( float ValMinExclude , float ValMaxInclude , CLGArray< const T * > & ArrayPtrResult , int StartIndex ) const ;

    // fonctions pour la recherche par index de dichotomie
    //void TriQuickSortThreadForDichoIndex() ;
    int FindWithDichotomyIndex( int DichotomyIndexToFind , CLGArray< const T * > & ArrayPtrResult ) const ;
    int FindWithDichotomyIndexIndexToInsert( int Value ) const ;

    // operateur
    const CLGArrayPtr& operator = ( const CLGArrayPtr& Array ) ;
    const CLGArrayPtr& CopyWithoutAlloc( const CLGArrayPtr& Array, bool DeleteObject = false ) ;
    bool  IsObjectAllocated() const
                        { return m_DeleteObject ; } ;
    void SetDeleteObjet( bool DeleteObject )
                        { ASSERT( GetSize() == 0 ) ; m_DeleteObject = DeleteObject ; } ;
    int Find( const T * pelement ) const ;

    // fonctions de la classe CLGTObjets
    int  GetSize() const { return CLGTObjets<T*>::GetSize(); } ;
    int  GetUpperBound() const
                        { return CLGTObjets<T*>::GetUpperBound(); } ;
    void SetSize(int nNewSize, int nGrowBy = -1)
                        { m_DernierTri=AUCUN_TRI; CLGTObjets<T*>::SetSize( nNewSize, nGrowBy ); } ;
    void SetGrow(int nGrowBy)
                        { CLGTObjets<T*>::SetGrow( nGrowBy ); } ;

    // Clean up
    void FreeExtra()
                        { m_DernierTri=AUCUN_TRI; CLGTObjets<T*>::FreeExtra(); } ;
    void RemoveAll()
                        { m_DernierTri=AUCUN_TRI; CLGTObjets<T*>::RemoveAll(); } ;

    // Accessing elements
    const T* GetAt(int nIndex) const
                        { return CLGTObjets<T*>::GetAt(nIndex); } ;
    void SetAt(int nIndex, T* newElement)
                        { /*m_DernierTri=AUCUN_TRI;*/ return CLGTObjets<T*>::SetAt( nIndex, newElement ); } ;
    T* & ElementAt(int nIndex)
                        { /*m_DernierTri=AUCUN_TRI;*/ return CLGTObjets<T*>::ElementAt( nIndex ); } ;

    // Potentially growing the array
    void SetAtGrow( int nIndex , T* newElement )
                        { m_DernierTri=AUCUN_TRI; CLGTObjets<T*>::SetAtGrow( nIndex , newElement ); } ;
    int  Add( T* newElement )
                        { m_DernierTri=AUCUN_TRI; return CLGTObjets<T*>::Add( newElement ); } ;

    // overloaded operator helpers
    const T* operator[](int nIndex) const
                        { return GetAt(nIndex); } ;
    T* &     operator[](int nIndex)
                        { return ElementAt(nIndex); } ;

    // Operations that move elements around
    void InsertAt(int nIndex, T* newElement, int nCount = 1 , bool ChangeTri = true )
                        {
                        if (ChangeTri) m_DernierTri=AUCUN_TRI ;
                        CLGTObjets<T*>::InsertAt( nIndex, newElement, nCount ) ;
                        } ;
    void RemoveAt(int nIndex, int nCount = 1)
                        { CLGTObjets<T*>::RemoveAt( nIndex, nCount ); } ;
    void RemoveThisIndex( CLGArray<int> & IndexToRemovArr ) ;
                void CompressNull() ;
    void InsertAt(int nStartIndex, const CLGArrayPtr<T> & NewArray )
                        { m_DernierTri=AUCUN_TRI; CLGTObjets<T*>::InsertAt( nStartIndex, NewArray ); }
    void InsertAtEnd( const CLGArrayPtr<T> & NewArray ) ;

    friend class CLGLocalArrp<T> ;

protected :

private :
    bool m_DeleteObject ;
    int m_DernierTri ; ///< indique le dernier tri effectué.

    void TriBulle( bool TriCroissant = true ) ;
    void TriSchellMetzner( bool TriCroissant = true ) ;
    //void TriQuickSort( bool TriCroissant = true ) ;
    //void TriQuickSortThread( bool TriCroissant = true ) ;
} ;

//////////////////////////////////////////////////////////////////////
// Implementation des fonctions des classes.
#include "CLGArray_inl.h"

#endif
