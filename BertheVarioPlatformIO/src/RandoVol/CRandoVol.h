////////////////////////////////////////////////////////////////////////////////
/// \file CRandoVol.h
///
/// \brief
///
/// \date creation     : 13/01/2025
/// \date modification : 14/01/2025
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
        Navigation
        } ;

    void InitRandoVol() ;
    void LireFichiersGpx() ;
    void AfficheRandoVol() ;

    EtatRando m_EtatRando = InitRando ;

    const char * GetTrackName( int i ) const ;

private :
    void LanceTacheRandoVol() ;
    void SortGpxFromCurPos() ;

    std::vector<CFileGpx*>  m_VecGpx ;  ///< vecteur des Gpx
} ;
