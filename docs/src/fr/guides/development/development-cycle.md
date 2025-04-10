
# Development Cycle

This section details the steps involved in the development cycle for projects using the RC2D Game Engine, from generating your project to running it across different platforms.

## Generate Project

Before you can start developing your game, you need to generate the project files specific to your development platform. Use the following commands based on your operating system:

### Linux
```bash
./build-scripts/generate-project/linux.sh
```

### macOS
```bash
./build-scripts/generate-project/macos.sh
```

### Windows
```bash
sh .\build-scripts\generate-project\windows.sh
```

### iOS (run in macOS)
```bash
./build-scripts/generate-project/ios.sh
```

<br /><br />


## Run Project

Once the project has been generated, you can run it using the commands provided below. These commands launch your project in a development environment, allowing you to test and debug your game.

### Linux
```bash
./build-scripts/run-project-development/linux.sh
```

### macOS
```bash
./build-scripts/run-project-development/macos.sh
```

### Windows
```bash
sh .\build-scripts\run-project-development\windows.sh
```

### Android (run in Unix)
```bash
./build-scripts/run-project-development/android.sh
```

### Android (run in Windows)
```bash
sh .\build-scripts\run-project-development\android.sh
```

### iOS (run in macOS)
```bash
./build-scripts/run-project-development/ios.sh
```

<br />

::: tip NOTE
Ensure your development environment is properly set up for each platform before running these scripts. This includes installing any required SDKs or tools specific to the platform you are developing for.
:::

By following these steps, you can streamline the development process for your RC2D projects, ensuring a smooth workflow from project generation to game testing and debugging.
