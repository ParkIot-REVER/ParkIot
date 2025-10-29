# 🅿️ Park IoT — Système de contrôle d’accès connecté

## 📘 Description du projet
**Park IoT** est une solution IoT destinée à **automatiser et sécuriser l’accès** à un parking ou à une zone privée.  
Le système repose sur une carte **NodeMCU (ESP8266)** connectée à un **lecteur RFID**, permettant l’ouverture automatique d’un portail après identification d’une carte autorisée.  
Les données sont centralisées via Wi-Fi pour un **suivi en temps réel** et une **gestion simplifiée**.

---

## 🧠 Objectif global
Créer une **solution d’accès intelligente**, **économique** et **ouverte**,  
facilement intégrable dans les parkings ou bâtiments souhaitant  
automatiser et sécuriser la gestion des entrées et sorties.

---

## 🧩 Architecture du projet

### `hardware/`
Contient les éléments matériels et schémas de conception :
- **Plan du circuit électronique et schéma de câblage**
  Illustrant la connexion entre la carte NodeMCU, le lecteur RFID, l’écran LCD et le buzzer.

---

### `source/`
Dossier contenant l’ensemble du **code source** du projet :
- Programme principal gérant la **lecture RFID** et le **contrôle d’accès**  
- Gestion de la **communication Wi-Fi** via l’ESP8266  
- Scripts de **journalisation** ou d’envoi de données vers une base externe

---

### `composants.xls`
Fichier listant tous les **composants matériels utilisés** :
- Nom et référence du composant  
- Quantité  
- Prix unitaire estimé  
- Fournisseur ou lien d’achat  

---

### `presentation.pdf`
Document de **présentation technico-commerciale** :
- Contexte et problématique du projet  
- Présentation du produit Park IoT  
- Démonstration de la maquette  
- Ciblage commercial et **avantages concurrentiels**
- Conclusion

---

### `vulnerabilities.pdf`
Analyse des **vulnérabilités** du système :
- Sécurité des communications (Wi-Fi, RFID)
- Risques matériels et logiciels
- Contre-mesures et bonnes pratiques recommandées

---

### `audit/`
Dossier dédié à l’**évaluation du projet** :
- Résultats des tests fonctionnels et de performance  
- Rapports de validation du prototype  
- Améliorations ou pistes futures identifiées  

---

## ⚙️ Technologies principales
- **Microcontrôleur :** NodeMCU ESP8266  
- **Communication :** Wi-Fi  
- **Identification :** Lecteur RFID MFRC522 
- **Interface utilisateur :** Écran LCD + buzzer  
- **Langage de programmation :** Arduino  

---

## 👥 Équipe projet
- **Begon Eliott**  
- **Benyakhlaf Aymane**  
- **Boussallef El Mokhtar**  
- **Chapelle Camille**  
- **Coste Félix**

