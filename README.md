# 24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET

# TP1 - BUS I2C
## 1 - Capteur BMP280
### Identification datasheet
1 - 
Les 7 bits d'adresses du composant sont **"111011x"**. Les 6 premiers bits sont fixé et le dernier est changé par le SDO en fonction de si le SDO est connecté à la massse, alors l'adresse est **"1110110(0x76)"** ou si il est connecté à V_DDIO alors l'adresse est **"1110111(0x77)"**.
Les registres sont : 
- 0x88 à 0xA1 pour la calibration.
- 0xD0 pour l'identifier le composant. Ici, le nunéro d'identitfication est **0x58**.
- 0xF3 pour le status.
- 0xF4 pour le controle des option d'acquisition des données.
- 0xF5 pour placer le composant en mode **"Normal"**, en mode **"Forced"** et on mode **"Sleep"**.
- 0xF7, OxF8 et OxF9 pour la pression. Il faut trois registres pour tout stocker sur 20 bits en tout. 
- 0xFA, OxFB et OxFC pour la température. Il faut trois registres pour tout stocker sur 20 bits en tout.

# TP4 - Bus CAN 
L'objectif de ce TP est de controler un moteur pas à pas à l'aide de la STM32 grâce à un Bus CNA.
