set(COMPONENT_REQUIRES json)

# Use GLOB_RECURSE to find all .c files in the src directory and include/Fonts directory
FILE(GLOB_RECURSE SRC_FILES 
    "src/*.c"
    "include/Fonts/*.c"
)

# Register the component with the required source files
idf_component_register(
    SRCS "main.c" ${SRC_FILES}
    INCLUDE_DIRS "include" "include/Fonts"
)
