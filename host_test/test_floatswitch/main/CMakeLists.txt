# host_test/test_floatswitch/main/CMakeLists.txt

idf_component_register(
    SRCS 
        "main.cpp"
        "test_node_state_machine.cpp"
    
    INCLUDE_DIRS
        "."
        "../../common"
        
    REQUIRES 
        floatswitch
        gtest
        WHOLE_ARCHIVE
)

if(IDF_TARGET STREQUAL "linux")
    target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
    target_link_options(${COMPONENT_LIB} PUBLIC --coverage)
endif()