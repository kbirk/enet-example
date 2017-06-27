#pragma once

#include "Common.h"
#include "game/StateMachine.h"
#include "serial/StreamBuffer.h"

namespace StateType {
enum Types {
    NONE,
    IDLE,
    MOVE_DIRECTION,
    MOVE_TO,
    JUMP
};
}

StreamBuffer::Shared& operator<<(StreamBuffer::Shared&, const StateMachine::Shared&);
StreamBuffer::Shared& operator>>(StreamBuffer::Shared&, StateMachine::Shared&);
