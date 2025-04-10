# 🌍 Distribution iOS

## 🔑 Création d'un certificat et d'un identifiant d'application

Pour créer le certificat et l'identifiant d'application nécessaires pour signer les applications macOS via Xcode, pour une application `en-dehors` du `Mac AppStore` suivez ces étapes :

1. **Accéder au portail Apple Developer** :
    - Connectez-vous via : [Apple Developer Portal](https://developer.apple.com/account/).

<br />

2. **Création d'un Identifiant d'Application (App ID / Identifier)** :
    - Accédez à `Certificats, identifiants et profils` et sélectionnez `Identifiants`.
    - Cliquez sur le bouton `+` pour ajouter un nouveau identifier.
    - Cocher la case `App IDs`, puis cliquer sur le boutton `Continue`.
    - Selectionner le type d'identifiant souhaité (`App` ou `App Clip`).
    - Nous avons à présent un formulaire, cocher la case `Explicit` pour un identifiant d'application `unique` ou `Wildcard` pour un identifiant d'application `générique` (mais sera limité en fonctionnalités).
    - Rentrer l'identifiant d'application souhaité (`Bundle ID`).
    - Ajouté également une valeur dans le champ `Description` pour l'identifiant d'application.

<br />

3. **Création d'un Certificat de Signature** :
    - Accédez à `Certificates, Identifiers et Profiles` et sélectionnez `Certificates`.
    - Cliquez sur le bouton `+` pour ajouter un nouveau certificat.
    - Sélectionnez le type de certificat `Developer ID Application`, puis cliquer sur le boutton `Continue`.
    - Suivez les instructions pour générer une demande de `signature de certificat` (`CSR`) à partir de votre Mac, voici la documentation : [CSR](https://developer.apple.com/fr/help/account/create-certificates/create-a-certificate-signing-request/).
    - `Importer` le fichier `CSR`. Puis cliquer sur le boutton `Continue`.
    - Téléchargez le certificat généré et `installez-le` sur votre Mac en `double-cliquant dessus`.
    - Il apparaît à présent dans votre `Keychain Access` sous `My Certificates` sur votre Mac.
    - Spécifiquement pour le pipeline CI / CD : `Exportez` le certificat en `fichier .p12` en allant dans `Keychain Access` sous `My Certificates`, puis trouver le via le nom du certificat, `cliquer` sur le certificat, puis `cliquer` sur `Export` et `suivez les instructions`. <br />
      `Attention` : Il vous sera demandé de définir un mot de passe pour le fichier `.p12`. Conservez ce mot de passe en lieu sûr, car il sera nécessaire pour `signer` l'application.
    - Spécifiquement pour le pipeline CI / CD : `Convertit` le fichier `.p12` en `base64` et copie le résultat dans le presse-papiers :
      ```bash
      base64 -i path/to/your/*.p12 | pbcopy
      ```

<br />

4. **Créer un mot de passe spécifique d'application** :
    - Accédez via : [Account Manage](https://appleid.apple.com/account/manage/section/security).
    - Cliquez sur `Mot de passe pour applications`.
    - Cliquer sur le boutton `+`.
    - Saisir un nom pour le mot de passe spécifique d'application, puis cliquer sur `Créer`.
    - Copier le mot de passe généré et le conserver en lieu sûr.
    - Attention : Ce mot de passe ne peut plus être révélé après sa création. Si vous l'oubliez, vous devrez en créer un nouveau.

<br />

5. **Ajout des Secrets dans Github, spécifique au pipeline CI / CD** : 
    - `APPLE_APPLICATION_SPECIFIC_PASSWORD` : Mot de passe spécifique d'application.
    - `MACOS_APPLE_CERTIFICATE_DEVELOPERIDAPPLICATION_P12_BASE64` : Base64 du certificat de signature `Developer ID Application`.
    - `MACOS_APPLE_CERTIFICATE_DEVELOPERIDAPPLICATION_P12_PASSWORD` : Mot de passe du certificat de signature `Developer ID Application`. C'est le mot de passe que vous avez mis lors de l'exportation du certificat au format `.p12`.

   :::warning IMPORTANT
   J'ai mis `MACOS_*` pour des certificats qui peuvent être utilisés `SEULEMENT` pour `macOS` avec `Developer ID Application`. <br /><br />
   Le `mot de passe spécifique d'application` peu être utiliser pour plusieurs applications, que ce soit pour iOS ou macOS. Mais il est recommandé de créer un mot de passe spécifique d'application pour chaque application.
   :::

<br /><br /><br /><br />


## 🚀 Production

### ✋ Manual Distribution Process
<br />

#### 1. Setup Environment :
::: details Click me to view
Il faudra au préalable avoir configurer l'environnement macOS.
::: warning OBLIGATOIRE
Si jamais cela n'ai pas fait voir : [Prérequis](https://librc2d.crzcommon2.com/guides/getting-started/prerequisites.html)
:::

<br />

#### 2. 🛠 Préparation (1)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` macOS. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/macos/preparing.html)
:::

<br />

#### 3. 🛠 Préparation (2)
::: details Click me to view
::: warning IMPORTANT
Il faut avoir fait le `guide` sur la création d'un `compte Apple Developer`. <br />

Documentation : [Guides iOS](https://librc2d.crzcommon2.com/platforms/macos/guides-apple.html)
:::

<br />

#### 4. Gestion des Versions
::: details Click me to view
Le processus de versionning pour chaque mise à jour de l'application est crucial pour le suivi et la gestion des déploiements. Trois éléments sont à considérer :

- **`CFBundleShortVersionString`** : Utilisé par l'App Store pour afficher la version de l'application aux utilisateurs. Doit être au format `MAJOR.MINOR.PATCH`.
- **`CFBundleVersion`** : Utilisé par l'App Store pour identifier une nouvelle mise à jour. Doit être un `entier strictement croissant` OU une `chaîne composée` de `un à trois entiers séparés par des points`.
- **`Info.plist`** : Fichier de configuration de l'application iOS, contenant les informations de version de l'application.
  :::

<br />

#### 5. Build Release
::: details Click me to view
Pour générer un projet iOS en mode `Release` pour la distribution, et également signer l'application vu que c'est directement configurer dans le fichier CMakeLists.txt. <br /><br />

Exécutez le script suivant :
```bash
./build-scripts/generate-project-production/macos.sh
```

Récupérer le bundle `.app` dans le dossier : `dist/project-production/macos/x64-arm64/`.

::: danger OBLIGATOIRE
Ajoutez 6 VARIABLES D'ENVIRONNEMENT à votre système et d'autres informations nécessaires pour la signature de l'application.
- `APP_VERSION` = Pour la propriété `CFBundleShortVersionString` dans le fichier `Info.plist`, utilisé dans le fichier `CMakelists.txt`.
- `APP_IOSMACOS_BUILD_VERSION` = Pour la propriété `CFBundleVersion` dans le fichier `Info.plist`, utilisé dans le fichier `CMakelists.txt`.
- `APP_IOSMACOS_IDENTIFIER` = Pour l'identifiant de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_GAME_DESCRIPTION` = Pour la description de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_LEGAL_COPYRIGHT` = Pour le droit d'auteur de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_COMPANY_NAME` = Pour le nom de la société de l'application, utilisé dans le fichier `CMakelists.txt`.
- `Certificat de signature` : `Developer ID Application: xxxxxx (TEAM_ID)` installé dans le trousseau dans `Mes certificats`.
:::

<br />

#### 6. Sign app AND Verify Signed App
::: details Click me to view
`L'étape précédent signe déjà l'application`, mais voici en détails ce que CMakelists.txt fait ci-dessous. <br /><br />

***Signer l'application*** :
```bash
codesign --sign "Developer ID Application: xxxxxx (TEAM_ID)" --verbose=4 --strict --deep --force --timestamp /path/to/MyBundle.app
```
- `--sign` : "Developer ID Application: xxxxxx (TEAM_ID)" : Spécifie le certificat de signature à utiliser.
- `--verbose` : Affiche des informations détaillées sur le processus de signature.
- `--strict` : Garantir que l'application respecte des normes de sécurité élevées avant de la distribuer aux utilisateurs finaux.
- `--deep` : Signe tous les composants imbriqués de l'application, comme les frameworks et les plugins.
- `--force` : Force la re-signature même si l'application a déjà été signée.
- `--timestamp` : Ajoute un horodatage à la signature pour une validation à long terme.

<br />

***Vérifier l'application signée***
```bash
codesign --verify --display --deep --verbose=4 /path/to/MyBundle.app
```
- `--verify` : Vérifie la signature du code de l'application.
- `--display` : Affiche des informations sur l'application signée.
- `--deep` : Vérifie également la signature de tous les composants imbriqués.
- `--verbose` : Affiche des informations détaillées sur le processus de vérification.

::: danger OBLIGATOIRE
- ATTENTION : Si nous avons des `.framework / .xcframework` dans le bundle `.app`, et qu'ont utilise le flag `--deep` lors de la signature, il faut avoir copier les `.framework / .xcframework` au préalable dans le dossier `Frameworks` via : `cp -a` (pour garder les liens symbolique, sinon gros risque d'avoir l'erreur : `bundle format is ambiguous`)
  :::

<br />

#### 7. Notarize App
::: details Click me to view
1. Si la signature à réussi, notarier l'application (`SEULEMENT` pour les certificats `Developer ID Application` en dehors du Mac AppStore) :
  ```bash
  # Store credentials for notarization
  xcrun notarytool store-credentials login \
  --apple-id myaccount@hotmail.com \
  --team-id TEAM_ID \
  --password app-password

  # Create zip for notarization
  zip -r --symlinks MyApp.zip path/to/MyBundle.app

  # Submit app for notarization
  xcrun notarytool submit MyApp.zip --wait --keychain-profile login
  ```
- `--password` : `Mot de passe d'application spécifique`, via -> [Account Manage](https://appleid.apple.com/account/manage/section/security), puis `Mot de passe d'application`. <br />
- `--apple-id` : Email du compte Apple Developer <br />
- `--team-id` : Il faudra aller sur : [Apple Account](https://developer.apple.com/account), puis `Membership details` -> `Team ID`
- `login` : Cette valeur peu être n'importe quel valeur de sont choix (c'est le `nom` du `store-credentials` qui sera `installé` dans le `trousseau de clé`)
- `--symlinks` : Cela est très important si il y a des `.framework` / `.xcframework` sinon vous aurez l'erreur : `bundle format is ambiguous`

<br />

2. Vérifier le status ou en détails de la notarization : <br />
  ```bash
  # Le status
  xcrun notarytool info <uuid> --keychain-profile login

  # En détails
  xcrun notarytool log <uuid> --keychain-profile login
  ```

<br />

3. Après avoir notarized l'application, utilisez `xcrun stapler` pour `attacher` le `ticket de notarisation` au bundle `.app` : <br />
   `Informations important` : Cela signifie que le ticket de notarisation, qui prouve que votre application a passé la notarisation par Apple, est ajouté directement à votre bundle d'application .app. Cela permet à macOS de vérifier que l'application a été notarizée sans avoir besoin de se connecter aux serveurs d'Apple à chaque fois que l'application est lancée. Cela garantit que Gatekeeper peut trouver le ticket même lorsqu'une connexion réseau n'est pas disponible.
  ```bash
  xcrun stapler staple path/to/MyBundle.app

  # Output :
  # Processing: /Users/corentin/Downloads/MyBundle.app
  # Processing: /Users/corentin/Downloads/MyBundle.app
  # The staple and validate action worked!
  ```

<br />

4. Vérifier la signature et la notarisation d'une application macOS :
  ```bash
  spctl --assess --verbose=2 --type exec path/to/MyBundle.app

  # Output
  # dist/project/macos/x64-arm64/bin/Release/MyBundle.app: accepted
  # source=Notarized Developer ID
  ```
- `--assess` : Cette option demande à spctl d'évaluer l'élément spécifié (dans ce cas, MyBundle.app). Cela vérifie que l'application respecte les politiques de sécurité en vigueur pour l'installation sur macOS.
- `--verbose` : La verbosité du rapport est définie à 2, ce qui signifie que la sortie de la commande sera détaillée. Cela fournit plus d'informations sur les résultats de l'évaluation, y compris les raisons pour lesquelles l'application pourrait être acceptée ou rejetée.
- `--type exec` : Cette option spécifie le type d'évaluation à effectuer.

<br />

5. `INFORMATIONS IMPORTANT` lors de la notarization :
- Il y a un gros risque d'avoir également l'erreur : `The executable requests the com.apple.security.get-task-allow entitlement.`, après l'étape de signature et qu'ont veut notarier ! Il faut ajouté cela lors de la construction CMake : `CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS=NO`, attention si cela est utilisé nous ne pouvons plus debbuger, à utiliser seulement pour la production.
:::

<br />

#### 8. Application prête pour la distribution
::: details Click me to view
Une fois l'application signée et notarized, elle est prête pour la distribution.
:::

<br /><br />


### ⚙️➡️ Automatic Distribution Process (CI / CD)
<br />

#### 1. 🛠 Préparation (1)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` macOS. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/macos/preparing.html)
:::

<br />

#### 2. 🛠 Préparation (2)
::: details Click me to view
::: warning IMPORTANT
Il faut avoir fait le `guide` sur la création d'un `compte Apple Developer`. <br />

Documentation : [Guides iOS](https://librc2d.crzcommon2.com/platforms/macos/guides-apple.html)
:::

<br />

#### 3. Gestion des Versions
::: details Click me to view
Le processus de versionning pour chaque mise à jour de l'application est crucial pour le suivi et la gestion des déploiements. Trois éléments sont à considérer :

- **`CFBundleShortVersionString`** : Utilisé par l'App Store pour afficher la version de l'application aux utilisateurs. Doit être au format `MAJOR.MINOR.PATCH`.
- **`CFBundleVersion`** : Utilisé par l'App Store pour identifier une nouvelle mise à jour. Doit être un `entier strictement croissant` OU une `chaîne composée` de `un à trois entiers séparés par des points`.
- **`Info.plist`** : Fichier de configuration de l'application iOS, contenant les informations de version de l'application.
  :::

<br />

#### 4. 🔄 Ajouts des SECRETS Github
::: details Click me to view
Ajoutées les SECRETS_GITHUB pour :
- `APPLE_APPLICATION_SPECIFIC_PASSWORD`
- `MACOS_APPLE_CERTIFICATE_DEVELOPERIDAPPLICATION_P12_BASE64`
- `MACOS_APPLE_CERTIFICATE_DEVELOPERIDAPPLICATION_P12_PASSWORD`
- `PAT` (crée un nouveau token si besoin sur le site de github puis dans le menu du "Profil" puis -> "Settings" -> "Developper Settings' -> 'Personnal Access Tokens' -> Tokens (classic))

:::warning IMPORTANT
Si vous avez `besoin d'informations` sur les `secrets ci-dessus`, il faut aller tout en haut de la page au `step 5`.
:::