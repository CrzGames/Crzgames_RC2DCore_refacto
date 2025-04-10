# Guide de préparation pour un nouveau jeu macOS

## Étapes de modification du projet macOS

### 1. Modification de la valeur de la propriété `project` dans `CMakeLists.txt`

- **Fichier concerné**: `./CMakeLists.txt`.
- **Modification**: Changez la valeur de la propriété `project` pour refléter le nouveau nom du projet, qui sera également le nom de l'application iOS.

<br />

### 2. Mise à jour des fichiers pipeline CI

- **Fichier concerné**: `./.github/workflows/build_deploy_staging.yml` et `./.github/workflows/build_deploy_production.yml`.
- **Modification**: Changez la valeur des secrets / variables d'environnement :
  - `APP_IOSMACOS_IDENTIFIER: com.crzgames.testexe` : Modifier `com.crzgames.testexe` avec le nouveau identifier de l'application.
  - `APP_COMPANY_NAME: CrzGames` : Modifier `CrzGames` avec le nouveau nom de la compagnie.
  - `APP_GAME_DESCRIPTION: MyGame - MMORPG` : Modifier `MyGame - MMORPG` avec la nouvelle description du jeu.
  - `APP_LEGAL_COPYRIGHT: Copyright 2024 CrzGames` : Modifier `Copyright 2024 CrzGames` avec le nouveau texte de droit d'auteur.