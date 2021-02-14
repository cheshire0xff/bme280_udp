idf_component_register(SRCS "udp_server.cpp" "Bme280Driver.cpp" 
					   REQUIRES "BME280_driver-master" "nvs_flash" "protocol_examples_common"
					   INCLUDE_DIRS ".")