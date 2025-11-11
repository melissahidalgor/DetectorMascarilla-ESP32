/*
 * webserver.c
 *
 *  Created on: 5 nov 2025
 *      Author: Melissa
 */

#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_camera.h"
#include "webserver.h" // Incluimos la cabecera que acabamos de crear

#include "tf_functions.h"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// ----------------------------------- Codigo HTML -------------------------------------------------
const char INDEX_HTML[] = 
	"<html>\n"
	"<head><title>XIAO ESP32S3</title></head>\n"
	"<body>\n"
	"  <h1>Detector de Cubrebocas</h1>\n"
	"  <div>\n"
	"    <img id=\"video_feed\" src=\"/stream\" style=\"width: 320px; border: 1px solid black;\">\n"
	"    <br><br>\n"
	"    <button onclick=\"toggleMode()\">Tomar Foto/ Reanudar Stream</button>\n"
	"    <p id=\"ia_result\">Modo actual: STREAMING</p>\n"
	"  <div>\n"
	"    <script>\n"
	"        const videoElement = document.getElementById('video_feed');\n"
	"        let isStreaming = true;\n"
	"        const streamUrl = \"/stream\";\n"
	"        const captureUrl = \"/single_capture\";\n"
	"\n"
	"        async function toggleMode() {\n"
	"            if (isStreaming) {\n"
	"                // 1. MODO FOTO\n"
	"                document.getElementById('ia_result').innerHTML = \"Procesando foto...\";\n"
	"                videoElement.src = 'about:blank'; // Pausar visualmente el stream\n"
	"\n"
	"                try {\n"
	"                    // Llamar a la ruta que devuelve la imagen JPEG\n"
	"                    const response = await fetch(captureUrl);\n"
	"                    if (!response.ok) throw new Error('Capture failed');\n"
	"\n"
	"                    // Crear un Blob de la imagen devuelta y usarlo como fuente\n"
	"                    const imageBlob = await response.blob();\n"
	"                    const imageUrl = URL.createObjectURL(imageBlob);\n"
	"                    \n"
	"                    videoElement.src = imageUrl;\n"
	"                    document.getElementById('ia_result').innerHTML = \"Pulse de nuevo para stream.\";\n"
	"                    isStreaming = false; // Cambiar estado a estático\n"
	"                \n"
	"                } catch (error) {\n"
	"                    console.error('Error:', error);\n"
	"                    document.getElementById('ia_result').innerHTML = \"Error al tomar foto: \" + error.message;\n"
	"                    videoElement.src = streamUrl; // Volver al stream en caso de error\n"
	"                    isStreaming = true; \n"
	"                }\n"
	"            } else {\n"
	"                // 2. REANUDAR STREAM\n"
	"                videoElement.src = streamUrl;\n"
	"                document.getElementById('ia_result').innerHTML = \"Modo actual: Streaming\";\n"
	"                isStreaming = true;\n"
	"            }\n"
	"        }\n"
	"    </script>\n"
	"</body>\n"
	"</html>\n";
	
// --------------------------------------------------- Handler principal (Ruta: /) -----------------------------------------------------
esp_err_t index_handler(httpd_req_t *req){
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

// --------------------------------------------------- Handler para el stream continuo (Ruta: /stream) --------------------------------------
esp_err_t jpg_stream_httpd_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char part_buf[64];
    
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    while(true){
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE("Webserver", "Camera capture failed");
            return ESP_FAIL;
        }
                
        // Conversión obligatoria si no se captura en JPEG (Escala de Grises)
        if (!frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len)) {
            ESP_LOGE("Webserver", "JPEG compression failed");
            res = ESP_FAIL;
        }
        esp_camera_fb_return(fb); // Devolver inmediatamente después de la conversión
       
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(res == ESP_OK){
			size_t hlen = snprintf(part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        
        free(_jpg_buf); // Liberar el búfer temporal de JPEG después de enviarlo

        if(res != ESP_OK) break;
    }
    return res;
}

// ------------------------------ Handler Captura de Foto Única (Ruta: /single_capture) --------------------------
esp_err_t single_capture_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    uint8_t *jpg_buf = NULL;
    size_t jpg_buf_len = 0;
    
    // Capturar el frame (96x96 GRIS)
    fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE("Foto", "Single capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
    // **PUNTO CLAVE DE PROCESAMIENTO IA:** fb->buf es 96x96x1
    ESP_LOGI("Foto", "Running IA Model (96x96x1)");
    PreProcessImage(fb->buf, fb->len);
    GetModelPrediction();
    
    // Convertir a JPEG (para que el navegador pueda mostrar la imagen estática)
    if (!frame2jpg(fb, 80, &jpg_buf, &jpg_buf_len)) {
        ESP_LOGE("Foto", "JPEG compression failed for IA pic");
        esp_camera_fb_return(fb); 
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    esp_camera_fb_return(fb); 

    // Devolver la imagen única
    httpd_resp_set_type(req, "image/jpeg");
    esp_err_t res = httpd_resp_send(req, (const char *)jpg_buf, jpg_buf_len);
    
    free(jpg_buf); 
    return res;
}

// --------------------------------- Configuración del Servidor HTTP ---------------------------------------------------------------
httpd_handle_t setup_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t stream_httpd = NULL;

	httpd_uri_t uri_index = {
		.uri = "/", 
		.method = HTTP_GET, 
		.handler = index_handler, 
		.user_ctx = NULL
	};
    httpd_uri_t uri_capture = {
		.uri = "/single_capture", 
		.method = HTTP_GET, 
		.handler = single_capture_handler, 
		.user_ctx = NULL
	};
    httpd_uri_t uri_stream = {
		.uri = "/stream", 
		.method = HTTP_GET, 
		.handler = jpg_stream_httpd_handler, 
		.user_ctx = NULL
	};
    
    if (httpd_start(&stream_httpd , &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd , &uri_index); 
        httpd_register_uri_handler(stream_httpd , &uri_capture); 
        httpd_register_uri_handler(stream_httpd , &uri_stream); 
    }
    return stream_httpd;
}
