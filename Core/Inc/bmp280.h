/*
* Driver du capteur temperature et pression I2C
*/

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include <stdint.h>

int checkID(void);
int BMP280_config(void);
void BMP_etalonage(void);
int BMP_get_temp(void);
int BMP_get_pres(void);
double bmp280_compensate_P_double(uint32_t);
double bmp280_compensate_T_double(uint32_t);



#endif
