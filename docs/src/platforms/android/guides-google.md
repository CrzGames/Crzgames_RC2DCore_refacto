# 📚 Google Guides

## 🌟 Création d'un compte Google Developer

### Étapes pour la création du compte

1. **Créez un compte Google** si vous n'en avez pas déjà un.
2. **Connectez-vous** à votre compte Google.
3. **Payez 25$** pour l'inscription à vie au compte développeur.
4. **Vérifiez votre compte Google Developer** via votre carte d'identité, permis, ou passeport.

<br /><br /><br />


## 📱 Création d'une nouvelle application Android sur le PlayStore

### Démarrage

1. Rendez-vous sur le site de : [Google Play Console](https://play.google.com/console).
2. Connectez-vous avec votre compte `Google Developer`.
3. Vous devriez être redirigé vers le tableau de bord de Google Play Console, dans la page `Accueil`.
4. Cliquer sur `Créer une application`. Il faudra remplir les informations suivantes :
   - Nom de l'application
   - Langue par défaut
   - Application ou jeu
   - Application gratuite ou payante
   - Déclarations à cocher
5. Puis cliquez sur `Créer une application` en bas à droite de la page.
6. Il y aura un boutton en haut à gauche `Toutes les applications`, cliquer dessus. <br />
   Votre application devrait `apparaître` en mode `Brouillon`.
7. `Cliquer` sur `la ligne` où se trouve `la nouvelle application` que vous venez de créer. <br />
   Vous devriez être redirigé vers la page `Tableau de bord` de votre application.
8. Depuis le `Tableau de bord`, il faut commencer par la partie `Démarrez les tests maintenant` en cliquant sur le boutton `Afficher les tâches` puis faire les étapes obligatoires :
   - `Sélectionner les testeurs` : Commencez par ajouter des testeurs internes pour tester votre application, en ajoutant une liste de diffusion appeler `qa`. <br />
   `100 testeurs internes` peuvent être ajoutés `maximum`.
   - `Créer une release` : Cliquer sur `Sélectionner une clé de signature` puis sur `Exporter et importer une clé à partir d'un keystore Java`. Ajoutez le fichier AAB SIGNÉ (build/signé le projet manuellement juste pour cette première version). Nommez la release, par exemple `0.0.1`.
   - `Prévisualiser et confirmer la version`.
9. Depuis le `Tableau de bord`, il faut ensuite faire la partie `Configuration de votre application` en cliquant sur le boutton `Afficher les tâches` puis faire les étapes obligatoires :
   - `Définir les règles de confidentialité`.
   - `Accès aux applications`.
   - `Annonces`
   - `Classification du contenu`.
   - `Cible`
   - `Application d'acutalités`.
   - `Sécurité des données`.
   - `Applis Gouvernementales`.
   - `Fonctionnalités fiancières`.
   - `Santé`
   - `Selectionner la catégorie de votre application et indiquer vos cordonnées`
   - `Configurer une fiche Play Store`
10. Depuis le `Tableau de bord`, il faut ensuite faire la partie `Publier votre application` en cliquant sur le boutton `Afficher les tâches` puis faire les étapes obligatoires :
    - `Selectionner les pays et les régions`.
    - `Selectionner les testeurs`.
    - `Créer une release`.
    - `Prévisualiser et confirmer la version`.
    - `Envoyer la version à Google pour examen`.
11. Depuis la side bar de gauche cliquer sur `Configuration` -> `Paramètres avancées` -> `Jouer pendant le téléchargement` :
    - Cocher la case `Jouer pendant le téléchargement désactivé`.

<br /><br /><br />


## Créer un compte "Account Service" pour Google Cloud Plateform (spécifique pour les pipelines CI / CD)
1. Activez l'API de développement Android de Google Play : <br />
- Aller sur : [API Android de Google Play](https://console.cloud.google.com/apis/library/androidpublisher.googleapis.com)
- Cliquez sur : `Activer`.

<br />

2. Créer un nouveau compte de service dans Google Cloud Platform : <br />
- Aller sur : [Google Cloud Platform](https://console.cloud.google.com/)
- Cliquer sur `IAM et Administration`
- Dans le menu à gauche cliquer sur `Comptes de service`
- En haut de la page cliquer sur `+ Créer un compte de service`.
- Cliquer sur les `...` dans la colonne `Actions` dans la ligne ou le compte à était créer, puis cliquer sur `Gérer les clés`, cliquer sur `clé de type "JSON"` il vas `télécharger` un fichier `.json` conserver le pour l'ajouter au SECRETS GITHUB : `ANDROID_GOOGLE_CLOUD_PLATEFORM_SERVICE_JSON`
- Ouvrez [Google Play Console](https://play.google.com/console) et choisissez votre compte développeur.
- Dans le menu de gauche cliquer sur `Utilisateurs et autorisations`.
- Cliquez sur `Inviter de nouveaux utilisateurs` et `ajoutez` l’e-mail du compte de service créé à l’étape précédente
- Revenir sur la page `Utilisateurs et autorisations` et cliquer sur le compte de service ajouté. Puis dans `Autorisations de l'application` il y a un boutton `Ajouter une application` cliquer dessus et ajouter l'application créer sur le PlayStore. Puis cliquer sur `Enregistrer les modifications`.
