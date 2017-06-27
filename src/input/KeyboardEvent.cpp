#include "input/KeyboardEvent.h"

KeyboardEvent::KeyboardEvent(KeyEvent type, Key key, std::time_t timestamp)
    : type(type)
    , key(key)
    , timestamp(timestamp)
{
}
