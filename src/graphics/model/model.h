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

#include <string>
#include <unordered_map>
#include <vector>

#include <boost/noncopyable.hpp>

#include "../aabb.h"

#include "animation.h"

namespace reone {

namespace graphics {

/**
 * Tree-like data structure, representing a 3D model. Contains model nodes
 * and animations. Models are cached and reused between model scene nodes.
 *
 * @see reone::graphics::ModelNode
 * @see reone::graphics::Animation
 */
class Model : boost::noncopyable {
public:
    enum class Classification {
        Other,
        Effect,
        Tile,
        Character,
        Door,
        Lightsaber,
        Placeable,
        Flyer
    };

    Model(
        std::string name,
        Classification classification,
        float animationScale,
        std::shared_ptr<ModelNode> rootNode,
        std::vector<std::shared_ptr<Animation>> animations,
        std::shared_ptr<Model> superModel = nullptr);

    void init();

    bool isAffectedByFog() const { return _affectedByFog; }

    std::vector<std::string> getAnimationNames() const;
    std::shared_ptr<Animation> getAnimation(const std::string &name) const;
    std::shared_ptr<ModelNode> findNodeByNumber(uint16_t number) const;
    std::shared_ptr<ModelNode> findNodeByName(const std::string &name) const;
    std::shared_ptr<ModelNode> findAABBNode() const;

    const std::string &name() const { return _name; }
    Classification classification() const { return _classification; }
    float animationScale() const { return _animationScale; }
    std::shared_ptr<ModelNode> rootNode() const { return _rootNode; }
    std::shared_ptr<Model> superModel() const { return _superModel; }
    const AABB &aabb() const { return _aabb; }

    void setAffectedByFog(bool affected) { _affectedByFog = affected; }

private:
    std::string _name;
    Classification _classification;
    float _animationScale;
    std::shared_ptr<ModelNode> _rootNode;
    std::unordered_map<std::string, std::shared_ptr<Animation>> _animations;
    std::shared_ptr<Model> _superModel;

    std::unordered_map<uint16_t, std::shared_ptr<ModelNode>> _nodeByNumber;
    std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nodeByName;
    AABB _aabb;
    bool _affectedByFog { false };

    void initInternal(const std::shared_ptr<ModelNode> &node);

    friend class MdlReader;
};

} // namespace graphics

} // namespace reone
