#pragma once

#include "Common.h"
#include "input/Input.h"
#include "input/KeyboardEnum.h"
#include "input/MouseEnum.h"
#include "input/MouseEvent.h"

#include <map>
#include <memory>
#include <vector>

typedef std::function<Input::Shared(
    const MouseMoveEvent&,
    const std::map<Button, ButtonState>&,
    const std::map<Key, KeyState>&)>
    MouseMoveHandler;

typedef std::function<Input::Shared(
    const MouseButtonEvent&,
    const std::map<Button, ButtonState>&,
    const std::map<Key, KeyState>&)>
    MouseButtonHandler;

typedef std::function<Input::Shared(
    const MouseScrollEvent&,
    const std::map<Button, ButtonState>&,
    const std::map<Key, KeyState>&)>
    MouseScrollHandler;

class Mouse {

public:
    typedef std::shared_ptr<Mouse> Shared;

    Mouse();
    virtual ~Mouse();

    void add(MouseMoveHandler);
    void add(MouseButtonHandler);
    void add(MouseScrollHandler);
    std::vector<Input::Shared> poll();

    virtual const std::map<Button, ButtonState>& state() const = 0;

protected:
    void dispatchEvent(
        const MouseMoveEvent&,
        const std::map<Button, ButtonState>&,
        const std::map<Key, KeyState>&);

    void dispatchEvent(
        const MouseButtonEvent&,
        const std::map<Button, ButtonState>&,
        const std::map<Key, KeyState>&);

    void dispatchEvent(
        const MouseScrollEvent&,
        const std::map<Button, ButtonState>&,
        const std::map<Key, KeyState>&);

private:
    // prevent copy-construction
    Mouse(const Mouse&);
    // prevent assignment
    Mouse& operator=(const Mouse&);

    std::vector<MouseMoveHandler> moveHandlers_;
    std::vector<MouseButtonHandler> buttonHandlers_;
    std::vector<MouseScrollHandler> scrollHandlers_;
    std::vector<Input::Shared> input_;
};
