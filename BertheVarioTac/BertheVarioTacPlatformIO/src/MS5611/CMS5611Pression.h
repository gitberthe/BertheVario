////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611Pression.h
///
/// \brief Fichier du capteur de pression
///
/// \date creation     : 07/03/2024
/// \date modification : 30/01/2025
///

#ifndef _CMS5611_
#define _CMS5611_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe pour capteur de pression
class CVirtCaptPress
{
public :

    virtual void  InitCapteur() = 0 ;
    virtual void  MesureAltitudeCapteur() = 0 ;
    virtual float GetTemperatureDegres() = 0 ;
    virtual float GetPressionhPa() const = 0 ;

    virtual float GetAltiRecMetres() ;

    void    SetAltiSolMetres( float AltitudeSolHgt ) ;
    float   GetAltiBaroPureMetres() const
                { return m_AltitudeBaroPure ; } ;
    void    SetAltiSolUndef() ;

protected :
    CMutex  m_Mutex ;
    float   m_AltitudeBaroPure ;            ///< altitude baro pure du capteur
    float   m_DiffAltiBaroHauteurSol = 0 ;  ///< difference alti baro avec la hauteur sol (recalee avant décollage)
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Capteur de pression et de temperature.
/// Precision du capteur 10cm.
/// Attention à la frequence du bus I2C CGlobalVar::InitI2C().
class CMS5611Pression : public CVirtCaptPress
{
public :

    void InitCapteur()
            { InitMs5611() ; } ;
    void InitMs5611() ;

    void MesureAltitudeCapteur() ;

    float GetAltiRecMetres() override ;
    float GetTemperatureDegres() override ;
    float GetPressionhPa() const override ;

private :
    void  Read() ;
    float GetPressureMb() ;
    float GetAltiPressionCapteurMetres() ;

    float CalcAltitude(float pressure_mb_x100 , float seaLevelPressure = 101325 ) ;
} ;

#endif
