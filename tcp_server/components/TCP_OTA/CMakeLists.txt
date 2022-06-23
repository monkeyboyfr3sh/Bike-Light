idf_component_register(SRCS "src/OTA_Thread.c" "src/TCPOTA_Thread.c"     
                    REQUIRES nvs_flash protocol_examples_common driver app_update esp_https_ota
                    INCLUDE_DIRS "." "inc/.")
