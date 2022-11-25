

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
float hts221_get_temp()
{
//	uint8_t buffer[2];
//	uint16_t temp;
//	hts221_readArray(buffer, HTS221_ADDRESS_TEMP_OUT_L, 2);
//	temp=(((uint16_t)buffer[1]) << 8)| (uint16_t)buffer[0];

	  int16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
	  int16_t T0_degC, T1_degC;
	  uint8_t buffer[4], tmp;
	  uint8_t buffer0,buffer1;
	  uint16_t buffer00,buffer11;
	  float   tmp_f=0;
//	  hts221_readArray(buffer0, HTS221_T0_DEGC_X8, 1);//把T0_degC_x8读出到buffer
//	  hts221_readArray(buffer1, HTS221_T1_DEGC_X8, 1);
	  buffer0=hts221_read_byte(HTS221_T0_DEGC_X8);
	  buffer1=hts221_read_byte(HTS221_T1_DEGC_X8);
	  hts221_readArray(&tmp, HTS221_T0_T1_DEGC_H2, 1);//MSB读到tmp

	  T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)buffer0);
	  T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)buffer1);
	  T0_degC = T0_degC_x8_u16/8;
	  T1_degC = T1_degC_x8_u16/8;


	  buffer00=hts221_read_byte(HTS221_T0_OUT_L);
	  buffer11=hts221_read_byte(HTS221_T0_OUT_H);
//	  hts221_readArray(buffer, HTS221_T0_OUT_L, 2);//读出T0_out,T1out
	  T0_out = buffer00;
//	  hts221_readArray(buffer, HTS221_T0_OUT_H, 2);//

	  T1_out = buffer11;

	  buffer0=hts221_read_byte(HTS221_ADDRESS_TEMP_OUT_L);
	  buffer1=hts221_read_byte(HTS221_ADDRESS_TEMP_OUT_H);
//	  hts221_readArray(buffer[0],HTS221_ADDRESS_TEMP_OUT_L, 1);
//	  hts221_readArray(buffer[1],HTS221_ADDRESS_TEMP_OUT_H, 1);
	  T_out = (((uint16_t)buffer1) << 8) | (uint16_t)buffer0;

	  tmp_f = (float)(T_out - T0_out) * (float)(T1_degC - T0_degC) / (float)(T1_out-T0_out)  +  T0_degC;
	 // tmp_f *= 10.0f;

	return ((tmp_f));
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
