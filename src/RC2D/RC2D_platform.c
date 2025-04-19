const char* rc2d_system_getPlatform(void)
{
#if defined(RC2D_PLATFORM_XBOXSERIES)
    return "Xbox Series";
#elif defined(RC2D_PLATFORM_XBOXONE)
    return "Xbox One";
#elif defined(RC2D_PLATFORM_WIN32)
    return "Windows";
#elif defined(RC2D_PLATFORM_WINGDK)
    return "Windows GDK";
#elif defined(RC2D_PLATFORM_GDK)
    return "Microsoft GDK";
#elif defined(RC2D_PLATFORM_MACOS)
    return "macOS";
#elif defined(RC2D_PLATFORM_IOS)
    return "iOS";
#elif defined(RC2D_PLATFORM_TVOS)
    return "tvOS";
#elif defined(RC2D_PLATFORM_VISIONOS)
    return "visionOS";
#elif defined(RC2D_PLATFORM_ANDROID)
    return "Android";
#elif defined(RC2D_PLATFORM_LINUX)
    return "Linux";
#elif defined(RC2D_PLATFORM_FREEBSD)
    return "FreeBSD";
#elif defined(RC2D_PLATFORM_NETBSD)
    return "NetBSD";
#elif defined(RC2D_PLATFORM_OPENBSD)
    return "OpenBSD";
#elif defined(RC2D_PLATFORM_UNIX)
    return "Unix";
#else
    return "Unknown";
#endif
}