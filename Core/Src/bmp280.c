/*
 * Driver du capteur temperature et pression I2C
 */

#include "bmp280.h"
#include "main.h"
#include "stdio.h"

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef retour;

uint8_t BMP280_address = 0x77<<1;
uint8_t ID_address = 0xD0;
uint8_t ctrl_meas = 0xF4;
uint8_t buffer[1];

uint8_t config = (0b010<<5)|(0b101<<2)|(0b11); // Construction osrs_t || osrs_p || mode
uint8_t calib = 0x88;
uint8_t temp_add = 0xFA;
uint8_t pres_add = 0xF7;

typedef uint32_t BMP280_S32_t;

short dig_T1 = 0;
short dig_T2 = 0;
short dig_T3 = 0;
short dig_P1 = 0;
short dig_P2 = 0;
short dig_P3 = 0;
short dig_P4 = 0;
short dig_P5 = 0;
short dig_P6 = 0;
short dig_P7 = 0;
short dig_P8 = 0;
short dig_P9 = 0;

int checkID(void){
	retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_address, &ID_address, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (ID-T)\r\n");
		return 1;
	}
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_address, buffer, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (ID-R)\r\n");
		return 1;
	}
	printf("L'ID du capteur est 0x%x\r\n",buffer[0]);
	return 0;
}

int BMP280_config(void){
	uint8_t buf[2];
	buf[0]= ctrl_meas;
	buf[1]= config;
	retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_address, buf, 2, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur l'I2C (Conf-T)\r\n");
		return 1;
	}
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_address, buffer, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C(Conf-R)\r\n");
		return 1;
	}

	if(buffer[0] == config){
		printf("La config envoye est bien recue\r\n");
		return 0;
	}
	return 1;
}

void BMP_etalonage(void){
	uint8_t receive_buf[24];

	retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_address, &calib, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (Etalo-T)\r\n");
	}
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_address, receive_buf, 24, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (Etalo-R)\r\n");
	}
	printf("les valeurs de l'etallonage sont :\r\n");
	for(int i=0;i<24;i++){
		printf("calib %2d = 0x%x\r\n",i,receive_buf[i]);
	}
	dig_T1 = receive_buf[0]|(receive_buf[1]<<8);
	dig_T2 = receive_buf[2]|(receive_buf[3]<<8);
	dig_T3 = receive_buf[4]|(receive_buf[5]<<8);
	dig_P1 = receive_buf[6]|(receive_buf[7]<<8);
	dig_P2 = receive_buf[8]|(receive_buf[9]<<8);
	dig_P3 = receive_buf[10]|(receive_buf[11]<<8);
	dig_P4 = receive_buf[12]|(receive_buf[13]<<8);
	dig_P5 = receive_buf[14]|(receive_buf[15]<<8);
	dig_P6 = receive_buf[16]|(receive_buf[17]<<8);
	dig_P7 = receive_buf[18]|(receive_buf[19]<<8);
	dig_P8 = receive_buf[20]|(receive_buf[21]<<8);
	dig_P9 = receive_buf[22]|(receive_buf[23]<<8);
}

int BMP_get_temp(void){

	uint8_t receive_buf[3];
	retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_address, &temp_add, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (GetT-T)\r\n");
	}
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_address, receive_buf, 3, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (GetT-R)\r\n");
	}
	int nc_temp=receive_buf[0]<<12|receive_buf[1]<<4|receive_buf[2]>>4;
	return nc_temp;
}

int BMP_get_pres(void){

	uint8_t receive_buf[3];
	retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_address, &pres_add, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (GetP_T)\r\n");
	}
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_address, receive_buf, 3, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (GetP_R)\r\n");
	}
	int nc_pres=receive_buf[0]<<12|receive_buf[1]<<4|receive_buf[2]>>4;
	return nc_pres;
}

// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
	double var1, var2, T;
	int32_t t_fine;

	var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
	var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) * (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
	t_fine = (int32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}
// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{

	double var1, var2, p;

	var1 = ((double)t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double)dig_P5) * 2.0;
	var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
	var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);

	if (var1 == 0.0) {
	    return 0; // éviter la division par zéro
	}

	p = 1048576.0 - (double)adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double)dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double)dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;

	return p;

}
