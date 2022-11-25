

#include "lps22hb.h"
#include "stdio.h"
#include "math.h"
uint8_t addres2 = LPS22HB_DEVICE_ADDRESS_0;
const float R = 8.31432; 		// Universal gas constant 				[Nm/mol/K]
const float g = 9.80665; 		// Gravitational acc. 					[m*s-2]
const float M = 0.0289644;   	// Molar mass of earth air 				[kg/mol]
const float Pb = 101325;  		// Static pressure at sea level 		[Pa]
const float Tb = 288.15;		// Standard temperature at sea level	[K]

uint8_t lps22hb_read_byte(uint8_t reg_addr)
{
	uint8_t data = 0;
	return *(i2c_master_read(&data, 1, reg_addr, addres2, 0));
}


void lps22hb_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, addres2, 0);
}


void lps22hb_readArray(uint8_t * data, uint8_t reg, uint8_t length)
{
	i2c_master_read(data, length, reg, addres2, 0);
}

float lps22hb_get_temp()
{
	uint8_t buffer1;
	uint8_t buffer2;
	uint16_t temp;
	//lps22hb_readArray(buffer, LPS22HB_ADDRESS_TEMP_OUT_L, 2);
	buffer1=lps22hb_read_byte(LPS22HB_ADDRESS_TEMP_OUT_L);
	buffer2=lps22hb_read_byte(LPS22HB_ADDRESS_TEMP_OUT_H);
	temp=(((uint16_t)buffer2) << 8)|(uint16_t)buffer1;
	return (float)((int16_t)(temp/100));
	//return ((int16_t)temp/48+42.5);
}

float lps22hb_get_press(){

	uint8_t buffer1;
	uint8_t buffer2;
	uint8_t buffer3;
	uint32_t press=0;
	uint8_t i;

	buffer1=lps22hb_read_byte(LPS22HB_ADDRESS_PRESS_OUT_XL);
	buffer2=lps22hb_read_byte(LPS22HB_ADDRESS_PRESS_OUT_L);
	buffer3=lps22hb_read_byte(LPS22HB_ADDRESS_PRESS_OUT_H);
//	for(i = 0; i < 3; i++)
//	   press |= (((uint32_t)buffer1[i]) << (8 * i));
	/* convert the 2's complement 24 bit to 2's complement 32 bit */
	  press=((uint32_t)buffer3)<<16|((uint32_t)buffer2)<<8|buffer1;
	  if(press & 0x00800000)
	    press |= 0xFF000000;
	return (float)(((int32_t)press)/4096.0f);//*100to hpa
}

float lps22hb_get_altitude(){
	return ((R*Tb*log(lps22hb_get_press()*100/Pb))/(-g*M));  //https://www.mide.com/air-pressure-at-altitude-calculator
	//return (((pow(Pb/100/(lps22hb_get_press()), 1/5.257)-1)*(lps22hb_get_temp()+273.15))/0.0065);	//https://keisan.casio.com/exec/system/1224585971

}


uint8_t lps22hb_init(void)
{

	uint8_t status = 1;


	LL_mDelay(100);

	uint8_t val = lps22hb_read_byte(LPS22HB_WHO_AM_I_ADDRES);

	if(val == LPS22HB_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		addres2 = LPS22HB_DEVICE_ADDRESS_1;
		val = lps22hb_read_byte(LPS22HB_WHO_AM_I_ADDRES);
		if(val == LPS22HB_WHO_AM_I_VALUE)
		{
			status = 1;
		}
		else
		{
			status = 0;
		}
	}

	uint8_t ctrl1 = lps22hb_read_byte(LPS22HB_ADDRESS_CTRL1);
	ctrl1 &= ~0xFC;
	ctrl1 |= 0x30;
	uint8_t ctrl2=0x00;
	uint8_t ctrl3=0x00;
	//uint8_t ctrl4=0x00;
	lps22hb_write_byte(LPS22HB_ADDRESS_CTRL1, ctrl1);

	lps22hb_write_byte(LPS22HB_ADDRESS_CTRL2, ctrl2);
	lps22hb_write_byte(LPS22HB_ADDRESS_CTRL3, ctrl3);
//	lps22hb_write_byte(LPS22HB_ADDRESS_CTRL4, ctrl4);
	return status;
}


double generalPower(double base, double power) {

	if (base == 0) {
		return 0;
	}
	return exp(power * log(base));

}
