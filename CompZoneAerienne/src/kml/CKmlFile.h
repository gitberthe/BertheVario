////////////////////////////////////////////////////////////////////////////////
/// \file CKmlFile.h
///
/// \brief
///
/// \date 09/01/2026 : creation
/// \date 09/01/2026 : modification
///

#ifndef _CKML_FILE_
#define _CKML_FILE_

#define CHEMIN_KML_BAZIL    "./zones_kml_bazile/"
#define CHEMIN_KML_OPENAIR  "./zones_kml_openair/"

class CKmlFile
{
public :
    void WriteFile( const CZone & Zone , long iz ) ;
} ;

#endif
