//////////////////////////////////////////////////////////////////////////////
/// \file CLGStringEx.cpp
/// \date 23/03/2002 : date de creation
///
/// \brief Ce fichier reprend la classe CString des MFC Windows.
///
/// \date  16/02/2017 : Correction d'une fuite memoire?
/// \date  12/11/2023 : Derniere modification
///

#include "LibGeneral.h"

extern char _afxChNil;

//////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur qui initialise avec un caractere.
CLGString::CLGString(char ch, int nRepeat)
{
m_pchData = NULL ;
if (nRepeat < 1)
    // return empty string if invalid repeat count
    Init();
else
    {
    AllocBuffer(nRepeat);
    memset(m_pchData, ch, nRepeat);
    m_nDataLength = nRepeat ;
    }
}

//////////////////////////////////////////////////////////////////////////////
/// \brief constructeur qui initialise avec une chaine de caracteres.
/// Allocation de nAllocMin au minimum.
CLGString::CLGString(const char* pch, int nAllocMin )
{
m_pchData = NULL ;
if (nAllocMin == 0 && SafeStrlen(pch) == 0)
    Init();
else
    {
    int nLen = strlen(pch) ;
    if ( nAllocMin < nLen )
        nAllocMin = nLen ;
    AllocBuffer(nAllocMin);
    m_nDataLength = nLen ;
    memcpy(m_pchData, pch, m_nDataLength );
    m_pchData[m_nDataLength] = 0 ;
    }
}

/*
//////////////////////////////////////////////////////////////////////////////
/// \brief Comme strtok avec init = true au depart.
char * CLGString::Strtok(const char * c , bool init )
{
if ( init )
    return strtok_r(m_pchData,c,&m_sav);
return strtok_r(NULL,c,&m_sav) ;
}
*/

//////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction permet d'executer un decalage a gauche de tous les caracteres.
/// La chaine est ainsi reduite de un.
const char * CLGString::LeftShift()
{
// si chaine vide
if ( IsEmpty() )
    return m_pchData ;

// copie des caracteres avec decalalge a droite
memmove( m_pchData , m_pchData + 1 , GetLength() ) ;
// caractere de fin de chaine
SetAt(GetUpperBound(),'\0') ;
return m_pchData ;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Assignment operators
/// Modif du 28/12/2022
const CLGString& CLGString::operator =(char ch)
{
char TmpChar[2] ;
TmpChar[0] = ch ;
TmpChar[1] = 0 ;
AssignCopy(1, TmpChar );
return *this;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief concatenation
/// Modif du 28/12/2022
const CLGString& CLGString::operator +=(char ch)
{
ASSERT( m_nDataLength == SafeStrlen(m_pchData) ) ;
if ( m_nDataLength + 1 > m_nAllocLength )
    {
    char * OldpChar = m_pchData ;
    m_nAllocLength = m_nDataLength+1 ;
    m_pchData = new char[m_nAllocLength+1] ;
    memcpy( m_pchData , OldpChar , m_nDataLength ) ;
    m_pchData[m_nDataLength] = ch ;
    m_nDataLength ++ ;
    m_pchData[m_nDataLength] = 0 ;
    if ( OldpChar != & _afxChNil )
        delete [] OldpChar ;
    }
else
    {
    m_pchData[m_nDataLength] = ch ;
    m_nDataLength ++ ;
    m_pchData[m_nDataLength] = 0 ;
    }

return *this;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief concatenation
/// Modif du 28/12/2022
CLGString operator +(const CLGString& String, char ch)
{
ASSERT( String.m_nDataLength == SafeStrlen(String.m_pchData) ) ;
CLGString s(String,String.m_nDataLength+1);
s += ch ;
return s;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief concatenation
/// Modif du 28/12/2022
CLGString operator +(char ch, const CLGString& String)
{
ASSERT( String.m_nDataLength == SafeStrlen(String.m_pchData) ) ;
CLGString s(ch,String.m_nDataLength+1);
s += String ;
return s;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief operateur pour stdout.
ostream& operator<<(ostream& os, const CLGString& s1)
{
os << (const char*)s1 ;
return os ;
}

//////////////////////////////////////////////////////////////////////////////
// Very simple sub-string extraction

//////////////////////////////////////////////////////////////////////////////
/// \brief renvoie la chaine a partir de nFirst.
CLGString CLGString::Mid(int nFirst) const
{
return Mid(nFirst, m_nDataLength - nFirst);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief renvoie la chaine de nFirst à nCount.
CLGString CLGString::Mid(int nFirst, int nCount) const
{
ASSERT(nFirst >= 0);
ASSERT(nCount >= 0);

// out-of-bounds requests return sensible things
if (nFirst + nCount > m_nDataLength)
    nCount = m_nDataLength - nFirst;
if (nFirst > m_nDataLength)
    nCount = 0;

CLGString dest;
AllocCopy(dest, nCount, nFirst, 0);
return dest;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief retourne les nCount carateres de droite
CLGString CLGString::Right(int nCount) const
{
ASSERT(nCount >= 0);

if (nCount > m_nDataLength)
    nCount = m_nDataLength;

CLGString dest;
AllocCopy(dest, nCount, m_nDataLength-nCount, 0);
return dest;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief retourne les nCount carateres de gauche
CLGString CLGString::Left(int nCount) const
{
ASSERT(nCount >= 0);

if (nCount > m_nDataLength)
    nCount = m_nDataLength;

CLGString dest;
AllocCopy(dest, nCount, 0, 0);
return dest;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la chaine jusqu'au pszCharSet trouve.
CLGString CLGString::SpanIncluding(const char* pszCharSet) const
{
// strspn equivalent
return Left(strspn(m_pchData, pszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la chaine jusqu'au premier pszCharSet non trouve.
CLGString CLGString::SpanExcluding(const char* pszCharSet) const
{
// strcspn equivalent
return Left(strcspn(m_pchData, pszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Retourne la position du premier caractere trouvé ou -1 sinon.
int CLGString::Find(char ch) const
{
// find a single character (strchr)

char* psz;
psz = (char*) strchr(m_pchData, ch);
return (psz == NULL) ? -1 : psz - m_pchData;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Retourne le premier caractere trouvé depuis la fin.
int CLGString::ReverseFind(char ch) const
{
// find a single character (start backwards, strrchr)

char* psz;
psz = (char*) strrchr(m_pchData, ch);
return (psz == NULL) ? -1 : psz - m_pchData;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Retourne le premier caratere de un de pszCharSet.
int CLGString::FindOneOf(const char* pszCharSet) const
{
// like single character find, but look for any of the characters
// in the string "pszCharSet", like strpbrk

char* psz = (char*) strpbrk(m_pchData, pszCharSet);
return (psz == NULL) ? -1 : (psz-m_pchData);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Retourne la premiere occurence de pszSub ou -1 sinon.
int CLGString::Find(const char* pszSub) const
{
// find a sub-string (like strstr)

char* psz = (char*) strstr(m_pchData, pszSub);
return (psz == NULL) ? -1 : (psz-m_pchData);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction re-alloue un buffer de taille nMinBufLength si necessaire
/// (et recopie le contenu de la CLGString) mais ne le reduit pas.
/// \return Renvoie le m_pchData.
char* CLGString::GetBuffer(int nMinBufLength /*=0*/)
{
// si le buffer doit etre agrandit
if (nMinBufLength > m_nAllocLength)
    {
    char* pszOldData = m_pchData;
    int nOldLen      = m_nDataLength;   // AllocBuffer modifie m_nDataLength
    m_pchData = NULL ;
    AllocBuffer(nMinBufLength);
    memcpy(m_pchData, pszOldData, nOldLen);
    m_nDataLength = nOldLen;
    m_pchData[m_nDataLength] = '\0';

    ASSERT(pszOldData != NULL);
    if (pszOldData != &_afxChNil)
        delete [] pszOldData;
    }

// retourne le pointeur de chaine
return m_pchData;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui re-alloue le buffer si necessaire (mais ne le reduit pas).
/// Mais positionne/coupe la chaine a nNewLength ('\0').
char* CLGString::GetBufferSetLength(int nNewLength)
{
GetBuffer(nNewLength);
if ( m_nDataLength > nNewLength )
    SetAt( nNewLength , '\0' ) ;
return m_pchData ;
}

