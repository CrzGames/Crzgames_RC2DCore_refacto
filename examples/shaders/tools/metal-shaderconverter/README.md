# Metal Shader Converter

## Configuration requise
Exigences concernant l'utilisation du binaire `metal-shaderconverter`.
### macOS :
- macOS >= 13.0
- Xcode >= 15.0
## Windows :
- Windows >= 10
- Visual Studio >= 2019

<br /><br />

## Plateforme compatible avec les fichiers .metallib générer
- macOS >= 14.0
- iOS/iPadOS >= 17.0
**Note** : Les `.metallib` générés avec `metal-shaderconverter` fonctionneront sur macOS 14.0+ et iOS/iPadOS 17.0+, mais sans support du débogage et du profiling shader. <br />
Ces fonctions sont `uniquement disponibles` à partir de : macOS 15.0+ et iOS/iPadOS 18.0+.

<br /><br />

## Débogage, profilage et validation des shaders
Metal Shader Converter >= 2.0 prend en charge le transfert des informations de débogage de l'IR source vers les bibliothèques Metal. Cela vous permet de déboguer, de profiler et d'activer la validation des shaders pour vos pipelines convertis sur Xcode 16, macOS 15 et iOS 18 (ou versions ultérieures).

<br /><br />

## Utilisation
```bash
metal-shaderconverter shader.fragment.dxil -o ./shader.fragment.metallib
```