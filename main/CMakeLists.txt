# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.cc connect_wifi.c webserver.cc tf_functions.cc 
    INCLUDE_DIRS        
    PRIV_INCLUDE_DIRS   
    REQUIRES            
    PRIV_REQUIRES       
        esp-tflite-micro 
        esp32-camera    
)

# Esto fuerza el estándar C++17, obligando al compilador a incluir las rutas de la librería C++
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)