/*
 * tf_functions.h
 *
 *  Created on: 6 nov 2025
 *      Author: Melissa
 */

#ifndef MAIN_TF_FUNCTIONS_H_
#define MAIN_TF_FUNCTIONS_H_

#include <esp_system.h>
#include <nvs_flash.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_log.h"

#ifdef __cplusplus
extern "C" {
#endif

// DECLARACIÓN de las funciones de TFLite que serán llamadas desde C

void tf_setup(void);
void PreProcessImage(uint8_t* image_data, int image_len);
void GetModelPrediction(void);

#ifdef __cplusplus
}
#endif


#endif /* MAIN_TF_FUNCTIONS_H_ */
