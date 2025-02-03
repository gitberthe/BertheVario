////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611.cpp
///
/// \brief Fichier du capteur de pression
///
/// \date creation     : 07/03/2024
/// \date modification : 18/01/2025
///

#include "../BertheVario.h"

MS5611 g_MS5611(0x77);

////////////////////////////////////////////////////////////////////////////////
/// \brief Voir aussi CGlobalVar::InitI2C() pour une frequence I2C qui ne plante pas.
void CMS5611::InitMs5611()
{
g_GlobalVar.m_MutexI2c.PrendreMutex() ;

#ifdef MS5611_DEBUG
    Serial.println("Initialize MS5611 Sensor");
#endif //MS5611_DEBUG

// init MS5611
if (g_MS5611.begin() == true)
    {
    #ifdef MS5611_DEBUG
    Serial.println("MS5611 found.");
    #endif
    }
else
    {
    //CGlobalVar::BeepError() ;
    #ifdef MS5611_DEBUG
     Serial.println("MS5611 not found. halt.");
    #endif
   }

// meilleur resolution de mb
g_MS5611.setOversampling( OSR_ULTRA_HIGH ) ;
//g_MS5611.setOversampling( OSR_HIGH ) ; // bug altitude negative non resolue
g_MS5611.setCompensation( true ) ; // bug altitude negative non evite si on enleve

g_MS5611.setTemperatureOffset( -1. ) ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul l'altitude en fonction de la pression sans compensation de
/// temperature.
float CMS5611::CalcAltitude(float pressure_mb_x100 , float seaLevelPressure)
{
return (44330.0f * (1.0f - powf((float)pressure_mb_x100 / (float)seaLevelPressure, 0.1902949f)));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des info du capteur
void CMS5611::Read()
{
g_MS5611.read() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la pression en mb ou hPa.
float CMS5611::GetPressureMb()
{
return g_MS5611.getPressure() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la temperature en degres celsius
float CMS5611::GetTemperatureDegres()
{
return g_MS5611.getTemperature() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la mesure d'altitude pression directe du capteur
float CMS5611::GetAltiPressionCapteurMetres()
{
float Alti = CalcAltitude( GetPressureMb() * 100. ) ;
// bug capteur alti tres basse non evite avec ça
if ( Alti < -500 )
    Alti = -500 ;
return Alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui donne la VZ de la pression/altitude capteur filtree
/// a 5hz.
/// Met a jour aussi le cap magnetique.
/// Modifier le fichier ./Projects/BertheVario/.pio/libdeps/esp32dev/MPU9250/MPU9250.h,
/// ligne 85, static constexpr uint8_t MAG_MODE {0x02}; pour une lecture basse frequence
/// du capteur (mais proche de 8hz).
void CMS5611::TacheVzCapMag(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache Vz lancee");
#endif

delay( 1000 ) ;

int count_div_mag = 0 ;
const int DIV_MAG = 3 ;
const int DIV_SECONDES = 3 ;
float AltiPressForVzArr[DIV_SECONDES+1] ;

// init des variables alti pression integree
g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 g_GlobalVar.m_MS5611.Read() ;
 //delay( 5 ) ;   // delais pour bug altitude tres negative ???
 g_GlobalVar.m_MS5611.m_AltiPressionFiltree = g_GlobalVar.m_MS5611.GetAltiPressionCapteurMetres() ;
 for ( int i = 0 ; i <= DIV_SECONDES ; i++ )
    AltiPressForVzArr[i] = g_GlobalVar.m_MS5611.m_AltiPressionFiltree ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ;

// alti pression filtree
while (g_GlobalVar.m_TaskArr[VZ_MAG_NUM_TASK].m_Run)
    {
    // mesure du cap magnetique a 9 hz ou plantage
    g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     g_GlobalVar.m_Mpu9250.Update() ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;

    // 9hz
    delay(1000/(DIV_SECONDES*DIV_MAG)) ;

    // 3hz
    if ( (count_div_mag++)%DIV_MAG )
        continue ;

    // mesures du capteur de pression
    g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     g_GlobalVar.m_MS5611.Read() ;
     //delay( 5 ) ;   // delais pour bug altitude tres negative ???
     float AltiMesCapteur = g_GlobalVar.m_MS5611.GetAltiPressionCapteurMetres()  ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;

    // filtrage alti pression
    const float CoefFiltre = g_GlobalVar.m_Config.m_coef_filtre_alti_baro ;
    float AltiPressionFiltree = g_GlobalVar.m_MS5611.m_AltiPressionFiltree ;
    AltiPressionFiltree = AltiPressionFiltree * CoefFiltre + (1.-CoefFiltre) * AltiMesCapteur ;
    g_GlobalVar.m_MS5611.m_AltiPressionFiltree = AltiPressionFiltree ;
    //Serial.print("altipress:") ;
    //Serial.println(g_GlobalVar.CMS5611::m_AltiPressionFiltree);

    // decalage du tableau alti fifo par 0 sur x secondes
    for ( int i = DIV_SECONDES ; i>0 ; i-- )
        AltiPressForVzArr[ i ] = AltiPressForVzArr[ i - 1 ] ;

    AltiPressForVzArr[ 0 ] = AltiPressionFiltree ;

    // calcul VZ sur x secondes
    float VitVert = AltiPressForVzArr[ 0 ] - AltiPressForVzArr[ DIV_SECONDES ] ;
    //g_GlobalVar.m_MutexI2c.RelacherMutex() ;    // Vz fausse et declenchement de faux vols non resolus
    #ifndef SIMU_VOL
     g_GlobalVar.m_VitVertMS = VitVert ;
    #endif
    }

g_GlobalVar.m_TaskArr[VZ_MAG_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'alti pression filtree recalee alti sol en debut de stabilisation gps
float CMS5611::GetAltiMetres()
{
float Alti = m_AltiPressionFiltree + m_DiffAltiFchAgl ;

// pour parer a un probleme
if ( Alti > 9999. || isnan(Alti) || Alti < -500. )
    {
    Alti = 9999. ;
    //m_DiffAltiFchAgl = 0. ; // si probleme de diff alti comme reboot en vol (impossible)
                              // supprime le 13/01/2025 cause faux depart de vol Vz
    //CGlobalVar::BeepError() ;
    g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     g_MS5611.reset() ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;
    InitMs5611() ;
    }
return Alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne la difference avec l'alti sol.
void CMS5611::SetAltiSolMetres( float AltiSol )
{
g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 Read() ;
 float AltiMesCapteur = GetAltiPressionCapteurMetres()  ;
 m_DiffAltiFchAgl = AltiSol - AltiMesCapteur ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ;

g_GlobalVar.m_TerrainPosCur.m_AltiBaro = GetAltiMetres() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de calcul de Vz et acquisition cap magnetique.
void CMS5611::LancerTacheCalculVzCapMag()
{
// tache de calcul Vz
xTaskCreatePinnedToCore(TacheVzCapMag, "MS5611AltiTaskEtMag", VZ_MAG_STACK_SIZE, this, VZ_MAG_PRIORITY,NULL, VZ_MAG_CORE);
}


