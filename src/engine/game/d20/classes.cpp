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

#include "classes.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::resource;

namespace reone {

namespace game {

static const char kClassesTableResRef[] = "classes";

Classes::Classes(ResourceServices &resource) :
    MemoryCache(bind(&Classes::doGet, this, _1)),
    _resource(resource) {
}

shared_ptr<CreatureClass> Classes::doGet(ClassType type) {
    shared_ptr<TwoDA> classes(_resource.resources().get2DA(kClassesTableResRef));

    auto clazz = make_shared<CreatureClass>(type, *this, _resource);
    clazz->load(*classes, static_cast<int>(type));

    return move(clazz);
}

} // namespace game

} // namespace reone
