////////////////////////////////////////////////////////////////////////////////
/// \file CRandoVol.h
///
/// \brief
///
/// \date creation     : 13/01/2025
/// \date modification : 20/01/2025
///

#pragma once

#define GPX_DIR "/gpx"

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de suivit de fichier de trace GPX avec le gps.
class CRandoVol
{
public :
    enum EtatRando
        {
        InitRando ,
        InitAfficheMenu ,
        AfficheMenu ,
        InitTrace ,
        Navigation
        } ;

    void InitRandoVol() ;
    void LireFichiersGpx() ;
    void AfficheRandoVol() ;

    EtatRando m_EtatRando         = InitRando ; ///< pour le sequencement init/menu/rando
    bool      m_OrientationCapGps = false ;      ///< orientation carte de trace

    const char * GetTrackName( int i ) const ;

    std::vector<CFileGpx*>  m_VecGpx ;  ///< vecteur des traces Gpx

private :
    void LanceTacheRandoVol() ;
    void SortGpxFromCurPos() ;
} ;
