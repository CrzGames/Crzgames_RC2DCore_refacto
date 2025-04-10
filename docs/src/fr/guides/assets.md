# Asset Management

To manage game assets efficiently, RC2D utilizes the RRESPACKER tool, which packages assets into a compressed and encrypted `.rres` format. This proprietary file format is fully supported by RC2D and streamlines asset loading and usage within your projects.

## Building Assets with RRESPACKER

To generate `.rres` files from your original assets placed in the 'assets' folder, run the following script. You should only use `.rres` files for all static game resources (images, audio, fonts, etc.):

#### Unix
```bash
./build-assets-rres.sh
```

#### Windows
```bash
sh .\build-assets-rres.sh
```

::: warning IMPORTANT
Ensure all original assets are placed within the 'assets' directory at the root of your project. Paths to assets within `.rres` files should be relative to this directory, e.g., `images/test.png` for an image located at `assets/images/test.png`.
:::

It's essential to rebuild your `.rres` files each time you add new assets to ensure your game uses the latest resources.