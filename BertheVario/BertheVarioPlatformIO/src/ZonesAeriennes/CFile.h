////////////////////////////////////////////////////////////////////////////////
/// \file CFile.h
///
/// \brief
///
/// \date 12/10/2026 : creation
/// \date 12/10/2026 : modification
///

#ifndef _CFILE_
#define _CFILE_

////////////////////////////////////////////////////////////////////////////////
/// \brief classe de lecture de fichier FreRtOs et linux
class CFile : public std::vector<std::string>
{
public :
    int  OuvrirFichier( const char * FileName , bool Lecture = true ) ;
    void FermerFichier() ;
    int  LireFichier() ;
    int  Ecrire( const char * Ligne ) ;
    int  EcrireNl( const char * Ligne ) ;
    int  Ecrire( int Ligne ) ;

private :
    #ifdef _COMP_ZONES_AERIENNES_
     FILE * m_File ;
    #else
     File m_File ;
    #endif
} ;

#endif
