# 24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET

# TP1 - BUS I2C
## 1 - Capteur BMP280
### Identification datasheet
1 - Les registres sont : 
- 0x88 à OxA1 pour la calibration.
- 0xD0 pour l'identifier le composant. Ici, le nunéro d'identitfication est **Ox58**.
- OxF3 pour le status.
- OxF4 pour le controle des option d'acquisition des données.
- OxF5 pour placer le composant en mode **"Normal"**, en mode **"Forced"** et on mode **"Sleep"**.
- OxF7, OxF8 et OxF9 pour la pression. Il faut trois registres pour tout stocker sur 20 bits en tout. 
- OxFA, OxFB et OxFC pour la température. Il faut trois registres pour tout stocker sur 20 bits en tout. 
