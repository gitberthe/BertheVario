////////////////////////////////////////////////////////////////////////////////
/// \file CFile.cpp
///
/// \brief
///
/// \date 12/10/2026 : creation
/// \date 12/10/2026 : modification
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif
#ifdef _COMP_ZONES_AERIENNES_
 #include "../../../../CompZoneAerienne/src/BertheVarioIncludes.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief
int CFile::OuvrirFichier( const char * FileName , bool lecture )
{
#ifdef _COMP_ZONES_AERIENNES_
#else
// ouverture fichier
if ( lecture )
    m_File = SD.open(FileName);
else
    {
    m_File = SD.open(FileName, FILE_WRITE , true ) ;
    m_File.seek(0) ;
    }

if ( !m_File )
    {
    Serial.println("Failed to open file");
    return 1;
    }
#endif
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CFile::FermerFichier()
{
#ifdef _COMP_ZONES_AERIENNES_
#else
 m_File.close() ;
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
int CFile::Ecrire( int Ligne )
{
#ifdef _COMP_ZONES_AERIENNES_
#else
 m_File.print( Ligne ) ;
#endif
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
int CFile::Ecrire( const char * Ligne )
{
#ifdef _COMP_ZONES_AERIENNES_
#else
 m_File.print( Ligne ) ;
#endif
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
int CFile::EcrireNl( const char * Ligne )
{
#ifdef _COMP_ZONES_AERIENNES_
#else
char c1 = 0x0d ;
char c2 = 0x0a ;
m_File.print( Ligne ) ;
m_File.print( c1 ) ;
m_File.print( c2 ) ;
#endif
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture de petit fichier
int CFile::LireFichier()
{
#ifdef _COMP_ZONES_AERIENNES_
#else
char TmpChar[1000] = "" ;

// raz precedent
clear() ;

// lecture fichier
int ic = 0 ;
while(m_File.available())
    {
    char c = m_File.read() ;
    TmpChar[ic++] = c ;
    if ( c == '\n' )
        {
        TmpChar[ic++] = 0 ;
        ic = 0 ;
        push_back( TmpChar ) ;
        }
    }
TmpChar[ic++] = 0 ;
//std::string Ligne = TmpChar ;
push_back( TmpChar ) ;
#endif
return 0 ;
}
