#pragma once

#include "Common.h"
#include "input/KeyboardEnum.h"

struct KeyboardEvent {

    KeyboardEvent(KeyEvent, Key, std::time_t);

    KeyEvent type;
    Key key;
    std::time_t timestamp;
};
