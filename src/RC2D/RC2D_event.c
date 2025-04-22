#include <RC2D/RC2D_internal.h>

void rc2d_event_quit(void)
{
	rc2d_engine_state.game_is_running = false;
}