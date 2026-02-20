//////////////////////////////////////////////////////////////////////////////
/// \file CLGString.cpp
/// \date 23/03/2002 : date de creation
///
/// \brief Ce fichier reprend la classe CString des MFC Windows.
///
/// \date  19/02/2016 : Implementation de MakeReverse.
/// \date  16/02/2017 : Correction d'une fuite memoire?
/// \date  28/12/2022 : ug avec strlen dans operateur de recopie et d'affectation.
/// \date  28/12/2022 : Derniere modification.
///

#include "LibGeneral.h"

//char _afxChNil = '\0';
char _afxChNil = 0 ;

//////////////////////////////////////////////////////////////////////////////
/// \brief Initialise a 0 les tailles et le pointeur sur _afxChNil.
void CLGString::Init()
{
if ( m_pchData!=NULL && m_pchData!=&_afxChNil )
    delete [] m_pchData ;                       // berthe ajout
m_nDataLength = m_nAllocLength = 0;
m_pchData     = & _afxChNil;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur qui initialise.
CLGString::CLGString()
{
m_pchData=NULL ;
Init();
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Operateur de recopie (ex:passage en parametre de fonction)
/// Modifier le 28/12/2022 pour m_nDataLength = strlen(m_pchData)
CLGString::CLGString(const CLGString& stringSrc)
{
if ( this == & stringSrc )
    return ;
ASSERT( stringSrc.GetLength() == (int)strlen(stringSrc.m_pchData) ) ;

m_nDataLength = m_nAllocLength = stringSrc.m_nDataLength ;
m_pchData = new char[stringSrc.m_nAllocLength+1];
memcpy(m_pchData,stringSrc.m_pchData,m_nAllocLength+1) ;
//m_nDataLength = strlen(m_pchData) ; // evite les bug de strtok
}


//////////////////////////////////////////////////////////////////////////////
/// \brief Alloue un nouveau buffer et detruit l'ancien.
/// Modif du 28/12/2022 pour une taille nulle de Getlength().
void CLGString::AllocBuffer(int nLen)
{
ASSERT(nLen >= 0);
ASSERT(nLen < 0x7FFFFFFF - 1); // max size (enough room for 1 extra)

if (nLen == 0)
    {
    Init();
    }
else
    {
    if ( m_pchData != NULL && m_pchData!=&_afxChNil)
        delete [] m_pchData ;                           // berthe ajout
    m_pchData = new char[nLen+1];
    memset(m_pchData,0,nLen+1);
    m_nDataLength = 0;
    m_nAllocLength = nLen;
    }
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Vide la chaine.
void CLGString::Empty()
{
if (m_pchData != &_afxChNil && m_pchData != NULL )
    {
    delete [] m_pchData;
    m_pchData = NULL ;
    Init();
    }
ASSERT(m_nDataLength == 0);
ASSERT(m_nAllocLength == 0);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur qui desalloue.
CLGString::~CLGString()
{
if (m_pchData != &_afxChNil && m_pchData != NULL)
    delete [] m_pchData;
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for the rest of the implementation

//////////////////////////////////////////////////////////////////////////////
/// \brief Duplique la chaine depuis nCopyIndex sur nCopyLen longueur.
/// Modif du 28/12/2022.
void CLGString::AllocCopy(CLGString& dest, int nCopyLen, int nCopyIndex,
     int nExtraLen) const
{
    // will clone the data attached to this string
    // allocating 'nExtraLen' characters
    // Places results in uninitialized string 'dest'
    // Will copy the part or all of original data to start of new string

int nNewLen = nCopyLen + nExtraLen;

if (nNewLen == 0)
    {
    dest.Init();
    }
else
    {
    dest.AllocBuffer(nNewLen);
    memcpy(dest.m_pchData, &m_pchData[nCopyIndex], nCopyLen);
    dest.m_nDataLength = strlen(dest.m_pchData)  ;
    }
}

//////////////////////////////////////////////////////////////////////////////
/// Modif du 28/12/2022.
CLGString::CLGString(const char* psz)
{
m_pchData = NULL ;
int nLen;
if ((nLen = SafeStrlen(psz)) == 0)
    Init();
else
    {
    AllocBuffer(nLen);
    memcpy(m_pchData, psz, nLen);
    m_nDataLength = nLen ;
    m_pchData[m_nDataLength] = 0 ;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Diagnostic support

//////////////////////////////////////////////////////////////////////////////
// Assignment operators
//  All assign a new value to the string
//      (a) first see if the buffer is big enough
//      (b) if enough room, copy on top of old buffer, set size and type
//      (c) otherwise free old string data, and create a new one
//
//  All routines return the new string (but as a 'const CLGString&' so that
//      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
//

//////////////////////////////////////////////////////////////////////////////
/// \brief Modif du 28/12/2022 ou m_nDataLength = strlen(m_pchData) meme si
/// m_nAllocLength est plus grand et contient des donnees.
void CLGString::AssignCopy(int nSrcLen, const char* pszSrcData)
{
// check if it will fit
if (nSrcLen > m_nAllocLength)
    {
    // it won't fit, allocate another one
    Empty();
    AllocBuffer(nSrcLen);
    }
if (nSrcLen != 0)
    memcpy(m_pchData, pszSrcData, nSrcLen);

m_pchData[nSrcLen] = '\0';
m_nDataLength = strlen(m_pchData);  // evite les bug de strtok
}


//////////////////////////////////////////////////////////////////////////////
/// \brief voir AssignCopy, Modif du 28/12/2022
const CLGString&
CLGString::operator =(const CLGString& stringSrc)
{
ASSERT( stringSrc.GetLength() == (int)strlen(stringSrc.m_pchData) ) ;
AssignCopy(stringSrc.m_nDataLength, stringSrc.m_pchData);
return *this;
}

//////////////////////////////////////////////////////////////////////////////
/// Modif du 28/12/2022
const CLGString&
CLGString::operator =(const char* psz)
{
AssignCopy(SafeStrlen(psz), psz);
return *this;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie une CLGString concatenation des 2 parametres.
/// Modif du 28/12/2022
CLGString operator +(const CLGString& String1, const CLGString& String2)
{
ASSERT( String1.m_nDataLength == SafeStrlen(String1.m_pchData) ) ;
ASSERT( String2.m_nDataLength == SafeStrlen(String2.m_pchData) ) ;
CLGString s( String1.m_pchData , String1.m_nDataLength + String2.m_nDataLength );
s += String2 ;
return s;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie une CLGString concatenation des 2 parametres.
/// Modif du 28/12/2022
CLGString operator +(const CLGString& String, const char* psz)
{
ASSERT( String.m_nDataLength == SafeStrlen(String.m_pchData) ) ;
CLGString s( String.m_pchData , String.m_nDataLength + strlen( psz ) ) ;
s += psz ;
return s;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie une CLGString concatenation des 2 parametres.
CLGString operator +(const char* psz, const CLGString& String)
{
ASSERT( String.m_nDataLength == SafeStrlen(String.m_pchData) ) ;
CLGString s(psz,strlen(psz)+String.m_nDataLength);
s += String ;
return s;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Concatenation de chaine.
/// Modif du 28/12/2022
const CLGString& CLGString::operator +=(const char* psz)
{
ASSERT( m_nDataLength == SafeStrlen(m_pchData) ) ;
int nLen = strlen(psz) ;
if ( m_nDataLength + nLen > m_nAllocLength )
    {
    char * OldpChar = m_pchData ;
    m_nAllocLength = m_nDataLength+nLen ;
    m_pchData = new char[m_nAllocLength+1] ;
    memcpy( m_pchData , OldpChar , m_nDataLength ) ;
    memcpy( m_pchData+m_nDataLength, psz , nLen ) ;
    m_nDataLength += nLen ;
    m_pchData[m_nDataLength] = 0 ;
    if ( OldpChar != &_afxChNil )
        delete [] OldpChar ;
    }
else
    {
    memcpy( m_pchData+m_nDataLength, psz , nLen ) ;
    m_nDataLength += nLen ;
    m_pchData[m_nDataLength] = 0 ;
    }

//ConcatInPlace(SafeStrlen(psz)+1, psz);
return *this;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Concatenation de chaine.
/// Modif du 28/12/2022
const CLGString& CLGString::operator +=(const CLGString& String)
{
ASSERT( m_nDataLength == SafeStrlen(m_pchData) ) ;
ASSERT( String.m_nDataLength == SafeStrlen(String.m_pchData) ) ;
if ( m_nDataLength + String.m_nDataLength > m_nAllocLength )
    {
    char * OldpChar = m_pchData ;
    m_nAllocLength = m_nDataLength+String.m_nDataLength ;
    m_pchData = new char[m_nAllocLength+1] ;
    memcpy( m_pchData , OldpChar , m_nDataLength ) ;
    memcpy( m_pchData+m_nDataLength, String.m_pchData , String.m_nDataLength ) ;
    m_nDataLength += String.m_nDataLength ;
    m_pchData[m_nDataLength] = 0 ;
    if ( OldpChar != &_afxChNil )
        delete [] OldpChar ;
    }
else
    {
    memcpy( m_pchData+m_nDataLength, String.m_pchData , String.m_nDataLength ) ;
    m_nDataLength += String.m_nDataLength ;
    m_pchData[m_nDataLength] = 0 ;
    }
return *this;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Desalloue la memoire en trop (avec recopie de la chaine)
/// Modif du 28/12/2022
void CLGString::FreeExtra()
{
ASSERT(m_nDataLength <= m_nAllocLength);
ASSERT( SafeStrlen(m_pchData) <= m_nAllocLength);
if (m_nDataLength != m_nAllocLength)
    {
    char * OldpChar = m_pchData ;
    m_nDataLength=m_nAllocLength=SafeStrlen(m_pchData);
    m_pchData= new char [m_nAllocLength+1]  ;
    memcpy(m_pchData, OldpChar, m_nDataLength + 1);
    if ( OldpChar != &_afxChNil )
        delete [] OldpChar ;
    }
ASSERT(m_pchData != NULL);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Remplace la fonction sprintf
/// \param pFormat [in] : parametre "%Lf" ... comme dans sprintf
/// \param ... [in] : parametres comme dans sprintf
void CLGString::Format(const char * pFormat , ... )
{
// allocation du buffer de conversion
//char pchar[50000] ;
AllocBuffer(50000) ;

// conversion par sprintf
va_list argptr;
va_start(argptr, pFormat);
vsprintf(m_pchData,pFormat,argptr) ;  // voir va_arg( argptr, type a retourne (int,float,...) );
va_end(argptr);

Strlen() ;
FreeExtra() ;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Renseigne si la chaine est un nombre.
bool CLGString::IsNumber()
{
bool PointTrouve = false ;
for ( int ic = 0 ; ic < GetLength() ; ic++ )
    {
    const char c = GetAt(ic) ;
    if ( (c == '-' || c == '+') && ic == 0 )
        {}
    else if ( isdigit(c) )
        {}
    else if ( c == '.' && !PointTrouve )
        PointTrouve = true ;
    else
        return false ;
    }
return true ;
}

