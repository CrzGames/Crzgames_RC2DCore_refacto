# 🌍 Distribution

## 🔑 Création d'une clé de signature

Pour créer une clé de signature nécessaire pour signer les applications Android via Gradle, suivez ces étapes : <br /><br />

1. **Installation de Java OpenJDK** :
    - Téléchargez et installez `Java OpenJDK`.
    - Ajoutez `OpenJDK`au `PATH ENVIRONMENT`.

<br />

2. **Génération du Keystore** :
    - Utilisez la commande `keytool` incluse avec OpenJDK pour générer un nouveau keystore :

      ```bash
      keytool -genkeypair -v -keystore my-release-key.keystore -keyalg RSA -keysize 2048 -validity 10000 -alias keystore_alias_name
      ```

      Explication des Paramètres :
      ::: details Click me to view

      - `genkeypair` : Génère une paire de clés (clé publique et clé privée). C'est la base de votre keystore.

      - `v` : Active le mode verbeux, fournissant des détails supplémentaires pendant l'exécution de la commande.

      - `keystore my-release-key.keystore` : Spécifie le nom et le chemin du fichier keystore à créer. Remplacez my-release-key.keystore par le nom de votre choix pour le fichier keystore.

      - `keyalg RSA` : Définit l'algorithme de chiffrement à utiliser pour générer les clés. RSA est un choix courant pour les applications Android.

      - `keysize 2048` : Détermine la taille de la clé. 2048 bits est recommandé pour une sécurité adéquate.

      - `validity 10000` : Définit la durée de validité de la clé en jours. 10000 jours couvrent plus de 27 ans, assurant que la clé reste valide pour une période considérable.

      - `alias keystore_alias_name` : Attribue un nom d'alias à votre clé. Remplacez keystore_alias_name par un nom d'alias unique pour identifier la clé dans le keystore.
      :::

      ::: warning IMPORTANT
      Après avoir créer la clé de signature, `conservez précieusement` les informations suivantes et ajoutez les 3 premiers aux `SECRETS` Github : 
      - `ANDROID_KEYSTORE_KEY_PASSWORD` (identique au `ANDROID_KEYSTORE_PASSWORD`)
      - `ANDROID_KEYSTORE_PASSWORD` 
      - `ANDROID_KEYSTORE_ALIAS_NAME`
      - Le fichier `my-release-key.keystore` générer
      :::

      ::: danger IMPORTANT
      Ne pas utiliser `Java JDK` de la distribution : `Oracle` pour créer la clé de signature, plusieurs problèmes à ce sujet qui concerne la création de la clé de signature ! (16/02/2024)
      :::

<br />

3. **Encodage du Keystore en base64** :
    - Encodez le fichier `.keystore` généré en base64 :

      ```bash
      base64 my-release-key.keystore > keystore_base64.txt
      ```

      ::: tip À noter
      Ce fichier `.txt` contient la représentation en base64 de votre keystore.
      :::
      ::: warning IMPORTANT
      Ajouter comme `SECRET` dans Github pour la CI / CD :
      - `ANDROID_KEYSTORE_BASE64` = valeur du fichier `keystore_base64.txt`
      :::

<br /><br /><br /><br />


## 🚀 Production
### ✋ Manual Distribution Process
<br />

#### 1. Setup Environment :
::: details Click me to view
Il faudra au préalable avoir configurer l'environnement Android par rapport à votre système.
::: warning OBLIGATOIRE
Si jamais cela n'ai pas fait voir : [Prérequis](https://librc2d.crzcommon2.com/guides/getting-started/prerequisites.html)
:::

<br />

#### 2. 🛠 Préparation (1)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` Android. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/android/preparing.html)
:::

<br />

#### 3. 🛠 Préparation (2)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait les `guides` sur la création d'un `compte Google Developer`, ainsi que la création d'une nouvelle application Android à faire `manuellement` sur `Google Play Store`. <br />

Voir : [Guides Google](https://librc2d.crzcommon2.com/platforms/guides-google/)
:::

<br />

#### 4. 🔄 Gestion des Versions
::: details Click me to view
Le processus de versionning pour chaque mise à jour de l'application est crucial pour le suivi et la gestion des déploiements. Trois éléments sont à considérer :

   - **`versionCode`** : Utilisé par le PlayStore pour identifier une nouvelle mise à jour.
   - **`versionName`** : Affiché aux utilisateurs, représentant la version de manière lisible.
   - **`android-project/app/build.gradle`** : Fichier de configuration de l'application Android, contenant les informations de version de l'application. Il y a deux variables d'environnement à définir : `ANDROID_VERSION_NAME` et `ANDROID_VERSION_CODE`.
   
   ::: warning IMPORTANT
   Pour le Play Store, le numéro de version visible aux utilisateurs `versionName` peut être n'importe quelle chaîne, donc inclure le nom de la branche et le hash du commit est techniquement possible. <br /><br />
   Vous devrez également spécifier un `versionCode` qui doit être un `entier strictement croissant`.
   Ce `versionCode` ne peut pas contenir de lettres ou de symboles, il doit être un `entier` et d'une valeur MAXIMUM de `2100000000`.
   :::

<br />

#### 5. Build Release AND Sign APK / AAB :
::: details Click me to view
Pour générer un projet Android en mode `Release` pour la distribution, exécutez le script suivant :
```bash
# Unix
./build-scripts/generate-project-production/android.sh

# Windows
sh .\build-scripts\generate-project-production\android.sh
```

Récupérer les fichiers `.apk` et `.aab` dans le dossier : `dist/project-production/android/`.

::: danger OBLIGATOIRE
Ajoutez 5 VARIABLES D'ENVIRONNEMENT à votre système, avec les valeurs correspondante quand vous avez créer la clé de signature et ainsi que le fichier .keystore :
- `ANDROID_KEYSTORE_PASSWORD`
- `ANDROID_KEYSTORE_KEY_PASSWORD`
- `ANDROID_KEYSTORE_ALIAS_NAME`
- `ANDROID_VERSION_NAME`
- `ANDROID_VERSION_CODE` <br />
- Assurez-vous également d'ajouter le fichier `my-release-key.keystore` (doit être nommé exactement comme cela) `à la racine` de votre `dépôt`.
:::

<br />

#### 6. Deploy the application to the PlayStore :
::: details Click me to view
Aller sur le site du PlayStore : https://play.google.com/console/ <br />

::: warning OBLIGATOIRE
Si l'application est déjà présent sur le PlayStore et déjà accepter par Google. Si vous souhaiter vous pouvez déployer une nouvelle version de l'application, via le pipeline CI / CD pour automatiser cela !
:::

<br /><br />


### ⚙️➡️ Automatic Distribution Process (CI / CD)
<br />

#### 1. 🛠 Préparation (1)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` Android. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/android/preparing.html)
:::

<br />

#### 2. 🛠 Préparation (2)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait les `guides` sur la création d'un `compte Google Developer`, ainsi que la création d'une nouvelle application Android à faire `manuellement` sur `Google Play Store`. <br />

Voir : [Guides Google](https://librc2d.crzcommon2.com/platforms/guides-google/)
:::

<br />

#### 3. Prérequis pour l'Automatisation du Déploiement
::: details Click me to view
::: danger OBLIGATOIRE
Pour automatiser le déploiement de l'application Android sur le PlayStore via le pipeline, il faut obligatoirement avoir soumis pour examen sont application Android et qu'elle soit accepter par Google.
:::

<br />

#### 4. 🔄 Ajouts des SECRETS Github
::: details Click me to view
Ajoutées les SECRETS_GITHUB pour :
   - `ANDROID_GOOGLE_CLOUD_PLATEFORM_SERVICE_JSON` <br />(Pour cela voir : [Création d'un compte de service](https://librc2d.crzcommon2.com/platforms/android/guides-google.html), tout en bas de la page)
   - `ANDROID_KEYSTORE_BASE64`
   - `ANDROID_KEYSTORE_ALIAS_NAME`
   - `ANDROID_KEYSTORE_PASSWORD`
   - `ANDROID_KEYSTORE_KEY_PASSWORD`
   - `PAT` (crée un nouveau token si besoin sur le site de github puis dans le menu du "Profil" puis -> "Settings" -> "Developper Settings' -> 'Personnal Access Tokens' -> Tokens (classic))
:::

<br />

#### 5. 📝 Gestion des Changelogs
::: details Click me to view
Pour chaque nouvelle mise à jour de l'application Android, il est possible de mettre à jour le changelog du jeu pour le PlayStore. <br /><br />
Les changelogs pour les différentes langues doivent être ajoutés depuis le dossier racine : `android-project/distribution/whatsnew`. <br /><br />
Il est crucial de respecter les noms des fichiers pour chaque langue, conformément à la norme BCP 47.

 ::: tip Documentation
 Pour plus de détails sur la structuration des fichiers de changelog et l'utilisation de la norme BCP 47. <br />
 
 Consultez la documentation officielle : [RFC Editor - BCP 47](https://www.rfc-editor.org/info/bcp47) <br />
 
 Le package Github Actions utiliser dans le pipeline : [Upload Google Play](https://github.com/r0adkll/upload-google-play).
:::

<br />

#### 6. 🔄 Gestion des Versions
::: details Click me to view
Le processus de versionning pour chaque mise à jour de l'application est crucial pour le suivi et la gestion des déploiements. Trois éléments sont à considérer :

   - **`versionCode`** : Utilisé par le PlayStore pour identifier une nouvelle mise à jour.
   - **`versionName`** : Affiché aux utilisateurs, représentant la version de manière lisible.
   - **`android-project/app/build.gradle`** : Fichier de configuration de l'application Android, contenant les informations de version de l'application. Il y a deux variables d'environnement à définir : `ANDROID_VERSION_NAME` et `ANDROID_VERSION_CODE`.

   ::: warning IMPORTANT
   Pour le Play Store, le numéro de version visible aux utilisateurs `versionName` peut être n'importe quelle chaîne, donc inclure le nom de la branche et le hash du commit est techniquement possible. <br /><br />
   Vous devrez également spécifier un `versionCode` qui doit être un `entier strictement croissant`.
   Ce `versionCode` ne peut pas contenir de lettres ou de symboles, il doit être un `entier` et d'une valeur MAXIMUM de `2100000000`.
   :::


<br />

#### ✅ Automatisation via CI/CD

Après la validation de la première version de l'application par le PlayStore, puis après avoir fait la configuration requises, le processus de CI/CD peut être pleinement exploité pour automatiser les déploiements successifs. Cette étape marque un tournant dans le cycle de développement, permettant des mises à jour plus rapides et plus efficaces de votre application.