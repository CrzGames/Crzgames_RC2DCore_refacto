#ifndef RC2D_EVENT_H
#define RC2D_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

void rc2d_event_clear(void);
int rc2d_event_poll(void);
void rc2d_event_pump(void);
int rc2d_event_push(void);
void rc2d_event_quit(void);
int rc2d_event_wait(void);

#ifdef __cplusplus
}
#endif

#endif // RC2D_EVENT_H