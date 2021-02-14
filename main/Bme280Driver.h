/*
 * bme280-interface.h
 *
 *  Created on: 2 lis 2020
 *      Author: chris
 */

#ifndef MAIN_BME280_DRIVER_H_
#define MAIN_BME280_DRIVER_H_

#include "bme280.h"
#include "driver/i2c.h"

/*
 * bme 280 interface implementation for esp32
 * with i2c address auto detection(prim/sec)
 */

class Bme280Driver
{
public:
    static constexpr auto BME280_SDA_PIN 			=		GPIO_NUM_23;
    static constexpr auto BME280_SCL_PIN 				=	    GPIO_NUM_22;
    static constexpr auto BME280_I2C_INSTANCE 	=		I2C_NUM_1;

	enum Registers : uint8_t
	{
		id = 0xD0,
		status = 0xF3,
	};

	enum Flags : uint8_t
	{
		status_measuring = 1 << 3,
		status_update  = 1 << 0,
	};
	static constexpr uint8_t deviceId = 0x60;
public:
	static bool init();
	static bme280_data measure();

private:
	static bme280_dev bme_h;
    static uint8_t i2c_address;
private:
    static bool findDevice(uint8_t address);
    static int8_t i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);
    static int8_t i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);
    static void delay_us(uint32_t usek, void *intf_ptr);
};


#endif /* MAIN_BME280_DRIVER_H_ */
