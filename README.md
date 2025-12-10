# projet-G-B
zeineb sahbani 
# 📌 Système de Gestion de Réservations — Langage C
Application en **langage C** permettant de gérer des réservations de salles de réunion, avec :
- vérification automatique des disponibilités,
- calcul du tarif,
- génération de factures,
- statistiques complètes,
- persistance des données dans des fichiers.

Projet adapté aux petites entreprises, espaces de coworking ou centres de formation.

---

## 🎯 Objectifs du projet
- Gérer les réservations avec vérification des conflits
- Calculer automatiquement le coût d’une réservation
- Générer des factures simples au format texte
- Fournir des statistiques d'utilisation
- Sauvegarder et charger automatiquement toutes les données

---

## 🗂️ Fonctionnalités principales

### ✔ Gestion des salles
- Nom, capacité, tarif horaire, équipements
- Tarifs enregistrés automatiquement dans `tarifs.txt`

### ✔ Système de réservation
- Vérification de disponibilité
- Détection des chevauchements d’horaires
- Vérification de capacité
- Calcul automatique du tarif  
  `tarif = tarif_horaire × durée_en_heures`
- Ajout, suppression, modification, recherche

### ✔ Facturation
- Génération automatique d’un fichier :  
  `factures/facture_<id>.txt`
- Contient : client, salle, date, durée, montant total

### ✔ Statistiques
- Chiffre d’affaires par salle
- Nombre de réservations par mois
- Salles les plus populaires
- Classement des clients
- Taux d’occupation
- Revenus annuels

### ✔ Persistance
- Les réservations sont sauvegardées dans  
  `data/reservations.txt`
- Chargement automatique au lancement

---

## 📁 Arborescence du projet
Le programme
