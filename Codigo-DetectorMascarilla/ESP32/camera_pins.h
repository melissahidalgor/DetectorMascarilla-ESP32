#ifndef CAMERA_PINS_H_
#define CAMERA_PINS_H_

//#define CONFIG_BOARD_ESP32CAM_AITHINKER 1     // Change if required
#define CONFIG_BOARD_XIAO_ESP32S3_SENSE 1

// AiThinker ESP32Cam PIN Map
#if CONFIG_BOARD_ESP32CAM_AITHINKER
	#define CAM_PIN_PWDN 32
	#define CAM_PIN_RESET -1 //software reset will be performed
	#define CAM_PIN_XCLK 0
	#define CAM_PIN_SIOD 26
	#define CAM_PIN_SIOC 27
	
	#define CAM_PIN_D9 35
	#define CAM_PIN_D8 34
	#define CAM_PIN_D7 39
	#define CAM_PIN_D6 36
	#define CAM_PIN_D5 21
	#define CAM_PIN_D4 19
	#define CAM_PIN_D3 18
	#define CAM_PIN_D2 5
	#define CAM_PIN_VSYNC 25
	#define CAM_PIN_HREF 23
	#define CAM_PIN_PCLK 22

// AiThinker ESP32Cam PIN Map
#elif CONFIG_BOARD_XIAO_ESP32S3_SENSE
	#define CAM_PIN_PWDN   -1
	#define CAM_PIN_RESET  -1   //software reset will be performed
	#define CAM_PIN_XCLK   10
	#define CAM_PIN_SIOD   40
	#define CAM_PIN_SIOC   39
	
	#define CAM_PIN_D9 48
	#define CAM_PIN_D8 11
	#define CAM_PIN_D7 12
	#define CAM_PIN_D6 14
	#define CAM_PIN_D5 16
	#define CAM_PIN_D4 18
	#define CAM_PIN_D3 17
	#define CAM_PIN_D2 15
	#define CAM_PIN_VSYNC 38
	#define CAM_PIN_HREF  47
	#define CAM_PIN_PCLK  13
#endif

#endif