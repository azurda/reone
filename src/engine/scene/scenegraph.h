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

#include <functional>
#include <memory>
#include <set>
#include <vector>

#include <boost/noncopyable.hpp>

#include "glm/vec3.hpp"

#include "../graphics/shader/shaders.h"
#include "../graphics/types.h"

#include "node/cameranode.h"
#include "node/emitternode.h"
#include "node/grassnode.h"
#include "node/lightnode.h"
#include "node/meshnode.h"

namespace reone {

namespace scene {

/**
 * Responsible for managing drawable objects and their relations.
 *
 * @see SceneNode
 */
class SceneGraph : boost::noncopyable {
public:
    SceneGraph(const graphics::GraphicsOptions &opts);

    /**
     * Recursively update the state of this scene graph. Called prior to rendering a frame.
     * This extracts drawable nodes from roots, culls and sorts objects, updates animation, lighting, shadows and etc.
     */
    void update(float dt);

    void draw(bool shadowPass = false);

    const graphics::GraphicsOptions &options() const { return _opts; }
    std::shared_ptr<CameraSceneNode> activeCamera() const { return _activeCamera; }
    graphics::ShaderUniforms uniformsPrototype() const { return _uniformsPrototype; }

    void setUpdateRoots(bool update) { _updateRoots = update; }
    void setActiveCamera(std::shared_ptr<CameraSceneNode> camera) { _activeCamera = std::move(camera); }
    void setUniformsPrototype(graphics::ShaderUniforms &&uniforms) { _uniformsPrototype = uniforms; }

    // Roots

    void clearRoots();
    void addRoot(std::shared_ptr<SceneNode> node);
    void removeRoot(const std::shared_ptr<SceneNode> &node);

    // END Roots

    // Lighting and shadows

    /**
     * Fill lights vector with up to count lights, sorted by priority and
     * proximity to the reference node.
     */
    void getLightsAt(
        const SceneNode &reference,
        std::vector<LightSceneNode *> &lights,
        int count = graphics::kMaxLights,
        std::function<bool(const LightSceneNode &)> predicate = [](auto &light) { return true; }) const;

    const glm::vec3 &ambientLightColor() const { return _ambientLightColor; }
    const std::vector<LightSceneNode *> closestLights() const { return _closestLights; }
    const LightSceneNode *shadowLight() const { return _shadowLight; }
    float shadowStrength() const { return _shadowStrength; }

    void setAmbientLightColor(glm::vec3 color) { _ambientLightColor = std::move(color); }
    void setLightingRefNode(std::shared_ptr<SceneNode> node) { _lightingRefNode = std::move(node); }

    // END Lighting and shadows

    // Fog

    bool isFogEnabled() const { return _fogEnabled; }

    float fogNear() const { return _fogNear; }
    float fogFar() const { return _fogFar; }
    const glm::vec3 &fogColor() const { return _fogColor; }

    void setFogEnabled(bool enabled) { _fogEnabled = enabled; }
    void setFogNear(float near) { _fogNear = near; }
    void setFogFar(float far) { _fogFar = far; }
    void setFogColor(glm::vec3 color) { _fogColor = std::move(color); }

    // END Fog

private:
    graphics::GraphicsOptions _opts;

    std::vector<std::shared_ptr<SceneNode>> _roots;
    std::shared_ptr<CameraSceneNode> _activeCamera;

    std::vector<MeshSceneNode *> _opaqueMeshes;
    std::vector<MeshSceneNode *> _transparentMeshes;
    std::vector<MeshSceneNode *> _shadowMeshes;
    std::vector<LightSceneNode *> _lights;
    std::vector<EmitterSceneNode *> _emitters;
    std::vector<GrassSceneNode *> _grass;
    std::vector<std::pair<EmitterSceneNode *, std::vector<EmitterSceneNode::Particle *>>> _particles;
    std::vector<std::pair<GrassSceneNode *, std::vector<GrassSceneNode::Cluster>>> _grassClusters;

    uint32_t _textureId { 0 };
    bool _updateRoots { true };
    graphics::ShaderUniforms _uniformsPrototype;

    // Lighting and shadows

    glm::vec3 _ambientLightColor { 0.5f };
    std::shared_ptr<SceneNode> _lightingRefNode; /**< reference node to use when selecting closest light sources */
    std::vector<LightSceneNode *> _closestLights;
    const LightSceneNode *_shadowLight { nullptr };
    float _shadowStrength { 1.0f };
    bool _shadowFading { false };

    // END Lighting and shadows

    // Fog

    bool _fogEnabled { false };
    float _fogNear { 0.0f };
    float _fogFar { 0.0f };
    glm::vec3 _fogColor { 0.0f };

    // END Fog

    void cullRoots();
    void updateLighting();
    void updateShadows(float dt);

    void refreshNodeLists();
    void refreshFromSceneNode(const std::shared_ptr<SceneNode> &node);
    void refreshShadowLight();

    void prepareTransparentMeshes();
    void prepareParticles();
    void prepareGrass();

    inline void flushEmitterParticles(EmitterSceneNode *emitter, std::vector<EmitterSceneNode::Particle *> &particles);
};

} // namespace scene

} // namespace reone
