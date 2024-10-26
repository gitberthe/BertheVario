////////////////////////////////////////////////////////////////////////////////
/// \file esp32-perfmon.h
///
/// \brief Include global du projet
///
/// \date creation     : 29/03/2024
/// \date modification : 29/03/2024
///


#ifndef COMPONENTS_PERFMON_INCLUDE_PERFMON_H_
#define COMPONENTS_PERFMON_INCLUDE_PERFMON_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

esp_err_t perfmon_start();

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_PERFMON_INCLUDE_PERFMON_H_ */
