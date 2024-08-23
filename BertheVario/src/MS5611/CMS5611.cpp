////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611.cpp
///
/// \brief Fichier du capteur de pression
///
/// \date creation     : 07/03/2024
/// \date modification : 23/08/2024
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
g_MS5611.setCompensation( true ) ;

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
/// \brief Renvoie la pression en mb.
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
return CalcAltitude( GetPressureMb() * 100. ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui donne la VZ moyenne sur SECONDES_ALTI_VZ secondes.
/// de la pression/altitude capteur filtree.
void CMS5611::TacheMS5611Vz(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache Vz lancee");
#endif

delay( 1000 ) ;

const int SECONDES_ALTI_VZ = g_GlobalVar.m_Config.m_periode_integration_sec ;
float AltiPressForVzArr[SECONDES_ALTI_VZ] ;

// init des variables alti pression integree
g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 g_GlobalVar.m_MS5611.Read() ;
 g_GlobalVar.m_MS5611.m_AltiPressionFiltree = g_GlobalVar.m_MS5611.GetAltiPressionCapteurMetres() ;
 for ( int i = 0 ; i < SECONDES_ALTI_VZ ; i++ )
    AltiPressForVzArr[i] = g_GlobalVar.m_MS5611.m_AltiPressionFiltree ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ;

// alti pression filtree
float CoefFiltre = g_GlobalVar.m_Config.m_coef_filtre_alti_baro ;
int iboucle = 0 ;
while (g_GlobalVar.m_TaskArr[MS5611_NUM_TASK].m_Run)
    {
    g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     // mesures du capteur
     g_GlobalVar.m_MS5611.Read() ;
     float AltiMesCapteur = g_GlobalVar.m_MS5611.GetAltiPressionCapteurMetres()  ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;

     // filtrage alti pression
    float AltiPressionFiltree = g_GlobalVar.m_MS5611.m_AltiPressionFiltree ;
    AltiPressionFiltree = AltiPressionFiltree * CoefFiltre + (1.-CoefFiltre) * AltiMesCapteur ;
    g_GlobalVar.m_MS5611.m_AltiPressionFiltree = AltiPressionFiltree ;
    //Serial.print("altipress:") ;
    //Serial.println(g_GlobalVar.CMS5611::m_AltiPressionFiltree);

    // 5hz
    delay(200) ;

    // reboucle
    if ( (iboucle++)%5 )
        continue ;

    // 1 hz

    // decalage du tableau alti fifo par 0 sur x secondes
    for ( int i = SECONDES_ALTI_VZ - 1 ; i>0 ; i-- )
        AltiPressForVzArr[ i ] = AltiPressForVzArr[ i - 1 ] ;

    AltiPressForVzArr[ 0 ] = AltiPressionFiltree ;

    // calcul VZ sur x secondes
    float VitVert = AltiPressForVzArr[ 0 ] - AltiPressForVzArr[ SECONDES_ALTI_VZ - 1 ] ;
    VitVert /= SECONDES_ALTI_VZ ;
    #ifndef SIMU_VOL
     g_GlobalVar.m_VitVertMS = VitVert ;
    #endif
    }

g_GlobalVar.m_TaskArr[MS5611_NUM_TASK].m_Stopped = true ;
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
    m_DiffAltiFchAgl = 0. ; // si probleme de diff alti comme reboot en vol
    /*CGlobalVar::BeepError() ;
    g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     g_MS5611.reset() ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;
    InitMs5611() ;
    delay( 500 ) ;*/
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
/// \brief Lance la tache de calcul de Vz.
void CMS5611::LancerTacheCalculVz()
{
// tache de calcul Vz
xTaskCreatePinnedToCore(TacheMS5611Vz, "MS5611AltiTask", MS5611_STACK_SIZE, this, MS5611_PRIORITY,NULL, MS5611_CORE);
//delay(100) ;
}


