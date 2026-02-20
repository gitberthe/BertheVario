////////////////////////////////////////////////////////////////////////////////
/// \file CSimuVol.cpp
///
/// \brief
///
/// \date creation     : 30/03/2024
/// \date modification : 13/01/2025
///

#include "../BertheVario213.h"

#ifdef SIMU_VOL

CSimuVol g_SimuVol ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de simulation de vol.
void CSimuVol::LancerTacheSimuVol()
{
xTaskCreatePinnedToCore(TacheSimuVol, "SimuVolTask", SIMUVOL_STACK_SIZE, this, SIMUVOL_PRIORITY,
    GetTaskHandle(SIMUVOL_NUM_TASK), SIMUVOL_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tache de simulation de vol, modification des variables a 2hz.
/// Autour du puy de dome.
void CSimuVol::TacheSimuVol(void* param)
{
// stabilisation GPS
g_GlobalVar.m_Config.m_vitesse_igc_kmh = 4 ;
g_GlobalVar.m_Config.m_XYMargin = 400 ;
g_GlobalVar.m_Config.m_AltiMargin = 100 ;

float LatAngle = 0 ;
float LonAngle = 0 ;
float Dist = 2500. ;
float AltiAngle = T_PI/2. + T_PI/10 ; // T_PI/2. * 0.6 ;

// pdd
float LastLat = 45.773785 ;
float LastLon = 2.96898  ;
float LastAlti = 0. ;

// week end periode
g_GlobalVar.m_Annee = 2024 ;
g_GlobalVar.m_Mois = 3 ;
g_GlobalVar.m_Jour = 31 ;
g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = 3500 ;

g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;

delay( 3000 ) ;

// gps ok
g_GlobalVar.m_MillisPremierGGA = millis() ;

srand(time(NULL)) ;

while( g_GlobalVar.m_TaskArr[SIMUVOL_NUM_TASK].m_Run )
    {
    delay( 500 ) ;

    // heure
    unsigned long heure = millis() ;

    heure /= 1000 ;
    heure += 10 * 3600 ;

    g_GlobalVar.m_HeureSec = heure ;

    // evolution variables
    AltiAngle -= T_PI / 2000. ;
    float alti = 2550  + 1000 * sinf( AltiAngle ) ;

    // simulation scotche par le vent
    if ( alti < 450 )
        continue ;

    // evolution variables
    LatAngle += T_PI / (320. * 3 /*+ rand()%800 */) ;
    LonAngle += T_PI / (400. * 3 /*+ rand()%1000*/ ) ;
    Dist     += 0.5 ; //(rand()%500)/100. - 2 ;

    g_GlobalVar.m_MutexLatLonGps.PrendreMutex() ;

    // pdd
    g_GlobalVar.m_TerrainPosCur.m_Lat  = 45.773785 + Dist / UnMileEnMetres / 60. * cosf( LatAngle ) ;
    g_GlobalVar.m_TerrainPosCur.m_Lon  = 2.96898   + Dist / UnMileEnMetres / 60. * cosf( LonAngle )  ;
    g_GlobalVar.m_AltiGps =
    g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = alti ;
    g_GlobalVar.m_VitVertMS = (g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec-LastAlti) * 2 ;

    float DeltaLat = g_GlobalVar.m_TerrainPosCur.m_Lat - LastLat ;
    float DeltaLon = g_GlobalVar.m_TerrainPosCur.m_Lon - LastLon ;
    g_GlobalVar.m_VitesseKmh = sqrtf( powf(DeltaLat,2) + powf(DeltaLon,2) ) * UnMileEnMetres * 60 * 2 * 3.6;
    g_GlobalVar.m_CapGpsDeg = 180. / T_PI * atan2f( DeltaLon , DeltaLat ) + 180. ;

    // latitude
    float Lat = fabsf(g_GlobalVar.m_TerrainPosCur.m_Lat) ;
    g_GlobalVar.m_LatChar[0] = (int)(Lat / 10) + '0' ;
    g_GlobalVar.m_LatChar[1] = ((int)Lat % 10) + '0' ;
    float Minutes = fmodf( Lat*60 , 60. ) ;
    g_GlobalVar.m_LatChar[2] = (int)(Minutes/10) + '0' ;
    g_GlobalVar.m_LatChar[3] = ((int)Minutes%10) + '0' ;

    g_GlobalVar.m_LatChar[4] = ((int)(Minutes*10)%10) + '0' ;
    g_GlobalVar.m_LatChar[5] = ((int)(Minutes*100)%10) + '0' ;
    g_GlobalVar.m_LatChar[6] = ((int)(Minutes*1000)%10) + '0' ;
    g_GlobalVar.m_LatChar[7] = 0 ;

    // longitude
    float Lon = fabsf(g_GlobalVar.m_TerrainPosCur.m_Lon) ;
    g_GlobalVar.m_LonChar[0] = (int)(Lon/100) + '0' ;
    g_GlobalVar.m_LonChar[1] = (int)((int)(Lon/10)%10) + '0' ;
    g_GlobalVar.m_LonChar[2] = (int)((int)Lon%10) + '0' ;
    Minutes = fmodf( Lon*60 , 60. ) ;
    g_GlobalVar.m_LonChar[3] = ((int)Minutes/10) + '0' ;

    g_GlobalVar.m_LonChar[4] = ((int)Minutes%10) + '0' ;
    g_GlobalVar.m_LonChar[5] = ((int)(Minutes*10)%10) + '0' ;
    g_GlobalVar.m_LonChar[6] = ((int)(Minutes*100)%10) + '0' ;
    g_GlobalVar.m_LonChar[7] = ((int)(Minutes*1000)%10) + '0' ;
    g_GlobalVar.m_LonChar[8] = 0 ;

    g_GlobalVar.m_MutexLatLonGps.RelacherMutex() ;

    // last variables
    LastLat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
    LastLon = g_GlobalVar.m_TerrainPosCur.m_Lon ;
    LastAlti= g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;
    }

DeleteTask(SIMUVOL_NUM_TASK) ;
}

#endif
