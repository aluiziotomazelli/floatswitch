idf_component_register(
    NAME floatswitch
    SRCS
        "src/float_switch.cpp"
        "src/hal_gpio.cpp"
        "src/hal_timer.cpp"
    
    INCLUDE_DIRS
        "include"
    
    REQUIRES
        esp_timer
        driver
    
    PRIV_REQUIRES
        esp_common
)

idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_libraries(${COMPONENT_LIB} PUBLIC --coverage)
endif()   
