# 24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET

## Sommaire
- [Présentation](#présentation)
- [TP1](#tp1---bus-i2c)
- [TP2](#tp2---interfaçage-stm32---raspberry)
- [TP3](#tp3---interface-rest)
- [TP4](#tp4---bus-can)
- [TP5]()
- [Auteurs et Contact](#auteurs-)

---

## Présentation

Bienvenue sur le dépôt GitHub de **TP Bus&Reseau**. Ce projet a pour but de développer différentes connexions réseau entre STM et des modules/ serveur sur raspy.

Ce projet est organisé en 5 TP :
- **TP1** : Bus I2C.
- **TP2** : Interface stm32 - Raspberry.
- **TP3** : Interface REST.
- **TP4** : Bus CNA.
- **TP5** : Intégration I2C - Serial - REST - CAN

---

## TP1 - BUS I2C
### 1 - Capteur BMP280
#### Identification datasheet
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

## TP2 - Interfaçage STM32 - Raspberry

### 2.1 Initialisation de la raspy

>[!Note]
>* Pour commencer nous avons flashé la Raspy avec imager sous linux avec le wifi déja connecte et la connexion ssh prête.   
>* Une fois fait on peut allumer la raspy et se connecter à elle par ssh.   

### 2.2. Port série

>Pour tester la connexion en UART, on connecte Rx et Tx et on utilise `minicom` pour s'auto-envoyer des messages. Une fois que cela fonctionne, on peut connecter Rx sur Tx de la STM32 et Tx sur Rx de la STM32, en utilisant l'UART1 et les pins PA9 et PA10. Il ne faut pas oublier la masse.  

>On peut maintenant créer des fonctions pour gérer les requêtes du Raspberry dans le Callback de l'UART. Les requêtes font 5 caractères de long donc le callback est appelé tous les 5 caractères reçus, puis les chaînes de caractères sont comparées aux différentes requêtes attendues pour choisir la bonne fonction à exécuter.  

### 2.3. Requêtes depuis Python

On veut maintenant pouvoir envoyer les requêtes grâce à un programme et non en les tapant sur `minicom`. On installe donc Python et on utilise la bibliothèque `serial`. Avec un timeout à 1, une baudrate de 115200, un bit de stop et pas de parité. On utilise la fonction `Serial.write` pour envoyer les requêtes automatiquement.


## TP3 - Interface REST

### 3.1 Initialisation du serveur 

>[!Note]
>* Comme montré dans les captures ci-dessous nous avons mis en place un serveur qui est accessible via curl.  
<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/installPy1.png" width="600" /> </p> 
<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/installPy2.png" width="600" /> </p> 

>[!Note]
>* Puis nous avons fait en sorte que cela soit disponible sur un site web au port 5000.
<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/installPy3.png" width="600" /> </p> 
<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/installPy4.png" width="600" /> </p> 

terminal1
web1

### 3.2 Première page REST

>[!Note]
>* Il est important de noter que le décodeur @app.route permet de spécifier la page que l'on souhaite accéder. Le fragment <int:index> est de pouvoir récupérer une variable passé dans l'URL (Dans notre cas l'int)  
>* En premier lieu on peut voir que la réponse est pas de type json mais bien html/text

<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/jsonDump2.png" width="600" /> </p> 

html/txt

>* avec json.dump() il est donc possible de formater la réponse mais aussi de faire en sorte que la réponse soit bien de type Json. Pour le reste cela sera la deuxième solution et JsonNotify qui sera utilisé.
### JsonDump
<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/jsonNotify.png" width="600" /> </p> 

### JsonNotify
<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/jsonNotify2.png" width="600" /> </p> 




>[!Note]
>* Dans l'état actuel des choses les erreurs 404 ne sont pas prises en compte. En effet demander une page qui n'existe pas nous amène à cela:

<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/jsonNotify2.png" width="600" /> </p> 
<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/TP3/jsonNotify2.png" width="600" /> </p> 
[404Console](/screen_TP/TP3/page404console.png)
[404WebFail](/screen_TP/TP3/page404NotWorking.png)

>* Par conséquent nous avons ajouté le code permettant de gérer les pages 404. 
[404webSucess](/screen_TP/TP3/page404Working.png)

### 3.3 Première page HTTP 

>[!Note]
>* Pour la suite du TP nous utilisons le plugin RESTED.  
>* On commence avec un simple test avec la méthode POST ce qui nous donne :  

[POST](/screen_TP/TP3/RequestPost.png)

>* Auquel on ajoute l'API Cruid pour avoir toutes les fonctions suivantes   
>* GET  
>* PUT  
>* PATCH  
>* DELETE  

[ALL](/screen_TP/TP3/RequestTotal.png)
[PATCH](/screen_TP/TP3/RequestPatch.png)
[GET](/screen_TP/TP3/RequestGet.png)
[DELETE](/screen_TP/TP3/RequestDelete.png)


## TP4 - Bus CAN 
L'objectif de ce TP est de controler un moteur pas à pas à l'aide de la STM32 grâce à un Bus CAN.
Pour cela on utilise un "Tranceiver CAN" que l'on fixe directement sur la STM.

Pour pouvoir communiquer via le Bus CAN il faut d'abord le paramétrer dans le fichier IOC de la stm 32 pour avoir une vitesse de CAN de 500kbit/s.


<p align="center"><img src="https://github.com/JolanTho/24-25_ESE_TP_BUS_RESEAUX_WEIDLE_THOMASSET/blob/main/screen_TP/config_IOC.png" width="600" /> </p> 

Après avoir correct configurer le microcontroler, nous allons utiliser les fonctions : 
- HAL_CAN_Start (CAN_HandleTypeDef * hcan)
- HAL_CAN_AddTxMessage (CAN_HandleTypeDef * hcan, CAN_TxHeaderTypeDef * pHeader, uint8_t aData[], uint32_t * pTxMailbox)

Pour commander le moteur, on utilise le mode automatic. Cela nous permet de juste mettre une valeur d'angle et le signe de l'angle dans un tableau à deux dimension. 

Pour pouvoir faire bouger proprotionnelement le moteur par rapport à la température, on défini une température de consigne ici 25°C puis on compare cette consigne à la valeur mesuré par le capteur. Nous avons ensuite fait un correcteur PI pour faire tourner le moteur proportionnelement à l'écart de température entre le consigne et la température mesuré. 
  


## Auteurs : 
Jolan Thomasset (jolan.thomasset@ensea.fr)
Rémi Weidle   (remi.weidle@ensea.fr)
