idf_component_register(SRCS "src/OTA_Thread.c" "src/TCPOTA_Thread.c" "src/Frame_Thread.c" "src/utils.c" "src/BK_IO.c"
                    REQUIRES nvs_flash protocol_examples_common driver app_update esp_https_ota led_strip led_strip
                    INCLUDE_DIRS "." "inc/.")
