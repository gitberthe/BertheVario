//////////////////////////////////////////////////////////////////////////////
/// \file   CLGTObjets.h
/// \date 23/03/2002 : date de creation
/// \addtogroup LibGeneral
/// \brief CLGTObjets
///
/// \date 23/06/2016 : Ajout de CLGTObjets::GetData().
/// \date 19/10/2020 : NVIDIA_FUNC
/// \date 14/06/2025 : On enleve les NVIDIA_FUNC.
/// \date 14/06/2025 : Derniere modification.
///

#ifndef _CTOBJETS_
#define _CTOBJETS_

///////////////////////////////////////////////////////////////////////////////
/// \brief Template de base pour un tableau (mere de CLGArray et CLGArrayPtr).
template <class T>
class CLGTObjets
{
public:

    CLGTObjets();

    int GetSize() const { return m_nSize; } ;
    int GetUpperBound() const { return m_nSize-1; } ;
    void SetSize(int nNewSize, int nGrowBy = -1) ;
    void SetGrow(int nGrowBy) ;
    int GetGrow() const
                        {return m_nGrowBy;} ;

    // Clean up
    void    FreeExtra();
    void    RemoveAll() { SetSize(0); } ;

    // Accessing elements
    const T & GetAt(int nIndex) const
            { ASSERT(nIndex >= 0 )
              ASSERT(nIndex < m_nSize); return m_pData[nIndex]; } ;
    void SetAt(int nIndex, const T & newElement)
            { ASSERT(nIndex >= 0 )
              ASSERT(nIndex < m_nSize);m_pData[nIndex] = newElement; } ;
    T& ElementAt(int nIndex)
            { ASSERT(nIndex >= 0 )
              ASSERT(nIndex < m_nSize );return m_pData[nIndex]; } ;
    T* GetData()
            { return m_pData; } ;
    const T* GetData() const
            { return m_pData; } ;
    bool IsIn( const T & Element ) const ;

    // Potentially growing the array
    void SetAtGrow( int nIndex , const T & newElement );
    int Add( const T & newElement)
            { int nIndex = m_nSize; SetAtGrow(nIndex, newElement);return nIndex; } ;

    // overloaded operator helpers
    const T & operator[](int nIndex) const { return GetAt(nIndex); } ;
    T&   operator[](int nIndex) { return ElementAt(nIndex); } ;

    // Operations that move elements around
    void InsertAt(int nIndex, const T & newElement, int nCount = 1);
    void RemoveAt(int nIndex, int nCount = 1);
    void InsertAt(int nStartIndex, const CLGTObjets & NewArray);
    #ifndef __CUDA_ARCH__
    void Shuffle( bool Deterministe = false ) ;
    void ShuffleCircular() ;
    #endif
    void Print() const ;
    void EncodeOneHot(int Class,int NumClass) ;
    int DecodeOneHot() const ;

    bool IsAllEqualLong() const ;
    bool IsAllEqualDouble() const ;
    int GetMaxIndex(T& MaxVal) const ;
    int GetMinIndex(T& MinVal) const ;

// Implementation
protected:

    T* m_pData __attribute__ ((packed));        ///< the actual array of data __attribute__((packed, aligned(sizeof(short))))
    int    m_nSize;        ///< nombre d'elements (upperBound - 1)
    int    m_nMaxSize;     ///< max allocated
    int    m_nGrowBy;      ///< grow amount

public:
    ~CLGTObjets();

private :
};

#include "CLGTObjets_inl.h"

#endif // __CTOBJETS__
