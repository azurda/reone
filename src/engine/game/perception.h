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

#include "../common/timer.h"

#include "object/creature.h"

namespace reone {

namespace game {

class Area;

/**
 * Creature perception routines, i.e. sight and hearing.
 */
class Perception {
public:
    Perception(Area *area);

    void update(float dt);

private:
    Area *_area;

    Timer _updateTimer;

    void doUpdate();

    bool isInLineOfSight(const Creature &subject, const SpatialObject &target) const;
};

} // namespace game

} // namespace reone
