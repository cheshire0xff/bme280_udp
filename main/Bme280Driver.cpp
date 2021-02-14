/*
 * bme280-interface.c
 *
 *  Created on: 2 lis 2020
 *      Author: chris
 */

#include "Bme280Driver.h"
#include "time.h"
#include "esp_log.h"


bme280_dev Bme280Driver::bme_h{};
uint8_t Bme280Driver::i2c_address{};

constexpr auto LOG_TAG = "bme280";

bool Bme280Driver::init(void)
{
    // Setup I2C interface
    i2c_config_t conf;

    conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = BME280_SDA_PIN;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = BME280_SCL_PIN;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
	i2c_param_config(BME280_I2C_INSTANCE, &conf);
	i2c_driver_install(BME280_I2C_INSTANCE, conf.mode, 0, 0, 0);

    // Initialize communication with the sensor
	if (findDevice(BME280_I2C_ADDR_PRIM))
	{
		i2c_address = BME280_I2C_ADDR_PRIM;
	}
	else if (findDevice(BME280_I2C_ADDR_SEC))
	{
		i2c_address = BME280_I2C_ADDR_SEC;
	}
	if (i2c_address != 0)
	{
		ESP_LOGI(LOG_TAG, "BME280 found on address:0x%X\n", i2c_address);
	}
	else
	{
		ESP_LOGE(LOG_TAG, "BME280 not found!n");
	}

	/* Map the delay function pointer with the function responsible for implementing the delay */
	bme_h.delay_us = delay_us;

	/* Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77)) */
	bme_h.intf_ptr = (void *)&i2c_address;

	/* Select the interface mode as I2C */
	bme_h.intf = BME280_I2C_INTF;

	/* Map the I2C read & write function pointer with the functions responsible for I2C bus transfer */
	bme_h.read = i2c_read;
	bme_h.write = i2c_write;


	auto status = bme280_init(&bme_h);
	if (status != 0)
	{
		ESP_LOGE(LOG_TAG, "BME280 init error: %d", status);
		return false;
	}
	/* Recommended mode of operation: Indoor navigation */
	uint8_t settings_sel;

	bme_h.settings.osr_h = BME280_OVERSAMPLING_1X;
	bme_h.settings.osr_p = BME280_OVERSAMPLING_16X;
	bme_h.settings.osr_t = BME280_OVERSAMPLING_2X;
	bme_h.settings.filter = BME280_FILTER_COEFF_16;

	settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
	status = bme280_set_sensor_settings(settings_sel, &bme_h);
	if (status != 0)
	{
		ESP_LOGE(LOG_TAG, "BME280 set settings error: %d", status);
		return false;
	}
	return true;
}

bme280_data Bme280Driver::measure()
{
	bme280_set_sensor_mode(BME280_FORCED_MODE, &bme_h);
	uint8_t data = 0;
	/* Wait for the measurement to complete and print data @25Hz */
	delay_us(40 * 1000, NULL);
	while(true)
	{
		bme280_get_regs(Registers::status, &data, 1, &bme_h);
		if (data & Flags::status_measuring)
		{
			delay_us(1 * 1000, NULL);
		}
		else
		{
			break;
		}
	}
	bme280_data mdata;
	bme280_get_sensor_data(BME280_ALL, &mdata, &bme_h);
	return mdata;
}

int8_t Bme280Driver::i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
	i2c_cmd_handle_t command_h = i2c_cmd_link_create();
	uint8_t shifted_addess = (*static_cast<uint8_t*>(intf_ptr)) << 1U;

	i2c_master_start(command_h);
	i2c_master_write_byte(command_h, shifted_addess | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(command_h, reg_addr, true);
	i2c_master_write(command_h, (uint8_t*)data, len, true);
	i2c_master_stop(command_h);

	auto ret = i2c_master_cmd_begin(BME280_I2C_INSTANCE, command_h, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(command_h);

	return ret;
}

int8_t Bme280Driver::i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
	i2c_cmd_handle_t command_h = i2c_cmd_link_create();
	uint8_t shifted_addess = (*static_cast<uint8_t*>(intf_ptr)) << 1U;

	i2c_master_start(command_h);
	i2c_master_write_byte(command_h, shifted_addess | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(command_h, reg_addr, true);

	i2c_master_start(command_h);
	i2c_master_write_byte(command_h, shifted_addess | I2C_MASTER_READ, true);

	if (len > 1) {
		i2c_master_read(command_h, data, len-1, I2C_MASTER_ACK);
	}
	i2c_master_read_byte(command_h, data+len-1, I2C_MASTER_NACK);
	i2c_master_stop(command_h);

	auto ret = i2c_master_cmd_begin(BME280_I2C_INSTANCE, command_h, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(command_h);

	return ret;
}

void Bme280Driver::delay_us(uint32_t usek, void *intf_ptr)
{
	vTaskDelay( usek / portTICK_PERIOD_MS / 1000);
}

bool Bme280Driver::findDevice(uint8_t address)
{
	uint8_t data{};
	i2c_read(Registers::id, &data, 1, &address);
	return data == deviceId;
}
