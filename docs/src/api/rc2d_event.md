
# RC2D Event API Documentation

Welcome to the updated RC2D Event API documentation. This section covers the functionalities provided by the RC2D game engine for event handling. The API leverages SDL's capabilities for managing game events such as input, window actions, and other interactive elements.

## API Functions

### `rc2d_event_quit`

Gracefully terminates the event handling system.

```c
void rc2d_event_quit(void);
```

### `rc2d_event_clear`

Clears the event queue, removing all pending events.

```c
void rc2d_event_clear(void);
```

### `rc2d_event_pump`

Pumps the event loop, gathering events from the input devices and updating the event queue.

```c
void rc2d_event_pump(void);
```

### `rc2d_event_poll`

Polls the event queue for any pending events, returning a flag to indicate if there are any.

```c
int rc2d_event_poll(void);
```

### `rc2d_event_push`

Pushes a new event onto the event queue. (Note: In the updated API, specific event details are managed internally.)

```c
void rc2d_event_push(void);
```

### `rc2d_event_wait`

Waits indefinitely for an event to be available in the event queue.

```c
void rc2d_event_wait(void);
```

## Conclusion

This documentation has been updated to reflect the changes in the RC2D event handling API. These functions provide a comprehensive framework for managing game events, ensuring your game can handle input and other interactive elements effectively and responsively.
