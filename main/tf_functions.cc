/*
 * tf_functions.cc
 *
 *  Created on: 6 nov 2025
 *      Author: Melissa
 */


#include "tf_functions.h"
#include "model.h"

namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 400 * 1024;                                    // ----------- MODIFICACION AQUI
//FORZAR la ubicación en la PSRAM (Memoria externa)
    uint8_t tensor_arena[kTensorArenaSize] __attribute__ ((section(".psram.bss")));
}  // namespace

void tf_setup(void)
{
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_model);
  
  if (!model) {
    ESP_LOGE("IA", "GetModel(g_model) devolvio NULL. ¿g_model incluido?");
    return;
  }
  ESP_LOGI("IA", "Model pointer: %p", model);

  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf("Model provided is schema version %d not equal to supported "
                "version %d.", model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  static tflite::MicroMutableOpResolver<6> resolver;

	if (resolver.AddConv2D() != kTfLiteOk) { ESP_LOGE("IA","AddConv2D failed"); return; }
	if (resolver.AddMaxPool2D() != kTfLiteOk) { ESP_LOGE("IA","AddMaxPool2D failed"); return; }
	if (resolver.AddAveragePool2D() != kTfLiteOk) { ESP_LOGE("IA","AddAveragePool2D failed"); return; }
	if (resolver.AddFullyConnected() != kTfLiteOk) { ESP_LOGE("IA","AddFullyConnected failed"); return; }
	if (resolver.AddQuantize() != kTfLiteOk) { ESP_LOGE("IA","AddQuantize failed"); return; }
	if (resolver.AddMean() != kTfLiteOk) { ESP_LOGE("IA","AddMean failed"); return; }

 	 // Build an interpreter to run the model with.
  	static tflite::MicroInterpreter static_interpreter(
    	  model, resolver, tensor_arena, kTensorArenaSize);
  	interpreter = &static_interpreter;


	 if (!interpreter) {
	    ESP_LOGE("IA","No se pudo crear interpreter");
	    return;
	  }
	  
  	// Allocate memory from the tensor_arena for the model's tensors.
  	TfLiteStatus allocate_status = interpreter->AllocateTensors();
  	if (allocate_status != kTfLiteOk) {
    	ESP_LOGE("IA", "AllocateTensors() fallo: %d. Revisa ops y tamanio de arena (%d bytes).", allocate_status, kTensorArenaSize);
    	return;
	}
	
	// Obtener punteros a tensores de entrada y salida
	input = interpreter->input(0);
	output = interpreter->output(0);

	if (!input) { ESP_LOGE("IA", "input() devolvio NULL"); return; }
 	 if (!output) { ESP_LOGE("IA", "output() devolvio NULL"); return; }


	ESP_LOGI("IA", "Model and Config OK");
}


// ------------------------------------------------------------------------------------------------
// FUNCIÓN DE PRE-PROCESAMIENTO Y CUANTIFICACIÓN
// ------------------------------------------------------------------------------------------------
void PreProcessImage(uint8_t* image_data, int image_len) {
    
    // 1. Obtener los parámetros de cuantificación del tensor de entrada
	if (input == nullptr) { // Verificar si el puntero es nulo
        ESP_LOGE("IA", "TFLite no inicializado. Omitiendo inferencia.");
        return; 
    }
    
	float scale = input->params.scale;
    int32_t zero_point = input->params.zero_point;
    
    // Verificar que la longitud de la imagen sea la esperada
    // 96 * 96 * 1 canal (escala de grises) = 9216
    if (image_len != 9216 || input->bytes != 9216) {
        MicroPrintf("Error de tamano: imagen (%d) vs tensor (%d)", image_len, input->bytes);
        return;
    }

    // 2. Iterar sobre todos los píxeles (96 x 96 = 9216 píxeles)
    for (int i = 0; i < image_len; i++) {
        
        // El valor real es el byte de escala de grises (0-255)
        float real_value = static_cast<float>(image_data[i]);
        
        // Aplicar la fórmula de cuantificación (simétrica)
        // La mayoria de los modelos de vision simetricos tienen un zero_point de 0.
        // Formula: q = round(r / scale) + zero_point
        int32_t quantized_value = static_cast<int32_t>(roundf(real_value / scale) + zero_point);

        // Limitar el valor al rango de int8_t (-128 a 127)
        if (quantized_value > 127) quantized_value = 127;
        if (quantized_value < -128) quantized_value = -128;

        // 3. Escribir el valor cuantizado en el tensor de entrada (int8_t)
        input->data.int8[i] = static_cast<int8_t>(quantized_value);
    }
    ESP_LOGI("IA", "Preprocess OK");
}

void GetModelPrediction(void)
{
	if (!interpreter) {
        MicroPrintf("Error: Interprete no inicializado.");
        return;
    }
    
    // 1. Ejecutar la inferencia
    TfLiteStatus invoke_status = interpreter->Invoke();
    
    if (invoke_status != kTfLiteOk) {
        MicroPrintf("Error: Invoke fallo.");
        return;
    }
    ESP_LOGI("IA", "Inferencia OK");
    
    // 2. Obtiene la salida
    int8_t output_inf = output->data.int8[0];
    
  
    ESP_LOGI("IA", "Output OK    |  output quant : %d",  output_inf);
    
    // 4. Accion Output
    if(output_inf > 0){
    	printf("\nSin Cubrebocas   |  Output : %d\n", output_inf);
    } else {
		printf("\nCon Cubrebocas   |  Output : %d\n", output_inf);
    }
}