
# Icon Management

Managing icons efficiently is crucial for the branding and presentation of your game across various platforms. RC2D 
simplifies this process through a streamlined approach for generating and using icons.

## Setup Environment for Icon Management in RC2D

Managing icons in RC2D requires setting up the appropriate environment on your system. Follow these general and platform-specific installation steps before proceeding with icon management.

### All Systems Install

1. **Download and Install Cargo :** [Rust Programming Language - Cargo](https://www.rust-lang.org/tools/install)
2. **Add cargo to PATH ENVIRONMENT**
3. **Install package cargo :**
```bash
cargo install tauri-cli
```

<br />

### Install on Linux

```bash
sudo apt-get update
sudo apt-get install binutils
```

<br />

### Install on macOS

```bash
brew install binutils
```
::: tip NOTE
This installs `binutils` which includes `windres` required for Windows icons.
:::

<br />

### Install on Windows

1. **Download and Install MinGW:** [MinGW - Minimalist GNU for Windows](https://sourceforge.net/projects/mingw/files/)
2. **Install `mingw32-base` Package:** Use the MinGW installation manager.
3. **Add MinGW to PATH ENVIRONMENT:** `C:\MinGW\bin`

::: warning IMPORTANT
Ensure these tools are installed and properly configured to facilitate the icon management process in RC2D.
:::

<br /><br />


## Replacing the Default Icon

Start by replacing the default icon file named `app-icon-default.png` located in `./icons/app-icon-default.png`. The new icon should be a 1024x1024 pixels PNG file, maintaining the original file name.

::: warning IMPORTANT
The replacement icon must strictly be in PNG format and sized at 1024x1024 pixels to ensure compatibility across all platforms.
:::

<br /><br />


## Generating Icons for Android

To generate icons for Android, follow these steps:

1. Visit [AppIcon.co](https://www.appicon.co/) and upload the updated `app-icon-default.png` from the `./icons` directory.
2. After generating the icons, download the resulting folder.
3. Extract and locate the `android` folder within. You will find several `mipmap-*` directories inside.
4. Replace the existing `mipmap-*` folders in your project’s `icons/android/` directory with the newly generated ones.

::: tip NOTE
This step is crucial for ensuring your Android application has the correct icon sizes for various device resolutions.
:::

<br /><br />


## Automatic Icon Generation for Other Platforms

RC2D automates the icon generation process for iOS, macOS, Windows, and Linux platforms through a script. To generate icons for these platforms, execute the following script:

### Unix

```bash
./generate-icons.sh
```

### Windows

```bash
sh .\generate-icons.sh
```

::: tip NOTE
Ensure the `generate-icons.sh` script has execution permissions and the Android icons are properly placed in the `icons/android/` directory before running the script.
:::

::: warning REMEMBER
The script automatically handles icon generation and placement for iOS, macOS, Windows, and Linux platforms, using the Android icons as a base. Make sure to review the icons after generation for any necessary adjustments.
:::

By following these steps, you can ensure your game presents a consistent and professional appearance across all supported platforms.
