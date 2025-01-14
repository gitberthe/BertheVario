////////////////////////////////////////////////////////////////////////////////
/// \file CFileGpx.h
///
/// \brief
///
/// \date creation     : 14/01/2025
/// \date modification : 14/01/2025
///

#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \brief Fichier Gpx de rando-vol. Avec son integration au GnuVario
class CFileGpx
{
public :
    struct StPoint
        {
        float m_Lat ;
        float m_Lon ;
        int   m_Alt ;
        } ;

    void    LireFichier() ;
    void    SetDistanceFrom( const StPoint PtCur ) ;

    bool    operator < ( const CFileGpx & File ) const
                { return m_DistFrom < File.m_DistFrom ; } ;
    bool    operator > ( const CFileGpx & File ) const
                { return m_DistFrom > File.m_DistFrom ; } ;

    std::string             m_TrackName ;   ///< nom de la trace dans le fichier
    std::string             m_FileName ;    ///< nom du fichier
    std::vector<StPoint>    m_VecTrack ;    ///< trace de la rando

    float                   m_SlopeMax = 1. ;   ///< echelle du dessin
    StPoint                 m_Barycenter ;      ///< barycentre de la trace

private :
    void    TraiteLigne( char * Ligne ) ;
    float                   m_DistFrom ;    ///< distance à un point pour tri menu proche
} ;
