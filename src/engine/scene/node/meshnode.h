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

#include "modelnodescenenode.h"

#include "../../graphics/material.h"
#include "../../graphics/materials.h"
#include "../../graphics/model/model.h"
#include "../../graphics/model/modelnode.h"
#include "../../graphics/pbribl.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/texture/textures.h"

namespace reone {

namespace scene {

class ModelSceneNode;

class MeshSceneNode : public ModelNodeSceneNode {
public:
    MeshSceneNode(
        const ModelSceneNode *model,
        std::shared_ptr<graphics::ModelNode> modelNode,
        SceneGraph *sceneGraph);

    void update(float dt) override;
    void drawSingle(bool shadowPass);

    bool shouldRender() const;
    bool shouldCastShadows() const;

    bool isTransparent() const;
    bool isSelfIlluminated() const;

    const ModelSceneNode *model() const { return _model; }

    void setDiffuseTexture(const std::shared_ptr<graphics::Texture> &texture);
    void setAlpha(float alpha) { _alpha = alpha; }
    void setSelfIllumColor(glm::vec3 color) { _selfIllumColor = std::move(color); }
    void setAppliedForce(glm::vec3 force);

private:
    struct NodeTextures {
        std::shared_ptr<graphics::Texture> diffuse;
        std::shared_ptr<graphics::Texture> lightmap;
        std::shared_ptr<graphics::Texture> envmap;
        std::shared_ptr<graphics::Texture> bumpmap;
    } _nodeTextures;

    struct DanglymeshAnimation {
        glm::vec3 force { 0.0f }; /**< net force applied to this scene node */
        glm::vec3 stride { 0.0f }; /**< how far have vertices traveled from the rest position in object space */
    } _danglymeshAnimation;

    const ModelSceneNode *_model;

    graphics::Material _material;
    glm::vec2 _uvOffset { 0.0f };
    float _bumpmapTime { 0.0f };
    int _bumpmapFrame { 0 };
    float _alpha { 1.0f };
    glm::vec3 _selfIllumColor { 0.0f };
    bool _transparent { false };

    void initTextures();

    void refreshMaterial();
    void refreshAdditionalTextures();

    bool isLightingEnabled() const;

    // Animation

    void updateUVAnimation(float dt, const graphics::ModelNode::TriangleMesh &mesh);
    void updateBumpmapAnimation(float dt, const graphics::ModelNode::TriangleMesh &mesh);
    void updateDanglyMeshAnimation(float dt, const graphics::ModelNode::TriangleMesh &mesh);

    // END Animation
};

} // namespace scene

} // namespace reone
