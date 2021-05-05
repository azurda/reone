/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <stdexcept>

#include "action.h"

namespace reone {

namespace game {

class Object;

class ObjectAction : public Action {
public:
    ObjectAction(ActionType type, std::shared_ptr<Object> object) : Action(type), _object(std::move(object)) {
        if (!_object) {
            throw std::invalid_argument("object must not be null");
        }
    }

    std::shared_ptr<Object> object() const { return _object; }

protected:
    std::shared_ptr<Object> _object;
};

} // namespace game

} // namespace reone
