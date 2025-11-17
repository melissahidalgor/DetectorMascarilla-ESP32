# 😷 Detector de Cubrebocas con Visión por Computadora 

## Resumen del Proyecto
Detector de Cubrebocas en tiempo real utilizando una Red Neuronal Convolucional (CNN) optimizada y desplegada en un microcontrolador ESP32-CAM. El procesamiento y la inferencia se realizan directamente en el MCU, y el resultado se visualiza a través de un Web Server local embebido en el MCU.

## 💻 Hardware 

| Componente | Función Principal |
| :--- | :---  | 
| **Seeed Studio XIAO ESP32S3 Sense** | Microcontrolador que integra sensor de cámara, micrófono digital, soporte para tarjeta SD y conexión a WiFi|
| **OV2640** | Cámara digital de baja potencia y alta resolución |

## ⚙️ Flujo de Procesamiento

**1. Pre-procesamiento de Imágenes**

  * **Captura:** El firmware captura un frame de la cámara OV2640 confiugrado a 96x96 pixeles en escala de grises.
    
  * **Cuantización:** La imagen se cuantiza 
    
**2. Inferencia y Predicción**

  * **Configuración**
    
  * **TensorFlow Lite Micro:** El framework ejecuta la inferencia de la CNN.
    
**3. Visualización (Web Server)**

  * **Web Server Asíncrono:** El ESP32 aloja un servidor web que transmite el stream de video (MJPEG) al navegador.
    
  * **Overlay:** El resultado de la predicción de la CNN se superpone a la imagen de video, 
