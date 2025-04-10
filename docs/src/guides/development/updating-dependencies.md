
# Managing Dependencies

It provides a guide on updating these dependencies to maintain the engine's efficiency and security.

## 🔄 Updating Dependencies
1. Utilisez la commande pour mettre à jour le sous-module à sa dernière version disponible dans le dépôt distant. Cela téléchargera les dernières modifications du sous-module depuis son dépôt distant et les incorporera dans votre répertoire principal :
   ```bash
   git submodule update --remote

   git add .
   git commit -m "chore: update all dependencies"
   git push
   ```

<br /><br />