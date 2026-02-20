////////////////////////////////////////////////////////////////////////////////
/// \file CKmlFile.cpp
///
/// \brief
///
/// \date 09/01/2026 : creation
/// \date 09/01/2026 : modification
///

#include "CompZoneAerienne.h"

using namespace std;

void CKmlFile::WriteFile( const CZone & Zone , long iz )
{
char TmpChar[5000] ;
string nom_fch_simple = Zone.m_NameSansBlanc ;

// nom avec chemin
sprintf( TmpChar , "%s/%03ld-%s.big.kml" , CHEMIN_KML , iz , nom_fch_simple.c_str() ) ;
// ouverture du fichier
ofstream ofsb( TmpChar , std::ofstream::out);

// entete
// example : <altitudeMode>absolute</altitudeMode>
// example : <altitudeMode>relativeToGround</altitudeMode>
ofsb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n\
 \n\
<Placemark>\n\
  <name>" ; ofsb << "Big-" << Zone.m_Name ; ofsb << "</name>\n\
  <LineString>\n\
    <extrude>1</extrude>\n\
    <altitudeMode>relativeToGround</altitudeMode>\n\
    <coordinates>\n" ;

for ( unsigned long ip = 0 ; ip < Zone.m_VecPtsBig.size() ; ip++ )
    {
    CVecZoneReduce::st_coord_poly * pPt = Zone.m_VecPtsBig[ip] ;
    ofsb << setprecision(9) << pPt->m_Lon << "," << setprecision(9) << pPt->m_Lat << "," << "200" << endl ;
    }

// fin
ofsb << "</coordinates>\n\
  </LineString>\n\
</Placemark>\n\
 \n\
</kml>\n" ;

ofsb.close() ;

// nom avec chemin
sprintf( TmpChar , "%s/%03ld-%s.small.kml" , CHEMIN_KML , iz , nom_fch_simple.c_str() ) ;
// ouverture du fichier
ofstream ofss( TmpChar , std::ofstream::out);

// entete
// example : <altitudeMode>absolute</altitudeMode>
// example : <altitudeMode>relativeToGround</altitudeMode>
ofss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n\
 \n\
<Placemark>\n\
  <name>" ; ofss << "Small-" << Zone.m_Name ; ofss << "</name>\n\
  <LineString>\n\
    <extrude>1</extrude>\n\
    <altitudeMode>relativeToGround</altitudeMode>\n\
    <coordinates>\n" ;

for ( unsigned long ip = 0 ; ip < Zone.m_VecPtsSmall.size() ; ip++ )
    {
    CVecZoneReduce::st_coord_poly * pPt = Zone.m_VecPtsSmall[ip] ;
    ofss << setprecision(9) << pPt->m_Lon << "," << setprecision(9) << pPt->m_Lat << "," << "100" << endl ;
    }

// fin
ofss << "</coordinates>\n\
  </LineString>\n\
</Placemark>\n\
 \n\
</kml>\n" ;

ofss.close() ;
}
