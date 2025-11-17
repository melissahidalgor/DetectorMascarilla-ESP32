# 😷 Detector de Cubrebocas con Visión por Computadora 

## Resumen del Proyecto
Detector de Cubrebocas en tiempo real utilizando una Red Neuronal Convolucional (CNN) optimizada y desplegada en un microcontrolador ESP32-S3 Sense. El procesamiento y la inferencia se realizan directamente en el MCU, y el resultado se visualiza a través de un Web Server local embebido en el MCU.

## 💻 Hardware 

| Componente | Función Principal |
| :--- | :---  | 
| **Seeed Studio XIAO ESP32S3 Sense** | Microcontrolador que integra sensor de cámara, micrófono digital, soporte para tarjeta SD y conexión a WiFi|
| **OV2640** | Cámara digital de baja potencia y alta resolución |

## ⚙️ Flujo de Procesamiento

**1. Pre-procesamiento de Imágenes**

  * **Captura:** El firmware captura un frame de la cámara OV2640 confiugrado a 96x96 pixeles en escala de grises.
    
  * **Cuantización:** La imagen se cuantiza entre los valores -127 y 127.
    
**2. Inferencia y Predicción**

  * **Configuración:** En el MCU se apartaron 400 KB de memoria para manejar los tensores y se registraron las operaciones necesarias para que el modelo pudiera implementarse.
    
  * **Inferencia:** Al presionar el botón en el servidro web se ejecuta la inferencia invocando al interprete y se obtiene a la salida valores positivos para la clase sin cubrebocas y valores negativos para la clase con cubrebocas
    
**3. Visualización (Web Server)**

  * **Web Server Asíncrono:** El ESP32 aloja un servidor web que transmite el stream de video (MJPEG) al navegador. 
    
  * **Despliegue:** Se guarda la imagen a la que se le aplicó la inferencia como blob en el servidor y se despliega junto con el resultado de la inferencia. 

## 🧠 Métricas de Rendimiento 

**Exactitud:** 0.9

| Métrica | Sin Cubrebocas | Con Cubrebocas | 
| :--- | :---: | :--- |
| **Precisión** | 1 | 0.83 | 
| **Recall** | 0.8 | 1 | 
| **F1 Score** | 0.89 | 0.91 | 

## 🎥 Video Explicativo

[https://drive.google.com/file/d/1PTjMPiA3vfWX6udvvVP7GrOufDO9nVzP/view?usp=sharing](https://drive.google.com/file/d/1KfVIKQQr5eJqM99pCCvH2AP4FNRlnBhu/view?usp=sharing)
