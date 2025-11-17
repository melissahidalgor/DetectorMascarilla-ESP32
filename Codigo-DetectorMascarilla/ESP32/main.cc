#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_camera.h"
#include "webserver.h"
#include "esp_timer.h"
#include "camera_pins.h"
#include "connect_wifi.h"
#include "sensor.h"

#include "tf_functions.h"

#define CONFIG_XCLK_FREQ 16000000 

// -------------------------------------- Configurar la Camara ------------------------------------------------
static esp_err_t init_camera(void)
{
    camera_config_t camera_config = {
        .pin_pwdn  = CAM_PIN_PWDN,
        .pin_reset = CAM_PIN_RESET,
        .pin_xclk = CAM_PIN_XCLK,
        .pin_sccb_sda = CAM_PIN_SIOD,
        .pin_sccb_scl = CAM_PIN_SIOC,

        .pin_d7 = CAM_PIN_D9,
        .pin_d6 = CAM_PIN_D8,
        .pin_d5 = CAM_PIN_D7,
        .pin_d4 = CAM_PIN_D6,
        .pin_d3 = CAM_PIN_D5,
        .pin_d2 = CAM_PIN_D4,
        .pin_d1 = CAM_PIN_D3,
        .pin_d0 = CAM_PIN_D2,
        .pin_vsync = CAM_PIN_VSYNC,
        .pin_href = CAM_PIN_HREF,
        .pin_pclk = CAM_PIN_PCLK,

        .xclk_freq_hz = CONFIG_XCLK_FREQ,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_GRAYSCALE,  // The pixel format of the image: PIXFORMAT_ + YUV422|GRAYSCALE|RGB565|JPEG
        .frame_size = FRAMESIZE_96X96,     // The resolution size of the image: FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
		
		.jpeg_quality = 10, // The quality of the JPEG image, ranging from 0 to 63.
		.fb_count = 1,
		
		.fb_location = CAMERA_FB_IN_PSRAM,   // Set the frame buffer storage location       
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY, //CAMERA_GRAB_LATEST. Sets when buffers should be filled
    	.sccb_i2c_port = -1,
    	};
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        return err;
    }
    return ESP_OK;
}

// ----------------------------------------------------- Main -------------------------------------------------------------
extern "C" void app_main()
{
	
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

	if (init_camera() != ESP_OK) {
            ESP_LOGE("Webserver", "Camera init failed!");
            return;
    }
       
    connect_wifi(); 
	
    if (wifi_connect_status) { 
        if (setup_server() != NULL) {
            ESP_LOGI("Webserver", "Web Server running!");
        } else {
            ESP_LOGE("Webserver", "HTTP Server start failed!");
        }
    } else {
        ESP_LOGE("Webserver", "Wi-Fi connection failed.");
    }
    
    tf_setup();
}  