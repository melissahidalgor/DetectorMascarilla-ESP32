/*
 * webserver.h
 *
 *  Created on: 5 nov 2025
 *      Author: Melissa
 */


#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "esp_http_server.h"

// Si el archivo es incluido por un compilador C++ (__cplusplus está definido), 
// envuelve las declaraciones de función en 'extern "C"'.
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Inicializa y arranca el servidor HTTP, registrando todas las rutas.
 * * @return httpd_handle_t El manejador del servidor HTTP o NULL si falla.
 */
httpd_handle_t setup_server(void);

#ifdef __cplusplus
}
#endif	

#endif // WEBSERVER_H