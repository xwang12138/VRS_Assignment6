

#include "hts221.h"

uint8_t addres3 = HTS221_DEVICE_ADDRESS_0;
int H0;
int H1;
int H2;
int H3;

uint8_t hts221_read_byte(uint8_t reg_addr)
{
	uint8_t data = 0;
	return *(i2c_master_read(&data, 1, reg_addr, addres3, 0));
}


void hts221_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, addres3, 0);
}


void hts221_readArray(uint8_t * data, uint8_t reg, uint8_t length)
{
	i2c_master_read(data, length, reg, addres3, 0);
}

float hts221_get_humidity(){



	int hum = hts221_read_byte(HTS221_ADDRESS_HUMIDITY_OUT_L);
	hum += hts221_read_byte(HTS221_ADDRESS_HUMIDITY_OUT_H)*256;

	float res = ((1.0 * H1) - (1.0 * H0)) * (1.0 * hum - 1.0 * H2) / (1.0 * H3 - 1.0 * H2) + (1.0 * H0);
	if(res>100)
	{
		res=100;
	}
	else if(res<0)
	{
		res=0;
	}
	else
	{
		res=res;
	}
	//res = ( res > 100.0f ) ? 100: ( res < 0.0f ) ? 0.0 : ( res );
	return res;
}


uint8_t hts221_init(void)
{

	uint8_t status = 1;

	LL_mDelay(100);

	uint8_t val = hts221_read_byte(HTS221_WHO_AM_I_ADDRES);

	if(val == HTS221_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		status = 0;
	}

	uint8_t ctrl1 = hts221_read_byte(HTS221_ADDRESS_CTRL1);
	ctrl1 &= ~0x83;
	ctrl1 |= 0x81;

	//get calibration data
	hts221_write_byte(HTS221_ADDRESS_CTRL1, ctrl1);

	H0 = hts221_read_byte(HTS221_ADDRESS_H0_rH_x2)/2;

	H1 = hts221_read_byte(HTS221_ADDRESS_H1_rH_x2)/2;

	H2 = hts221_read_byte(HTS221_ADDRESS_H0_T0_OUT_L);
	H2 += hts221_read_byte(HTS221_ADDRESS_H0_T0_OUT_H) * 256;


	H3 = hts221_read_byte(HTS221_ADDRESS_H1_T0_OUT_L);
	H3 += hts221_read_byte(HTS221_ADDRESS_H1_T0_OUT_H) * 256;

	return status;
}
