
# RC2D Filesystem API Documentation

Welcome to the RC2D Filesystem API documentation. This section covers the functionalities provided by the RC2D game engine for filesystem operations, including file management and path retrieval.

## API Functions

### `rc2d_filesystem_getPrefPath`

Retrieves the preferred path for storing application data.

- `nameOrganisation`: The name of the organization.
- `nameApplication`: The name of the application.

```c
char* rc2d_filesystem_getPrefPath(const char* nameOrganisation, const char* nameApplication);
```

### `rc2d_filesystem_getBasePath`

Retrieves the base path of the application.

```c
char* rc2d_filesystem_getBasePath(void);
```

### `rc2d_filesystem_quit`

Cleans up filesystem resources.

```c
void rc2d_filesystem_quit(void);
```

### `rc2d_filesystem_append`

Appends data to a file.

- `pathFile`: The path to the file.
- `data`: The data to append.

```c
void rc2d_filesystem_append(const char* pathFile, const char* data);
```

### `rc2d_filesystem_createAndWriteFile`

Creates a new file and writes data to it.

- `pathFile`: The path to the file.
- `data`: The data to write.

```c
void rc2d_filesystem_createAndWriteFile(const char* pathFile, const char* data);
```

### `rc2d_filesystem_newFile`

Creates a new, empty file.

- `pathFile`: The path to the file.

```c
void rc2d_filesystem_newFile(const char* pathFile);
```

### `rc2d_filesystem_readFile`

Reads the contents of a file.

- `pathFile`: The path to the file.

```c
char* rc2d_filesystem_readFile(const char* pathFile);
```

### `rc2d_filesystem_fileExist`

Checks if a file exists.

- `pathFile`: The path to the file.

```c
bool rc2d_filesystem_fileExist(const char* pathFile);
```

### `rc2d_filesystem_getPathAssetsInDevice`

Retrieves the path where assets are stored on the device.

```c
const char* rc2d_filesystem_getPathAssetsInDevice(void);
```

### `rc2d_filesystem_getPathAssetsInResourceRRES`

Retrieves the path where assets are stored in RRES resources.

```c
const char* rc2d_filesystem_getPathAssetsInResourceRRES(void);
```

## Conclusion

This documentation provides an overview of the filesystem operations available in the RC2D game engine. These functions allow for efficient management of files and paths, facilitating access to application data and resources.
