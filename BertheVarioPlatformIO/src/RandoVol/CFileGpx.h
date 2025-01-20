////////////////////////////////////////////////////////////////////////////////
/// \file CFileGpx.h
///
/// \brief
///
/// \date creation     : 14/01/2025
/// \date modification : 20/01/2025
///

#pragma once

#define SLOPE_MAX_DIV (200.)

////////////////////////////////////////////////////////////////////////////////
/// \brief Fichier Gpx de rando-vol. Avec son integration au GnuVario
class CFileGpx
{
public :
    ~CFileGpx() ;

    struct StPoint
        {
        float m_Lat = 0 ;
        float m_Lon = 0 ;
        float m_Alt = 0 ;
        } ;

    void    LireFichier(bool AvecAlt = false) ;
    void    SetDistanceFrom( const StPoint PtCur ) ;
    void    GetInfo( const StPoint PtCur , float & AltitudeRest , float & DistanceRest , float & AltitudeFait , float & DistanceFait ) ;

    bool    operator < ( const CFileGpx & File ) const
                { return m_DistFrom < File.m_DistFrom ; } ;
    bool    operator > ( const CFileGpx & File ) const
                { return m_DistFrom > File.m_DistFrom ; } ;

    std::string             m_TrackName ;   ///< nom de la trace dans le fichier
    std::string             m_FileName ;    ///< nom du fichier
    std::vector<StPoint> *  m_pVecTrack = NULL ;    ///< trace de la rando

    float       m_SlopeMax = 1. ;       ///< echelle du dessin
    StPoint     m_Barycenter ;          ///< barycentre de la trace
    float       m_DistFrom ;            ///< distance à un point pour tri menu proche
    bool        m_PtTerConnu = false ;  ///< point de terrain connu

private :
    void    TraiteLigne( char * Ligne , bool AvecAlt ) ;
    float   DistanceMetre( StPoint PtA , StPoint PtB ) const ;

} ;
