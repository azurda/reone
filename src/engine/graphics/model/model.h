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

#include "../aabb.h"

#include "animation.h"

namespace reone {

namespace graphics {

/**
 * 3D model, a tree-like data structure. Contains model nodes and animations.
 *
 * @see ModelNode
 * @see Animation
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
        std::shared_ptr<ModelNode> rootNode,
        std::vector<std::shared_ptr<Animation>> animations,
        std::shared_ptr<Model> superModel,
        float animationScale);

    void init();

    bool isAffectedByFog() const { return _affectedByFog; }

    const std::string &name() const { return _name; }
    Classification classification() const { return _classification; }
    std::shared_ptr<ModelNode> rootNode() const { return _rootNode; }
    std::shared_ptr<Model> superModel() const { return _superModel; }
    float animationScale() const { return _animationScale; }
    const AABB &aabb() const { return _aabb; }

    void setAffectedByFog(bool affected) { _affectedByFog = affected; }

    // Nodes

    std::shared_ptr<ModelNode> getNodeByName(const std::string &name) const;
    std::shared_ptr<ModelNode> getNodeByNameRecursive(const std::string &name) const;
    std::shared_ptr<ModelNode> getAABBNode() const;
    std::set<std::string> getAncestorNodes(const std::string &parentName) const;

    // END Nodes

    // Animations

    std::vector<std::string> getAnimationNames() const;
    std::shared_ptr<Animation> getAnimation(const std::string &name) const;

    // END Animations

private:
    std::string _name;
    Classification _classification;
    std::shared_ptr<ModelNode> _rootNode;
    std::unordered_map<std::string, std::shared_ptr<Animation>> _animations;
    std::shared_ptr<Model> _superModel;
    float _animationScale; /**< scales supermodel animations */

    AABB _aabb;
    bool _affectedByFog;
    std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nodeByName;

    void fillNodeByName(const std::shared_ptr<ModelNode> &node);
    void computeAABB();
};

} // namespace graphics

} // namespace reone
