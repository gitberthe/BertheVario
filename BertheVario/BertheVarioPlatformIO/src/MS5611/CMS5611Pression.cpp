////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611Pression.cpp
///
/// \brief Fichier du capteur de pression
///
/// \date creation   : 07/03/2024
/// \date 17/10/2025 : modification mutex
/// \date 18/10/2025 : modification millis() dans TacheVzCapMag()
/// \date 05/01/2026 : mesure capteur dans SetAltiSolMetres()
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

MS5611 g_MS5611(0x77);

// Store the current sea level pressure at your location in Pascals.
const float seaLevelPressure = 101325;

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine la difference du jour entre l'altibaro pure et la hauteur
/// sol des fichier hgt
void CVirtCaptPress::SetAltiSolMetres( float AltitudeSolHgt )
{
MesureAltitudeCapteur() ;
m_DiffAltiBaroHauteurSol = AltitudeSolHgt - m_AltitudeBaroPure ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief En cas de debut de vol sans stabilisation gps, altibaro == alti recalée
void CVirtCaptPress::SetAltiSolUndef()
{
m_DiffAltiBaroHauteurSol = 0 ;
}

/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief Voir aussi CGlobalVar::InitI2C() pour une frequence I2C qui ne plante pas.
void CMS5611Pression::InitMs5611()
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

g_MS5611.setTemperatureOffset( -1.6 ) ;

g_GlobalVar.m_MutexI2c.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul l'altitude en fonction de la pression sans compensation de
/// temperature.
float CMS5611Pression::CalcAltitude(float pressure_mb_x100 , float seaLevelPressure)
{
return (44330.0f * (1.0f - powf((float)pressure_mb_x100 / (float)seaLevelPressure, 0.1902949f)));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des info du capteur
void CMS5611Pression::Read()
{
g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 g_MS5611.read() ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la pression en hPa.
float CMS5611Pression::GetPressionhPa() const
{
return g_MS5611.getPressure() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la pression en mb.
float CMS5611Pression::GetPressureMb()
{
return g_MS5611.getPressure() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la temperature en degres celsius
float CMS5611Pression::GetTemperatureDegres()
{
return g_MS5611.getTemperature() ;
}

/*
////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la mesure d'altitude pression directe du capteur
float CMS5611Pression::GetAltiPressionCapteurMetres()
{
float ret = CalcAltitude( GetPressureMb() * 100. ) ;
 #ifdef MS5611_DEBUG
  Serial.print( "alti pression : " ) ;
  Serial.println( ret ) ;
 #endif
return ret ;
}*/

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'alti pression filtree recalee alti sol en debut de stabilisation gps
float CMS5611Pression::GetAltiRecMetres()
{
float Alti = m_AltitudeBaroPure + m_DiffAltiBaroHauteurSol ;
#ifdef MS5611_DEBUG
 Serial.print( "baro pure" ) ;
 Serial.println( m_AltitudeBaroPure ) ;
#endif

// pour parer a un probleme
if ( Alti > 9999. || isnan(Alti) || Alti < -500. )
    {
    Alti = 9999. ;
    //SetAltiSolUndef() ; // si probleme de diff alti comme reboot en vol
                          // supprime le 13/01/2025 cause faux depart de vol Vz
    //CGlobalVar::BeepError() ;
    g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     g_MS5611.reset() ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;
    InitMs5611() ;
    //delay( 500 ) ;
    }
return Alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lit l'altitude capteur
void CMS5611Pression::MesureAltitudeCapteur()
{
Read() ;
m_AltitudeBaroPure = CalcAltitude( GetPressureMb() * 100. ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de calcul de Vz et acquisition cap magnetique.
void CMS5611Pression::LancerTacheCalculVzCapMag()
{
// tache de calcul Vz
xTaskCreatePinnedToCore(TacheVzCapMag, "MS5611AltiTaskEtMag", VZ_MAG_STACK_SIZE, this, VZ_MAG_PRIORITY,
    GetTaskHandle(VZ_MAG_NUM_TASK), VZ_MAG_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui donne la VZ de la pression/altitude capteur filtree
/// a 3hz.
/// Met a jour aussi le cap magnetique à une frequence plus haute pour ne pas bloquer le
/// capteur.
/// Modifier le fichier ./Projects/BertheVario/.pio/libdeps/esp32dev/MPU9250/MPU9250.h,
/// ligne 85, static constexpr uint8_t MAG_MODE {0x02}; pour une lecture basse frequence
/// du capteur (mais proche de 8hz).
void CMS5611Pression::TacheVzCapMag(void *param)
{
#ifdef G_DEBUG
 Serial.println("tache Vz lancee");
#endif

const int DIV_SECONDES = 3 ;
float AltiPressForVzArr[DIV_SECONDES+1] ;

// init des variables alti pression integree
g_GlobalVar.m_MS5611Press.MesureAltitudeCapteur() ;
float AltiBaroFiltre = g_GlobalVar.m_MS5611Press.GetAltiBaroPureMetres() ;
for ( int i = 0 ; i <= DIV_SECONDES ; i++ )
    AltiPressForVzArr[i] = AltiBaroFiltre ;

// alti pression filtree
unsigned long time = millis() ;
while (g_GlobalVar.m_TaskArr[VZ_MAG_NUM_TASK].m_Run)
    {
    // lecture cap magnetique
    g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     #ifdef _BERTHE_VARIO_
      g_GlobalVar.m_Mpu9250.Update() ;
     #endif
     #ifdef _BERTHE_VARIO_213_
      g_GlobalVar.m_QMC5883Mag.LectureCap() ;
     #endif
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;

    // 3hz
    if ( (millis()-time) < (1000./DIV_SECONDES) )
        {
        delay( 100 ) ;
        continue ;
        }
    time = millis() ;

    // mesures du capteur de pression
    g_GlobalVar.m_MS5611Press.MesureAltitudeCapteur() ;
    float AltiBaroPure = g_GlobalVar.m_MS5611Press.GetAltiBaroPureMetres()  ;

    // filtrage alti pression
    const float CoefFiltre = g_GlobalVar.m_Config.m_coef_filtre_alti_baro ;
    AltiBaroFiltre = AltiBaroFiltre * CoefFiltre + (1.-CoefFiltre) * AltiBaroPure ;

    // decalage du tableau alti fifo par 0 sur 1 secondes
    for ( int i = DIV_SECONDES ; i>0 ; i-- )
        AltiPressForVzArr[ i ] = AltiPressForVzArr[ i - 1 ] ;

    AltiPressForVzArr[ 0 ] = AltiBaroFiltre ;

    // calcul VZ sur 1 secondes
    float VitVert = AltiPressForVzArr[ 0 ] - AltiPressForVzArr[ DIV_SECONDES ] ;
    #ifndef SIMU_VOL
     g_GlobalVar.m_VitVertMS = VitVert ;
    #endif
    }

// fin de tache
DeleteTask(VZ_MAG_NUM_TASK) ;
}


