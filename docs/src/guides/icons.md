
# 🎨 Icon Management

Managing icons efficiently is crucial for the branding and presentation of your game across various platforms. RC2D 
simplifies this process through a streamlined approach for generating and using icons.

## ⚙️ Setup Environment

Managing icons in RC2D requires setting up the appropriate environment on your system. Follow these general and platform-specific installation steps before proceeding with icon management.

### Windows / macOS / Linux

1. **Download and Install Cargo :** [Rust Programming Language - Cargo](https://www.rust-lang.org/tools/install)
2. **Add cargo to PATH ENVIRONMENT**
3. **Install package cargo :**
```bash
cargo install tauri-cli
```
::: tip NOTE
This installation then allows you to generate icons for macOS, Windows and Linux.
:::

<br /><br />


## 🔄 Replacing the Default Icon

Start by replacing the default icon file named `app-icon-default.png` located in `./icons/app-icon-default.png`. The new icon should be a 1024x1024 pixels PNG file, maintaining the original file name.

::: warning IMPORTANT
The replacement icon must strictly be in PNG format and sized at 1024x1024 pixels to ensure compatibility across all platforms.
:::

<br /><br />


## 🤖 Generating Icons for Android

To generate icons for Android, follow these steps:

1. Visit [AppIcon.co](https://www.appicon.co/) and upload the updated `app-icon-default.png` from the `./icons` directory.
2. Check the boxes for `Android`. Then click on the `Generate` button.
3. After generating the icons, download the resulting folder.
4. Extract and locate the `AppIcons.zip` folder within. You will find several `android/mipmap-*` directories inside.
5. Replace the existing `mipmap-*` folders in your project’s `./icons/android/` directory with the newly generated ones.

::: tip NOTE
This step is crucial for ensuring your Android application has the correct icon sizes for various device resolutions.
:::

<br /><br />


##  📱 Generating Icons for iOS

To generate icons for iOS, follow these steps:

1. Visit [AppIcon.co](https://www.appicon.co/) and upload the updated `app-icon-default.png` from the `./icons` directory.
2. Check the boxes for `Iphone` and `Ipad`. Then click on the `Generate` button.
3. After generating the icons, download the resulting folder.
4. Extract and locate the `AppIcons.zip` folder inside. You will find the `appstore.png` file and the `Assets.xcassets` folder inside.
5. Replace the existing `Assets.xcassets` folder and `appstore.png` file in your project's `./icons/ios/` directory with the newly generated ones.

::: tip NOTE
This step is crucial for ensuring your iOS application has the correct icon sizes for various device resolutions.
:::

<br /><br />


## 🛠️ Automatic Icon Generation for Other Platforms

RC2D automates the icon generation process for macOS, Windows, and Linux platforms through a script. To generate icons for these platforms, execute the following script:

### Unix

```bash
./build-scripts/icons/generate-icons.sh
```

### Windows

```bash
sh .\build-scripts\icons\generate-icons.sh
```

::: tip NOTE
Android icons are properly placed in the `./icons/android/` directory before running the script.
:::

::: warning REMEMBER
- The script automatically handles icon generation and placement for macOS, Windows, and Linux platforms.

- iOS icons are manually placed in the `./icons/ios/` directory.

- Android icons are manually placed in the `./icons/android/` directory and the icons will then be automatically placed in: `./android-project/app/src/main/res/`.

Be sure to review the icons after generation for adjustments if necessary.
:::

By following these steps, you can ensure your game presents a consistent and professional appearance across all supported platforms.
