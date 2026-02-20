////////////////////////////////////////////////////////////////////////////////
/// \file CIgc.h
///
/// \brief
///
/// \date creation   : 14/03/2024
/// \date 01/01/2026 : buffer d'ecriture igc toutes les 15 secondes
///

#ifndef _IGC_CLASS_
#define _IGC_CLASS_

#define TAILLE_MESSAGE_COURANT (38)
#define MARGE_BUFFER        (2)
#define TAILLE_BUFFER_FCH   (TAILLE_MESSAGE_COURANT*(15+MARGE_BUFFER)) ///< 38 octets pour 15 secondes

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet d'ecrire un fichier IGC. L'altitude prise en compte sur les sites
/// web est l'altibarometrique (recalee gps au debut du vol) tres precise (moins d'1m).
/// l'alti gps n'est precise qu'au debut du vol. Pendant le vol elle derive de 50m.
class CIgc
{
public :

    void InitCurentIgc() ;
    void PushLoc2Igc(bool write=false) ;
    const char * GetIgcFileName() const
        { return m_NomFchCourant ; } ;

private :
    char m_BufferEcriture[TAILLE_BUFFER_FCH] ;  ///< buffer carte sd
    char * m_pBuff = NULL ;                     ///< pointeur dans le buffer
    char m_NomFchCourant[50] ;  ///< nom du fichier courant igc
    File m_File ;               ///< fichier igc
} ;

#endif

