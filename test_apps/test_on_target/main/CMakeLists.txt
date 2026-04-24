idf_component_register(
    SRCS
        "main.c" 
        "floatswitch.cpp"
    INCLUDE_DIRS 
        "."
    REQUIRES 
        unity 
        floatswitch
        gpio_validator
    WHOLE_ARCHIVE
)

# Move the test source logic here later if needed, 
# for now we keep it in this main component to be the entry point.
