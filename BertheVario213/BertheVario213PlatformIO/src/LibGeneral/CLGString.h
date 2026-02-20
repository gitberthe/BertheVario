//////////////////////////////////////////////////////////////////////////////
/// \file CLGString.h
/// \date 23/03/2002 : date de creation
/// \addtogroup LibGeneral
/// \brief CLGString
///
/// \date 19/02/2016 : Implementation de MakeReverse.
/// \date 22/07/2016 : Implementation de Format.
/// \date 16/02/2017 : Correction d'une fuite memoire?
/// \date 28/12/2022 : Derniere modification.
///

#ifndef __CMYSTRING__
#define __CMYSTRING__

///////////////////////////////////////////////////////////////////////////////
/// \brief Classe de manipulation de chaine (contient un char *).

class CLGString
{
public:

// constructeur
    CLGString();
    CLGString(const CLGString& stringSrc);
    CLGString(char ch, int nRepeat = 1);
    CLGString(const char* psz);
    CLGString(const char* pch, int nAllocMin);

    ~CLGString();

// attributs et operations

    // comme un tableau de carracteres
    int  Strlen() ;
    int  GetLength() const;     ///< nombre de caracteres dans la chaine sans le 0
    int  GetUpperBound() const { return m_nDataLength-1 ;} ;
    int  IsEmpty() const;
    void Empty();               ///< libere les donnees
    bool IsNumber() ;

    const char & GetAt(int nIndex) const;       // a partir de 0
    const char & operator[](int nIndex) const;  // comme GetAt
    void SetAt(int nIndex, char ch);            // ecrase un caractere
    operator const char*() const;               // comme une chaine en C
    //operator char*() ;               // comme une chaine en C
    const char * LeftShift() ;                  // decal a gauche

    // operateur d'assignement
    const CLGString& operator=(const CLGString& stringSrc);
    const CLGString& operator=(char ch);
    const CLGString& operator=(const char* psz);

    // concatenation de chaine
    const CLGString& operator+=(const CLGString& string);
    const CLGString& operator+=(char ch);
    const CLGString& operator+=(const char* psz);

    friend CLGString operator+(const CLGString& string1, const CLGString& string2);
    friend CLGString operator+(const CLGString& string, char ch);
    friend CLGString operator+(char ch, const CLGString& string);
    friend CLGString operator+(const CLGString& string, const char* psz);
    friend CLGString operator+(const char* psz, const CLGString& string);

    // comparaison de chaine
    int Compare(const char* psz) const;
    int CompareNoCase(const char* psz) const;
    int Collate(const char* psz) const;

    // extraction de sous chaine simple
    CLGString Mid(int nFirst, int nCount) const;
    CLGString Mid(int nFirst) const;
    CLGString Left(int nCount) const;
    CLGString Right(int nCount) const;

    CLGString SpanIncluding(const char* pszCharSet) const;
    CLGString SpanExcluding(const char* pszCharSet) const;

    // conversion upper/lower/reverse
    void MakeUpper();
    void MakeLower();
    void MakeReverse();

    // recherche (retourne l'index de depart, ou -1 si rien trouvé
    // recherche pour un seul caractere
    int Find(char ch) const;                    ///< comme en "C" la fonction strchr
    int ReverseFind(char ch) const;
    int FindOneOf(const char* pszCharSet) const;

    // recherche pour une sous chaine
    int Find(const char* pszSub) const;         ///< comme en "C" strstr

    // accet a la chaine comme un buffer de caracteres en "C"
    char* GetBuffer(int nMinBufLength = 0 );
    char* GetBufferSetLength(int nNewLength) ; // pour compatibilite anterieure
    void FreeExtra() ;
    void Format(const char * , ... ) ;
    //char * Strtok(const char * c , bool init = false ) ;

// Implementation
protected:
    // longeur/taille en characteres
    //  (note: un caractere suplementaire est toujours alloue
    char* m_pchData;            ///< chaine actuelle terminee par zero
    int   m_nDataLength;        ///< n'inclus pas le 0 de fin
    int   m_nAllocLength;       ///< n'inclus pas le 0 de fin

    // l'implementation de base
    void Init();
    void AllocCopy(CLGString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
    void AllocBuffer(int nLen);
    void AssignCopy(int nSrcLen, const char* pszSrcData);
};


// implementation de la comparaison
int operator==(const CLGString& s1, const CLGString& s2);
int operator==(const CLGString& s1, const char* s2);
int operator==(const char* s1, const CLGString& s2);
int operator!=(const CLGString& s1, const CLGString& s2);
int operator!=(const CLGString& s1, const char* s2);
int operator!=(const char* s1, const CLGString& s2);
int operator<(const CLGString& s1, const CLGString& s2);
int operator<(const CLGString& s1, const char* s2);
int operator<(const char* s1, const CLGString& s2);
int operator>(const CLGString& s1, const CLGString& s2);
int operator>(const CLGString& s1, const char* s2);
int operator>(const char* s1, const CLGString& s2);
int operator<=(const CLGString& s1, const CLGString& s2);
int operator<=(const CLGString& s1, const char* s2);
int operator<=(const char* s1, const CLGString& s2);
int operator>=(const CLGString& s1, const CLGString& s2);
int operator>=(const CLGString& s1, const char* s2);
int operator>=(const char* s1, const CLGString& s2);
ostream& operator<<(ostream& os, const CLGString& s1) ;

#include "CLGString_inl.h"

#endif // __CMYSTRING__
