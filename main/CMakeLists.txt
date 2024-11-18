set(COMPONENT_REQUIRES "json")

idf_component_register(SRCS "main.c" "include/epaper.c" "include/draw.c" "include/paint.c" "include/Fonts/font16.c" "include/Fonts/font8.c" "include/Fonts/font12.c" "include/bus.c" "include/wifi.c"
                    INCLUDE_DIRS "include" "include/Fonts"
                    )
