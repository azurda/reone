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

#include "../resource/resources.h"

#include "material.h"

namespace reone {

namespace graphics {

class Materials : boost::noncopyable {
public:
    Materials(resource::Resources &resources);
    ~Materials();

    void init();
    void deinit();

    /**
     * Retrieves material properties by diffuse texture ResRef.
     */
    std::shared_ptr<Material> get(const std::string &texResRef) const;

private:
    resource::Resources &_resources;

    bool _inited { false };
    std::unordered_map<std::string, std::shared_ptr<Material>> _materials;
};

} // namespace graphics

} // namespace reone
