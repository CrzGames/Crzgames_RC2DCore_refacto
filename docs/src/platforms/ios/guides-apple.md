# 📚 Apple Guides

## 🌟 Création d'un compte Apple Developer

### Étapes pour la création du compte

1. **Créez un compte Apple Developer** si vous n'en avez pas déjà un.
2. **Connectez-vous** à votre compte Apple.
3. **Payez 100$** pour la licence annuelle au compte développeur.
4. **Vérifiez votre compte Apple Developer** via votre carte d'identité, permis, ou passeport pour un compte individuel. Pour une licence d'organisation, vous aurez besoin de documents supplémentaires.

<br /><br /><br />


## 📱 Création d'une nouvelle application iOS minimal - AppStoreConnect

### Démarrage

1. Rendez-vous sur le site de : [AppStoreConnect](https://appstoreconnect.apple.com/)
2. Connecté vous avec votre compte Apple Developer.
3. Cliquez sur 'Apps' (icone).
4. Cliquez sur le bouton `+` en haut à gauche pour ajouter une nouvelle application, puis cliquez sur `Nouvelle app`.
5. Remplissez les informations demandées pour créer une nouvelle application. <br />
   - Notamment coché la case `iOS` pour la plateforme.
   - `Nom` Ceci apparaît sur la page produit de votre app une fois que vous l’avez publiée dans l’App Store et lorsque les utilisateurs installent votre app.
   - `Langue principale` Si la traduction des informations d’une app n’est pas disponible dans un pays ou une région, les informations dans votre langue principale seront utilisées.
   - `Identifiant de lot` pour l'identifiant de l'application (ex: com.company.app). Il faut avoir créer au préalable un identifier sur le site de Apple Developer. Il ne peut plus être modifié après le chargement du premier build.
   - `UGS` Il s’agit d’un identifiant unique pour votre app qui n’est pas visible par les utilisateurs.
   - Cliquer ensuite sur le boutton `Créer`.
6. La version `1.0.0` sera set automatiquement à l'application lors de la création.

<br />

## 📱 Application iOS pour la phase de production - AppStore

### Démarrage

1. Rendez-vous sur le site de : [AppStoreConnect](https://appstoreconnect.apple.com/)
2. Connecté vous avec votre compte Apple Developer.
3. Cliquez sur 'Apps' (icone).
4. Choisir notre application.
5. Depuis la `page principale` de `Distribution`. Ils faut ajoutées :
   - Les aperçus et capture d'écran pour iOS pour Iphone et Ipad. 
   - Un text promotionnel pour l'application (facultatif).
   - Un text de description pour l'application.
   - Des mots-clés séparer par des virgules.
   - Copyright.
   - Il y a `Publication de la version dans l’App Store` tout en bas de la page, coché la case : `Publier cette version manuellement`.
   - il y a `Informations utiles à la vérification de l’app` en bas de la page, si l'application nécessite des informations spécifiques pour être vérifier, il faut les ajouter ici.
6. Depuis la page `Distribution`, puis la side bar de gauche `Informations sur l'app`. Ils faut ajoutées :
   - Le nom de l'application.
   - Un sous-titre, un bref descriptif de l’app qui apparaîtra sous le nom de votre app sur votre page produit dans l’App Store.
   - Une langue-principale pour les meta-data pour l'AppStore (ainsi que des langues secondaires possibles).
   - Catégorie de l'application. 
   - Catégorie de l'âge.
   - Droits relatifs au contenu.
   - Sur la même page il y a `Réglementations et autorisations de l’App Store`, cliquer sur le boutton `Modifier`, puis choisir si il `s’agit` d’une `app de commerçant` (donc abonnement payant, achat intégrer dans le jeu, app payante..) ou si il `s'agit` d'une `app de non-commerçant`.
7. Depuis la page `Distribution`, puis la side bar de gauche il y a `Confiance et sécurité` > `Confidentialité de l'app`. Ils faut ajoutées :
   - URL de l’Engagement de confidentialité
   - Cliquer sur le boutton `Démarrer` tout en bas de la page pour `fournir` des `informations` sur les pratiques de votre app en `matière de collecte de données`.
8. Depuis la page `Distribution`, dans la side bar de gauche il y a `Monétisation` > `Tarifs et disponibilité` : 
   - On peut désactiver le fait que l'application iOS peu être disponible sur le store Vision Pro et Mac AppStore (seulement pour les puces Apple Silicon type M1/M2/M3).
   - Il faut également même si `l'application est gratuit` cliquer sur le boutton `Ajouter une tarification` selectionner la région `France (EUR)` et mettre le prix à `0.00 €` pour que l'application soit gratuite.
   - Il faut également sur la même page pour `Disponibilité de l'app` cliquer sur le boutton puis `selectionner` les `pays` ou `l'application sera disponible`.
   - Il faut également sur la même page pour `Catégorie d’imposition` cliquer sur le boutton `Modifier`, puis `selectionner` la `catégorie` de l'application.
9. A partir de ce moment là sois on passe par la CI / CD pour publier un build de l'application, puis cela vas soumettre l'application pour examen à l'AppStore OU sois directement via le site si on fait cela sans pipeline automatiser. <br />
    Il faut aller sur la page `Distribution`, puis sur la même page descendre un peu il y a un boutton `Ajouter un build`, cliquer dessus, puis ajouter le build de l'application, puis cliquer sur le boutton `Ajouter pour vérification` tout en haut à droite de la page.

<br />

## 📱 Application iOS pendant les phases de tests - TestFlight

### Démarrage

1. Rendez-vous sur le site de : [AppStoreConnect](https://appstoreconnect.apple.com/)
2. Connecté vous avec votre compte Apple Developer.
3. Cliquez sur 'Apps' (icone).
4. Choisir notre application.
5. Cliquer sur l'onglet `TestFlight`, il faut `obligatoirement` remplir la page `Informations sur les tests`, `requises` pour soumettre un `build` à des `testeurs externes`.
6. Ajouter des testeurs, depuis la page `TestFlight`, puis `Testeurs` > `Tests Internes +` ou `Tests Externes +`, à ce moment la on peut créer des groupes de testeurs internes et externes.
7. Je conseille de créer un groupe avec le nom `qa` pour les `testeurs internes`. 
8. Je conseille de créer un groupe avec le nom `beta` pour les `testeurs externes`.
9. Concernant les `testeurs externes`, pendant la phase `beta`, on peut `activer` un `lien public`, depuis la page `TestFlight`, en cliquant sur le nom du groupe des `testeurs externes`, puis sur la page cliquer sur `Créer un lien public`, puis copier le lien et le partager avec les testeurs (Il faut que le build de l'application soit `en cours de test` pour que le lien soit actif).
10. Concernant les `testeurs internes`, il faut les `ajouter manuellement` via `leur email`, ils recevront un email avec un lien pour télécharger l'application via TestFlight.
11. Vous pouvez `inviter` jusqu'à `100 testeurs internes` par `application`, et chacun peut tester jusqu'à `30 appareils`.
12. Vous pouvez également `inviter` jusqu'à `10 000 testeurs externes` par `application`.
13. `Accès Immédiat` : Les `testeurs internes`, généralement membres de votre équipe de développement, ont un `accès immédiat` au build dès son `téléversement` sur TestFlight. Il n'est pas nécessaire de passer par un processus de révision d'Apple pour commencer les tests internes.
14. `Révision par Apple Nécessaire` : Contrairement aux tests internes, les builds destinés aux `testeurs externes` doivent être soumis et `approuvés par Apple` avant que les tests puissent commencer. Ce processus est requis pour s'assurer que l'application respecte les directives minimales de sécurité et de fonctionnalité d'Apple.
15. Assurez-vous d'informer vos testeurs que, bien que votre application supporte iOS 12.0 ou + (par exemple), `l'utilisation` de `TestFlight` pour les tests, nécessite `iOS 14.0 ou une version ultérieure` (12/07/2024).
