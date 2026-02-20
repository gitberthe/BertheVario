////////////////////////////////////////////////////////////////////////////////
/// \file perfmon.cpp
///
/// \brief
///
/// \date creation     : 29/03/2024
/// \date modification : 29/03/2024
///

#include "esp32_perfmon.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "sdkconfig.h"

#include "esp_log.h"

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

//static const char *TAG = "perfmon";

static uint64_t idle0Calls = 0;
static uint64_t idle1Calls = 0;

#if defined(CONFIG_ESP32_DEFAULT_CPU_FREQ_240)
static const uint64_t MaxIdleCalls = 1855000;
#elif defined(CONFIG_ESP32_DEFAULT_CPU_FREQ_160)
static const uint64_t MaxIdleCalls = 1233100;
#else
#error "Unsupported CPU frequency"
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief fonction idle core 0.
static bool idle_task_0()
{
idle0Calls += 1;
return false;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief fonction idle core 1.
static bool idle_task_1()
{
idle1Calls += 1;
return false;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui met a jour les compteur d'utilisation de core 0 et 1
static void TachePerfMon(void *args)
{
idle0Calls = 0;
idle1Calls = 0;

while (g_GlobalVar.m_TaskArr[PERF_MON_NUM_TASK].m_Run)
	{
	float idle0 = idle0Calls;
	float idle1 = idle1Calls;
	idle0Calls = 0;
	idle1Calls = 0;

	int cpu0 = 100.f - ((float)idle0) / MaxIdleCalls * 100.f;
	int cpu1 = 100.f - ((float)idle1) / MaxIdleCalls * 100.f;

    g_GlobalVar.m_PercentCore0 = cpu0 ;
    g_GlobalVar.m_PercentCore1 = cpu1 ;

    /*Serial.print("cpu0:") ;
    Serial.println(cpu0) ;
    Serial.print("cpu1:") ;
    Serial.println(cpu1) ;*/
	//ESP_LOGI(TAG, "Core 0 at %d%%", cpu0);
	//ESP_LOGI(TAG, "Core 1 at %d%%", cpu1);
	// TODO configurable delay
	//vTaskDelay(900 / portTICK_PERIOD_MS);
	// delay mis a jour suivant programme vide pour core 1 a 0 et 50% et
	// core 0 toujous vers 75% a vide
	vTaskDelay(900 / portTICK_PERIOD_MS);
	}

esp_deregister_freertos_idle_hook_for_cpu(idle_task_0, 0) ;
esp_deregister_freertos_idle_hook_for_cpu(idle_task_1, 1) ;

DeleteTask( PERF_MON_NUM_TASK ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief fonction a appeler une fois
esp_err_t LancerTachePerfMon()
{
ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_task_0, 0));
ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_task_1, 1));

g_GlobalVar.m_TaskArr[PERF_MON_NUM_TASK].m_Run = true ;

// TODO calculate optimal stack size
xTaskCreatePinnedToCore(TachePerfMon, "PerfMonTask", PERF_MON_STACK_SIZE, NULL, PERF_MON_PRIORITY,
    GetTaskHandle(PERF_MON_NUM_TASK), PERF_MON_CORE );

return ESP_OK;
}
