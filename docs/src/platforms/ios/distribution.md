# 🌍 Distribution iOS

## 🔑 Création d'un certificat, identifiant d'application et profil de provisionnement

Pour créer les certificats, identifiants d'application et profils de provisionnement nécessaires pour signer les applications iOS / IpadOS via Xcode sur Mac, suivez ces étapes :

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
   - Sélectionnez le type de certificat souhaité (`Apple Development / iOS Development` ou `Apple Distribution / iOS Distribution`), puis cliquer sur le boutton `Continue`.
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

4. **Création d'un Profil de Provisionnement** :
   - Accédez à `Certificates, Identifiers et Profiles` et sélectionnez `Profiles`.
   - Cliquez sur le bouton `+` pour `ajouter` un `nouveau profil de provisionnement`.
   - Sélectionnez le type de profil souhaité (`iOS App Development` ou `App Store Connect`), puis cliquer sur le boutton `Continue`.
   - `Choisissez l'App ID` que vous avez créé précédemment.
   - `Sélectionnez le certificat` que vous avez `généré`.
   - Sélectionnez les appareils pour lesquels vous souhaitez utiliser ce profil (`pour les profils de développement seulements`).
   - Donnez un nom à votre profil de provisionnement et générez-le.
   - Téléchargez le profil de provisionnement créer précédemment et `installez-le` en `double-cliquant dessus`.
   - Pour Xcode 15 il doit être dans : `~/Library/MobileDevice/Provisioning\`. <br />
     Pour Xcode 16 cela à changer il doit être dans : `~/Library/Developer/Xcode/UserData/Provisioning Profiles\`.

<br />

5. **Récupération de l'UUID, du Name et du Base64 du Profil de Provisionnement** :
   - Accédez à `Certificates, Identifiers et Profiles` et sélectionnez `Profiles`.
   - Cliquer sur le profil de provisionnement créer précédemment, `récupérer` la valeur du champ `Name` pour le nom du profil de provisionnement.
   - Sur la même page cliquer sur `Download` pour télécharger le profil de provisionnement.
   - Exécutez la commande suivante sur le fichier `.mobileprovision` précédemment téléchargé pour récupérer la valeur de l'UUID :
     ```bash
     grep -a -A 1 'UUID' path/to/your/*.mobileprovision
     ```
   - `Convertit` le fichier `.mobileprovision` en `base64` et copie le résultat dans le presse-papiers :
     ```bash
     base64 -i path/to/your/*.mobileprovision | pbcopy
     ```

<br />

6. **Créer un mot de passe spécifique d'application** :
   - Accédez via : [Account Manage](https://appleid.apple.com/account/manage/section/security).
   - Cliquez sur `Mot de passe pour applications`.
   - Cliquer sur le boutton `+`.
   - Saisir un nom pour le mot de passe spécifique d'application, puis cliquer sur `Créer`.
   - Copier le mot de passe généré et le conserver en lieu sûr.
   - Attention : Ce mot de passe ne peut plus être révélé après sa création. Si vous l'oubliez, vous devrez en créer un nouveau.
   
   :::warning IMPORTANT
   Cela sera utilisé par le binaire `altool` pour `valider` l'application et `uploader` le build sur TestFlight/AppStore. <br />
   Utilisable en local et utiliser dans le pipeline CI / CD.
   :::

<br />

7. **Ajout des Secrets dans Github, spécifique au pipeline CI / CD** :
   - `IOS_APPLE_PROVISIONING_PROFILE_APPLEDISTRIBUTION_NOMDEMONAPP_UUID` : `UUID` du profil de provisionnement.
   - `IOS_APPLE_PROVISIONING_PROFILE_APPLEDISTRIBUTION_NOMDEMONAPP_NAME` : `Name` du profil de provisionnement.
   - `IOS_APPLE_PROVISIONING_PROFILE_APPLEDISTRIBUTION_NOMDEMONAPP_BASE64` : Base64 du fichier `*.mobileprovision` du profil de provisionnement.
   - `APPLE_APPLICATION_SPECIFIC_PASSWORD` : Mot de passe spécifique d'application.
   - `IOSMACOS_APPLE_CERTIFICATE_APPLEDISTRIBUTION_P12_BASE64` : Base64 du certificat de signature `Apple Distribution` ou `iOS Distribution`.
   - `IOSMACOS_APPLE_CERTIFICATE_APPLEDISTRIBUTION_P12_PASSWORD` : Mot de passe du certificat de signature `Apple Distribution` ou `iOS Distribution`. C'est le mot de passe que vous avez mis lors de l'exportation du certificat au format `.p12`.
   
   :::warning IMPORTANT
   Vous pouvez remplacez `*_NOMDEMONAPP_*` par le nom de votre application pour le distinguer des autres applications. <br /><br />
   J'ai mis `IOSMACOS_*` pour des certificats qui peuvent être utilisés pour les deux plateformes iOS et macOS avec `Apple Distribution`. Mais le type de certificat `Apple Distribution` est compatible pour toute les plateformes de chez Apple : `iOS, iPadOS, macOS, tvOS, watchOS, visionOS`. <br /><br />
   J'ai mis `IOS_*` puisque le profil de provisionnement est spécifique à iOS. Les `profil de provisionnement` sont `spécifiques` pour `chaque plateforme` (iOS, macOS, tvOS, watchOS, VisionOS). <br /><br />
   Le `mot de passe spécifique d'application` peu être utiliser pour plusieurs applications, que ce soit pour iOS ou macOS. Mais il est recommandé de créer un mot de passe spécifique d'application pour chaque application.
   :::

<br />

8. Informations supplémentaires :
:::danger IMPORTANT
Il faut créer un profil de provisionnement et un identifiant d'application spécifique pour chaque application, sauf si vous avez créer un identifiant d'application générique (Wildcard). <br /><br />   
Donc pour une autre application, il faudra répéter les étapes `2`, `4`, `5` et ainsi que les `3 SECRETS github` concernant le `profil de provisionnement`. <br /><br />

En résumé, les certificats existants peuvent être réutilisés, mais les identifiants d'application et les profils de provisionnement doivent être uniques et spécifiques à chaque application.
:::

<br /><br /><br /><br />


## 🚀 Production

### ✋ Manual Distribution Process
<br />

#### 1. Setup Environment :
::: details Click me to view
Il faudra au préalable avoir configurer l'environnement iOS.
::: warning OBLIGATOIRE
Si jamais cela n'ai pas fait voir : [Prérequis](https://librc2d.crzcommon2.com/guides/getting-started/prerequisites.html)
:::

<br />

#### 2. 🛠 Préparation (1)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` iOS. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/ios/preparing.html)
:::

<br />

#### 3. 🛠 Préparation (2)
::: details Click me to view
::: warning IMPORTANT
Il faut avoir fait les `guides` sur la création d'un `compte Apple Developer`, ainsi que la création d'une nouvelle application iOS à faire `manuellement` sur `AppStoreConnect`. <br />

Documentation : [Guides iOS](https://librc2d.crzcommon2.com/platforms/ios/guides-apple.html)
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
./build-scripts/generate-project-production/ios.sh
```

Récupérer le bundle `.app` dans le dossier : `dist/project-production/ios/arm64-arm64e/iphoneos/`.

::: danger OBLIGATOIRE
Ajoutez 8 VARIABLES D'ENVIRONNEMENT à votre système et d'autres informations nécessaires pour la signature de l'application.
- `APPLE_PROVISIONING_PROFILE_NAME` = Nom du profil de provisionnement, utilisé dans le fichier `CMakelists.txt`.
- `APPLE_PROVISIONING_PROFILE_UUID` = UUID du profil de provisionnement, utilisé dans le fichier `CMakelists.txt`.
- `APP_VERSION` = Pour la propriété `CFBundleShortVersionString` dans le fichier `Info.plist`, utilisé dans le fichier `CMakelists.txt`.
- `APP_IOSMACOS_BUILD_VERSION` = Pour la propriété `CFBundleVersion` dans le fichier `Info.plist`, utilisé dans le fichier `CMakelists.txt`.
- `APP_IOSMACOS_IDENTIFIER` = Pour l'identifiant de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_GAME_DESCRIPTION` = Pour la description de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_LEGAL_COPYRIGHT` = Pour le droit d'auteur de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_COMPANY_NAME` = Pour le nom de la société de l'application, utilisé dans le fichier `CMakelists.txt`.
- `Certificat de signature` : `Apple Distribution: xxxxxx (TEAM_ID)` ou `iOS Distribution: xxxxxx (TEAM_ID)` installé dans le trousseau dans `Mes certificats`.
- `Profil de provisionning` : Dans le dossier `~/Library/MobileDevice/Provisioning Profiles\` (xCode 15) ou `~/Library/Developer/Xcode/UserData/Provisioning Profiles` (Xcode 16) sur le système.
:::

<br />

#### 6. Sign app AND Verify Signed App
::: details Click me to view
`L'étape précédent signe déjà l'application`, mais voici en détails ce que CMakelists.txt fait ci-dessous. <br /><br />

***Signer l'application*** :
```bash
codesign --sign "Apple Distribution: xxxxxx (TEAM_ID)" --verbose=4 --strict --deep --force --timestamp /path/to/MyBundle.app
```
- `--sign` : "Apple Distribution: xxxxxx (TEAM_ID)" : Spécifie le certificat de signature à utiliser.
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

#### 7. Create an .ipa package from an .app bundle
::: details Click me to view
Il faut créer un fichier `.ipa` à partir du bundle `.app` pour pouvoir le distribuer via TestFlight ou l'App Store. <br /><br />

```bash
# Create Payload directory and move .app into it
mkdir Payload
cp -a /path/to/MyBundle.app Payload/

# zip le Payload dans un .ipa
zip -r --symlinks MyIpa.ipa Payload/
```

::: danger OBLIGATOIRE
- `--symlinks` et `cp -a` : Cela est très important si il y a des `.framework` / `.xcframework` sinon vous aurez l'erreur : `bundle format is ambiguous`.
:::

<br />

#### 8. Upload .ipa to App Store Connect (TestFlight / App Store)
::: details Click me to view
Pour uploader le `build` sur App Store Connect via le binaire `altool`. <br /><br />

***Check si l'application est valide*** :
```bash
xcrun altool --validate-app --file path/to/MyIpa.ipa --type ios --username compteappledeveloper@toto.com --password motpassappspecific
```
- `--username` : C'est l'email du compte apple developer.
- `--password` : Le mot de passe de l'application spécifique, via -> [Account Manage](https://appleid.apple.com/account/manage/section/security), puis `Mot de passe d'application`.

<br />

***Uploader l'application sur TestFlight/AppStore*** :
```bash
xcrun altool --upload-app --file path/to/MyIpa.ipa --type ios --username compteappledeveloper@toto.com --password motpassappspecific
```
- `--username` : C'est l'email du compte apple developer.
- `--password` : Le mot de passe de l'application spécifique, via -> [Account Manage](https://appleid.apple.com/account/manage/section/security), puis `Mot de passe d'application`.
:::

<br />

#### 9. Soumettre l'application pour examen
::: details Click me to view
Une fois l'application uploadé sur App Store Connect, il faut soumettre l'application pour examen, via l'inteface web d'App Store Connect.
:::

<br /><br />


### ⚙️➡️ Automatic Distribution Process (CI / CD)
<br />

#### 1. 🛠 Préparation (1)
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` iOS. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/ios/preparing.html)
:::

<br />

#### 2. 🛠 Préparation (2)
::: details Click me to view
::: warning IMPORTANT
Il faut avoir fait les `guides` sur la création d'un `compte Apple Developer`, ainsi que la création d'une nouvelle application iOS à faire `manuellement` sur `AppStoreConnect` la première fois avant de pouvoir `utiliser le pipeline`. <br />

Documentation : [Guides iOS](https://librc2d.crzcommon2.com/platforms/ios/guides-apple.html)
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
- `IOS_APPLE_PROVISIONING_PROFILE_APPLEDISTRIBUTION_NOMDEMONAPP_UUID`
- `IOS_APPLE_PROVISIONING_PROFILE_APPLEDISTRIBUTION_NOMDEMONAPP_NAME`
- `IOS_APPLE_PROVISIONING_PROFILE_APPLEDISTRIBUTION_NOMDEMONAPP_BASE64`
- `APPLE_APPLICATION_SPECIFIC_PASSWORD`
- `IOSMACOS_APPLE_CERTIFICATE_APPLEDISTRIBUTION_P12_BASE64`
- `IOSMACOS_APPLE_CERTIFICATE_APPLEDISTRIBUTION_P12_PASSWORD`
- `PAT` (crée un nouveau token si besoin sur le site de github puis dans le menu du "Profil" puis -> "Settings" -> "Developper Settings' -> 'Personnal Access Tokens' -> Tokens (classic))

:::warning IMPORTANT
Il faut remplacer `*_NOMDEMONAPP_*` par le nom de votre application. <br />
Si vous avez `besoin d'informations` sur les `secrets ci-dessus`, il faut aller tout en haut de la page au `step 7`.
:::

<br />

#### 5. 📝 Gestion des Changelogs
::: details Click me to view
Pour chaque nouvelle mise à jour de l'application iOS, il est possible de mettre à jour le changelog du jeu pour l'AppStore. <br /><br />

TODO
:::
