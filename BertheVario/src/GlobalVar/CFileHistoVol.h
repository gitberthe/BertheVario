////////////////////////////////////////////////////////////////////////////////
/// \file CFileHistoVol.h
///
/// \brief
///
/// \date creation     : 22/03/2024
/// \date modification : 07/04/2024
///

#ifndef _FILE_HISTO_
#define _FILE_HISTO_

#define NOM_FCH_HISTO_VOL  "/config/histovol.txt"

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe d'historique du dernier vol sur fichier. Calcul aussi la finesse
/// sol.
class CFileHistoVol
{
public :
    int   m_ZMax  = 0 ;         ///< altitude max
    int   m_ZDeco  = 0 ;        ///< altitude deco
    float m_VzMax = -10. ;      ///< Vz Max
    float m_VzMin =  10. ;      ///< Vz Min
    float m_VsMax = 0. ;        ///< Vs Max
    float m_DistanceKm = 0. ;   ///< distance
    int   m_TempsDeVol = 0. ;   ///< temps de vol

    float m_LastLat = 0. ;  ///< pour calcul de distance
    float m_LastLon  = 0. ; ///< pour calcul de distance

    void LectureFichier() ;
    void EcritureFichier() ;

    float PushHistoVol() ;

private :
    File  m_File ;  ///< lecture ecriture fichier histo
} ;

#endif
