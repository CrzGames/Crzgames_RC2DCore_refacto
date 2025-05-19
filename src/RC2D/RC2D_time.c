#include <RC2D/RC2D_time.h>
#include <RC2D/RC2D_logger.h>
#include <SDL3/SDL_time.h>

/**
 * \brief Convertit un SDL_DateTime en RC2D_DateTime.
 * 
 * Cette fonction remplit la structure RC2D_DateTime avec les valeurs
 * de l'objet SDL_DateTime fourni.
 */
static void rc2d_time_convert_sdl_datetime(const SDL_DateTime *sdl_dt, RC2D_DateTime *rc2d_dt) 
{
    if (!sdl_dt || !rc2d_dt) return;

    rc2d_dt->year = sdl_dt->year;
    rc2d_dt->month = sdl_dt->month;
    rc2d_dt->day = sdl_dt->day;
    rc2d_dt->hour = sdl_dt->hour;
    rc2d_dt->minute = sdl_dt->minute;
    rc2d_dt->second = sdl_dt->second;
    rc2d_dt->nanosecond = sdl_dt->nanosecond;
    rc2d_dt->day_of_week = sdl_dt->day_of_week;
    rc2d_dt->utc_offset = sdl_dt->utc_offset;
}

bool rc2d_time_getCurrentTime(RC2D_DateTime *datetime) 
{
    if (!datetime) 
    {
        RC2D_Log(RC2D_LOG_ERROR, "le paramètre datetime est NULL");
        return false;
    }

    SDL_Time ticks;
    if (!SDL_GetCurrentTime(&ticks)) 
    {
        RC2D_Log(RC2D_LOG_ERROR, "echec de SDL_GetCurrentTime: %s", SDL_GetError());
        return false;
    }

    SDL_DateTime sdl_dt;
    if (!SDL_TimeToDateTime(ticks, &sdl_dt, false)) 
    {
        RC2D_Log(RC2D_LOG_ERROR, "echec de SDL_TimeToDateTime: %s", SDL_GetError());
        return false;
    }

    rc2d_time_convert_sdl_datetime(&sdl_dt, datetime);
    return true;
}

bool rc2d_time_getDateTimeLocalePreferences(RC2D_DateFormat *dateFormat, RC2D_TimeFormat *timeFormat) 
{
    SDL_DateFormat sdl_date_format;
    SDL_TimeFormat sdl_time_format;

    if (!SDL_GetDateTimeLocalePreferences(&sdl_date_format, &sdl_time_format)) 
    {
        RC2D_Log(RC2D_LOG_ERROR, "echec de SDL_GetDateTimeLocalePreferences: %s", SDL_GetError());
        return false;
    }

    if (dateFormat) 
    {
        *dateFormat = (RC2D_DateFormat)sdl_date_format;
    }
    if (timeFormat) 
    {
        *timeFormat = (RC2D_TimeFormat)sdl_time_format;
    }

    return true;
}