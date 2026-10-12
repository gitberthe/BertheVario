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

////////////////////////////////////////////////////////////////////////////////
/// \brief
int CFile::OuvrirFichier( const char * FileName , bool lecture )
{
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

return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CFile::FermerFichier()
{
m_File.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
int CFile::Ecrire( const char * Ligne )
{
m_File.print( Ligne ) ;
return 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture de petit fichier
int CFile::LireFichier()
{
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
        std::string Ligne = TmpChar ;
        push_back( Ligne ) ;
        }
    }
TmpChar[ic++] = 0 ;
std::string Ligne = TmpChar ;
push_back( Ligne ) ;

return 0 ;
}
