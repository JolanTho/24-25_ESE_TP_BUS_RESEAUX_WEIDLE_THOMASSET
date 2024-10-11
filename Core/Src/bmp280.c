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

void BMP_etallonage(void){
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
}

int BMP_get_temp(void){

	uint8_t receive_buf[3];
	retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_address, &temp_add, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (Etalo-T)\r\n");
	}
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_address, receive_buf, 3, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C (Etalo-R)\r\n");
	}
	int nc_temp=receive_buf[0]<<12|receive_buf[1]<<4|receive_buf[2]>>4;
	return nc_temp;
}

int BMP_get_pres(void){

	uint8_t receive_buf[3];
	retour = HAL_I2C_Master_Transmit(&hi2c1,BMP280_address, &pres_add, 1, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C\r\n");
	}
	retour = HAL_I2C_Master_Receive(&hi2c1, BMP280_address, receive_buf, 3, HAL_MAX_DELAY);
	if(retour != HAL_OK){
		printf("Erreur de l'I2C\r\n");
	}
	int nc_pres=receive_buf[0]<<12|receive_buf[1]<<4|receive_buf[2]>>4;
	return nc_pres;
}

BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
	BMP280_S64_t var1, var2, p;
	var1 = ((BMP280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
	var2 = var2 + ((var1*(BMP280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BMP280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3)>>8) + ((var1 * (BMP280_S64_t)dig_P2)<<12);
	var1 = (((((BMP280_S64_t)1)<<47)+var1))*((BMP280_S64_t)dig_P1)>>33;
	if (var1 == 0){
		return 0; // On fait une levée d'erreur afin de ne pas avoir de division par 0.
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BMP280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BMP280_S64_t)dig_P8) * p) >> 19; p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7)<<4);
	return (BMP280_U32_t)p;
}
