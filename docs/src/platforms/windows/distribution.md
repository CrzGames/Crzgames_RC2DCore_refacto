# 🌍 Distribution Windows

## 🔑 Création d'un certificat

Pour créer le certificat pour signer les applications Windows, suivez ces étapes :

1. **Accéder au site SignMyCode** :
    - Créez un compte si vous n'en avez pas : [SignMyCode Register](https://signmycode.com/).
    - Connectez-vous à votre compte : [SignMyCode Login](https://signmycode.com/login).
    - Cliquer dans le menu en haut `Products`, puis choisir un prestataire de certificat (Sectigo, DigiCert, Certera, etc.).
    - Pour cet exemple, nous allons choisir `Azure` via le certificat: `Azure Key Vault EV Code Signing`
    - Nous prenons un certificat de type `EV` pour une `validation étendue` (Extended Validation) pour une `sécurité maximale`.
    - Ajoutez le certificat à votre panier et suivez les instructions pour le paiement.
    - Une fois les démarches effectuées, vous recevrez un email de confirmation avec les instructions pour récupérer le certificat.

<br />

2. **Accéder au site Azure** :
    - Créez un compte si vous n'en avez pas : [Azure Register](https://azure.microsoft.com/fr-fr/free/).
    - Connectez-vous à votre compte : [Azure Login](https://portal.azure.com/).
    - Puis suivez les instructions pour cela via la documentation officiel de SignMyCode pour : [Azure Key Vault HSM](https://signmycode.com/resources/how-to-create-private-keys-csr-and-import-code-signing-certificate-in-azure-keyvault-hsm).

<br />

3. **Ajout des Secrets dans Github, spécifique au pipeline CI / CD** :
    - `AZURE_KEY_VAULT_APP_CLIENT_ID` : Le `client ID` de l'application liée au certificat de signature `EV Code Signing`.
    - `AZURE_KEY_VAULT_APP_CLIENT_SECRET` : Le `client secret` de l'application liée au certificat de signature `EV Code Signing`.
    - `AZURE_KEY_VAULT_TENANT_ID` : `ID du tenant` du `coffre HSM` qui contient le certificat de signature `EV Code Signing`.

<br /><br /><br /><br />


## 🚀 Production

### ✋ Manual Distribution Process
<br />

#### 1. Setup Environment :
::: details Click me to view
Il faudra au préalable avoir configurer l'environnement Windows.
::: warning OBLIGATOIRE
Si jamais cela n'ai pas fait voir : [Prérequis](https://librc2d.crzcommon2.com/guides/getting-started/prerequisites.html)
:::

<br />

#### 2. 🛠 Préparation
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` Windows. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/windows/preparing.html)
:::

<br />

#### 3. Build Release
::: details Click me to view
Pour générer un projet Windows en mode `Release` pour la distribution. <br /><br />

Exécutez le script suivant :
```bash
./build-scripts/generate-project-production/windows.sh
```

Récupérer le bundle `.exe` dans le dossier : `dist/project-production/windows/x64-x86/`.

::: danger OBLIGATOIRE
Ajoutez 4 VARIABLES D'ENVIRONNEMENT à votre système.
- `APP_VERSION` = Pour la version de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_GAME_DESCRIPTION` = Pour la description de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_LEGAL_COPYRIGHT` = Pour le droit d'auteur de l'application, utilisé dans le fichier `CMakelists.txt`.
- `APP_COMPANY_NAME` = Pour le nom de la société de l'application, utilisé dans le fichier `CMakelists.txt`.
  :::

<br />

#### 4. Sign app AND Verify Signed App
::: details Click me to view
***Signer l'application*** : <br />
Dans notre cas, nous utilisons `AzureSignTool` pour signer l'application Windows. <br />
Disponible ici : [AzureSignTool](https://github.com/vcsjones/AzureSignTool).<br />
```bash
AzureSignTool.exe sign path/to/file.exe --azure-key-vault-url https://xxxx.vault.azure.net --azure-key-vault-client-id xxxx --azure-key-vault-client-secret xxxxx --azure-key-vault-tenant-id xxxxxx --azure-key-vault-certificate MyCertificate --timestamp-rfc3161 http://timestamp.digicert.com --timestamp-digest sha512 --file-digest sha512
```
- `--azure-key-vault-url` : URL du coffre-fort Azure Key Vault HSM.
- `--azure-key-vault-client-id` : ID du client de l'application liée au certificat de signature.
- `--azure-key-vault-client-secret` : Secret du client de l'application liée au certificat de signature.
- `--azure-key-vault-tenant-id` : ID du tenant du coffre HSM qui contient le certificat de signature.
- `--azure-key-vault-certificate` : Nom du certificat de signature.
- `--timestamp-rfc3161` : URL du serveur de timestamp.
- `--timestamp-digest` : Algorithme de hachage pour le timestamp.
- `--file-digest` : Algorithme de hachage pour le fichier.
- `path/to/file.exe` : Chemin du fichier à signer (exemple)

<br />

***Vérifier l'application signée*** : <br />
Pour vérifier l'application signée, vous pouvez utiliser le binaire `signtool` de Windows. <br />
```bash
signtool verify /pa /v <chemin_vers_le_fichier>
```
- `/pa` : Vérifie la signature en utilisant l'autorité de certification racine.
- `/v` : Affiche des informations détaillées sur le processus de vérification.
- `<chemin_vers_le_fichier>` : Chemin du fichier signé à vérifier.
:::

<br />

#### 5. Application prête pour la distribution
::: details Click me to view
Une fois l'application signée, elle est prête pour la distribution.
:::

<br /><br />


### ⚙️➡️ Automatic Distribution Process (CI / CD)
<br />

#### 1. 🛠 Préparation
::: details Click me to view
::: warning OBLIGATOIRE
Il faut avoir fait le `guide` de `préparation` pour un `nouveau jeu` Windows. <br />
Voir : [Guides Préparation](https://librc2d.crzcommon2.com/platforms/windows/preparing.html)
:::

<br />

#### 2. 🔄 Ajouts des SECRETS Github
::: details Click me to view
Ajoutées les SECRETS GITHUB pour :
- `AZURE_KEY_VAULT_APP_CLIENT_ID`
- `AZURE_KEY_VAULT_APP_CLIENT_SECRET`
- `AZURE_KEY_VAULT_TENANT_ID`
- `PAT` (crée un nouveau token si besoin sur le site de github puis dans le menu du "Profil" puis -> "Settings" -> "Developper Settings' -> 'Personnal Access Tokens' -> Tokens (classic))

:::warning IMPORTANT
Si vous avez `besoin d'informations` sur les `secrets ci-dessus`, il faut aller tout en haut de la page au `step 3`.
:::