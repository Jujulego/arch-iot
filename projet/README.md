## Dépendances
### Zolertia:
Le projet doit être dans l'environnement Contiki: voir le dépôt de [Contiki-OS](https://github.com/contiki-os/contiki)

### Python
Pour se connecter au cloud Ubidots, ce projet utilise Python 3.
Pour installer les dépendances nécessaires entrez la commande suivante:

```shell script
pip install -r requirements.txt
```

## Lancement
### Capteur
1) Branchez sur une carte zolertia le capteur de trempérature Grove (v1.1), sur le port ADC1
2) Branchez la zolertia sur votre ordinateur et entrez la commande suivante:
```shell script
make sensor
```

### Serveur
1) Branchez sur une carte zolertia le buzzer Grove (v1.1), sur le port ADC1
2) Branchez sur la zolertia l'écran LCD Grove, sur le port digital
3) Branchez la zolertia sur votre ordinateur et entrez la commande suivante:
```shell script
make server
```

### Connexion au cloud
1) Entrez votre Token dans le fichier cloud.py
2) **En gardant la zolertia 'serveur' branchée à votre ordinateur** lancez le script python:
```shell script
python cloud.py
``` 