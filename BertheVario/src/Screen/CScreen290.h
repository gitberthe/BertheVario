////////////////////////////////////////////////////////////////////////////////
/// \file CScreen290.h
///
/// \brief
///
/// \date creation     : 15/03/2024
/// \date modification : 08/08/2024
///

#ifndef _SCREEN290_
#define _SCREEN290_

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet l'affichage de differentes pages d'ecrans et messages.
/// Les includes dans CSreen.cpp permetent de selectionner les differents ecrans materiel.
class CScreen290 : public CGestEcrans
{
public :
    void InitScreen() ;
    void AfficheVoltage() ;
    void AfficheConnectWifi() ;
    void AfficheWifi(char * IpAdress) ;
    void AfficheCalibreMag() ;
    void ScreenRaz() ;
    void ScreenOff() ;
    void DoRect( int x , int y , int w , int h , bool Black ) ;
    void DoChar( int x , int y , const char * pChar ) ;

    EtatsAuto Ecran0Vz() ;
    EtatsAuto Ecran1Histo() ;
    EtatsAuto Ecran2aTmaAll() ;
    EtatsAuto Ecran2bTmaMod() ;
    EtatsAuto Ecran3Sys() ;
    EtatsAuto Ecran4CfgFch() ;
    EtatsAuto Ecran5alisteIgcFch() ;
    EtatsAuto Ecran5bConfimeArchIgcFch() ;

private :
} ;

#endif
