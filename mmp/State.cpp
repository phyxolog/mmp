#include "pch.hpp"
#include "State.hpp"

namespace mmp {
    State *State::instance() {
        if (!_instance) {
            _instance = new State();
        }

        return _instance;
    }

    State *State::_instance = 0;
}