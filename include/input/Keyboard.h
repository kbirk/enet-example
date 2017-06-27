#pragma once

#include "Common.h"
#include "input/Input.h"
#include "input/KeyboardEnum.h"
#include "input/KeyboardEvent.h"
#include "input/MouseEnum.h"

#include <map>
#include <memory>
#include <vector>

typedef std::function<Input::Shared(
    const KeyboardEvent&,
    const std::map<Key, KeyState>&,
    const std::map<Button, ButtonState>&)>
    KeyboardEventHandler;

class Keyboard {

public:
    typedef std::shared_ptr<Keyboard> Shared;

    Keyboard();
    virtual ~Keyboard();

    void add(KeyboardEventHandler);
    std::vector<Input::Shared> poll();

    virtual const std::map<Key, KeyState>& state() const = 0;

protected:
    void dispatchEvent(
        const KeyboardEvent&,
        const std::map<Key, KeyState>&,
        const std::map<Button, ButtonState>&);

private:
    // prevent copy-construction
    Keyboard(const Keyboard&);
    // prevent assignment
    Keyboard& operator=(const Keyboard&);

    std::vector<KeyboardEventHandler> handlers_;
    std::vector<Input::Shared> input_;
};
