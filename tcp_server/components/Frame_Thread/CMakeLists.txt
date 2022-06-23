idf_component_register(SRCS "src/Frame_Thread.c" "src/utils.c"     
                    REQUIRES led_strip nvs_flash protocol_examples_common
                    INCLUDE_DIRS "." "inc/.")
