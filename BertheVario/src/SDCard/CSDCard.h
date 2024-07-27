////////////////////////////////////////////////////////////////////////////////
/// \file CSDCard.h
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 26/07/2024
///

#ifndef _SDCARD_
#define _SDCARD_

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de creer l'utilisation de la carte SD. Definit differentes pin
/// qui sont necessaire a l'utilisation des autres capteurs. A initialiser en premier.
class CSDCard
{
public :
    void InitSDCard() ;

    void ListeIgc( std::vector<std::string> & VecNomIgc , std::vector<int> & VecTempsIgc ) ;
    void DeleteIgc() ;

private :
} ;

#endif
