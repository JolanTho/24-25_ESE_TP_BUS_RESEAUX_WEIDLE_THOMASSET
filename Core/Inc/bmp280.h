/*
* Driver du capteur temperature et pression I2C
*/

#ifndef INC_BMP280_H_
#define INC_BMP280_H_


int checkID(void);
int BMP280_config(void);
void BMP_etallonage(void);
int BMP_get_temp(void);
int BMP_get_pres(void);

#endif
