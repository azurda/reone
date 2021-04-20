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

#include "area.h"

#include <algorithm>
#include <stdexcept>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "glm/gtx/norm.hpp"

#include "../../common/log.h"
#include "../../common/random.h"
#include "../../common/streamutil.h"
#include "../../render/meshes.h"
#include "../../render/model/models.h"
#include "../../render/textures.h"
#include "../../render/walkmesh/walkmeshes.h"
#include "../../resource/format/lytreader.h"
#include "../../resource/format/visreader.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"
#include "../../scene/node/grassnode.h"
#include "../../scene/node/meshnode.h"
#include "../../scene/types.h"

#include "../enginetype/location.h"
#include "../game.h"
#include "../objectconverter.h"
#include "../room.h"

#include "objectfactory.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

namespace game {

static constexpr float kDefaultFieldOfView = 75.0f;
static constexpr float kElevationTestZ = 1024.0f;
static constexpr float kCreatureObstacleTestZ = 0.1f;
static constexpr int kMaxSoundCount = 4;
static constexpr float kGrassDensityFactor = 0.25f;

static bool g_debugPath = false;

Area::Area(uint32_t id, Game *game) :
    Object(id, ObjectType::Area),
    _game(game),
    _collisionDetector(this),
    _objectSelector(this, &game->party()),
    _actionExecutor(game),
    _map(game),
    _heartbeatTimer(kHeartbeatInterval),
    _creatureFinder(this),
    _perception(this) {

    init();
}

void Area::init() {
    const GraphicsOptions &opts = _game->options().graphics;
    _cameraAspect = opts.width / static_cast<float>(opts.height);

    _objectsByType.insert(make_pair(ObjectType::Creature, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Item, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Trigger, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Door, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::AreaOfEffect, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Waypoint, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Placeable, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Store, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Encounter, ObjectList()));
    _objectsByType.insert(make_pair(ObjectType::Sound, ObjectList()));
}

void Area::load(const string &name, const GffStruct &are, const GffStruct &git) {
    _name = name;

    loadLYT();
    loadVIS();
    loadPTH();
    loadARE(are);
    loadGIT(git);
}

void Area::loadLYT() {
    LytReader lyt;
    lyt.load(wrap(Resources::instance().getRaw(_name, ResourceType::Lyt)));

    for (auto &lytRoom : lyt.rooms()) {
        shared_ptr<Model> model(Models::instance().get(lytRoom.name));
        if (!model) continue;

        glm::vec3 position(lytRoom.position.x, lytRoom.position.y, lytRoom.position.z);
        shared_ptr<Walkmesh> walkmesh(Walkmeshes::instance().get(lytRoom.name, ResourceType::Wok));

        auto sceneNode = make_shared<ModelSceneNode>(ModelUsage::Room, model, &_game->sceneGraph());
        sceneNode->setWalkmesh(walkmesh);
        sceneNode->setLocalTransform(glm::translate(glm::mat4(1.0f), position));
        for (auto &anim : model->getAnimationNames()) {
            if (boost::starts_with(anim, "animloop")) {
                sceneNode->animator().playAnimation(anim, AnimationProperties::fromFlags(AnimationFlags::loopOverlay));
            }
        }

        auto room = make_unique<Room>(lytRoom.name, position, sceneNode, walkmesh);
        _rooms.insert(make_pair(room->name(), move(room)));
    }
}

void Area::loadVIS() {
    VisReader vis;
    vis.load(wrap(Resources::instance().getRaw(_name, ResourceType::Vis)));

    _visibility = fixVisibility(vis.visibility());
}

Visibility Area::fixVisibility(const Visibility &visibility) {
    Visibility result;
    for (auto &pair : visibility) {
        result.insert(pair);
        result.insert(make_pair(pair.second, pair.first));
    }
    return move(result);
}

void Area::loadPTH() {
    shared_ptr<GffStruct> pth(Resources::instance().getGFF(_name, ResourceType::Pth));
    if (!pth) return;

    Path path;
    path.load(*pth);

    const vector<Path::Point> &points = path.points();
    unordered_map<int, float> pointZ;

    for (int i = 0; i < points.size(); ++i) {
        const Path::Point &point = points[i];
        Room *room = nullptr;
        float z = 0.0f;

        if (!getElevationAt(glm::vec2(point.x, point.y), room, z)) {
            warn(boost::format("Area: point %d elevation not found") % i);
            continue;
        }
        pointZ.insert(make_pair(i, z));
    }

    _pathfinder.load(points, pointZ);
}

void Area::initCameras(const glm::vec3 &entryPosition, float entryFacing) {
    glm::vec3 position(entryPosition);
    position.z += 1.7f;

    SceneGraph *sceneGraph = &_game->sceneGraph();

    _firstPersonCamera = make_unique<FirstPersonCamera>(sceneGraph, _cameraAspect, glm::radians(kDefaultFieldOfView));
    _firstPersonCamera->setPosition(position);
    _firstPersonCamera->setFacing(entryFacing);

    _thirdPersonCamera = make_unique<ThirdPersonCamera>(_game, sceneGraph, _cameraAspect, _camStyleDefault);
    _thirdPersonCamera->setFindObstacle(bind(&Area::getCameraObstacle, this, _1, _2, _3));
    _thirdPersonCamera->setTargetPosition(position);
    _thirdPersonCamera->setFacing(entryFacing);

    _dialogCamera = make_unique<DialogCamera>(sceneGraph, _camStyleDefault, _cameraAspect);
    _dialogCamera->setFindObstacle(bind(&Area::getCameraObstacle, this, _1, _2, _3));

    _animatedCamera = make_unique<AnimatedCamera>(sceneGraph, _cameraAspect);
    _staticCamera = make_unique<StaticCamera>(sceneGraph, _cameraAspect);
}

bool Area::getCameraObstacle(const glm::vec3 &origin, const glm::vec3 &dest, glm::vec3 &intersection) const {
    // Check whether camera is blocked by room and door walkmeshes, and return the intersection point

    glm::vec3 originToDest(dest - origin);
    glm::vec3 dir(glm::normalize(originToDest));

    RaycastProperties props;
    props.flags = RaycastFlags::rooms;
    props.origin = origin;
    props.direction = dir;
    props.objectTypes = { ObjectType::Door };

    RaycastResult result;

    if (_collisionDetector.raycast(props, result)) {
        float dist = glm::min(result.distance, glm::length(originToDest));
        intersection = origin + dist * dir;
        return true;
    }

    return false;
}

bool Area::getCreatureObstacle(const Creature &creature, const glm::vec3 &dest) const {
    glm::vec3 creatureToDest(dest - creature.position());
    glm::vec3 dir(glm::normalize(creatureToDest));

    glm::vec3 origin(creature.position());
    origin.z += kCreatureObstacleTestZ;

    float maxObstacleDist = glm::max(glm::length(creatureToDest), 0.5f);

    // Check whether creatures movement is blocked by AABB of other (alive) creatures
    {
        RaycastProperties props;
        props.flags = RaycastFlags::aabb | RaycastFlags::alive;
        props.origin = origin;
        props.direction = dir;
        props.objectTypes = { ObjectType::Creature };
        props.except = &creature;

        RaycastResult result;

        if (_collisionDetector.raycast(props, result) &&
            result.distance <= maxObstacleDist) return true;
    }

    // Check whether creatures movement is blocked by room and door walkmeshes
    {
        RaycastProperties props;
        props.flags = RaycastFlags::rooms;
        props.origin = origin;
        props.direction = dir;
        props.objectTypes = { ObjectType::Door };

        RaycastResult result;

        if (_collisionDetector.raycast(props, result) &&
            result.distance <= maxObstacleDist) return true;
    }

    return false;
}

void Area::add(const shared_ptr<SpatialObject> &object) {
    _objects.push_back(object);
    _objectsByType[object->type()].push_back(object);
    _objectById[object->id()] = object;
    _objectsByTag[object->tag()].push_back(object);

    determineObjectRoom(*object);
}

void Area::determineObjectRoom(SpatialObject &object) {
    glm::vec3 position(object.position());
    Room *room = nullptr;

    if (getElevationAt(position, room, position.z)) {
        object.setRoom(room);
    }
}

void Area::doDestroyObjects() {
    for (auto &object : _objectsToDestroy) {
        doDestroyObject(object);
    }
    _objectsToDestroy.clear();
}

void Area::doDestroyObject(uint32_t objectId) {
    shared_ptr<SpatialObject> object(getObjectById(objectId));
    if (!object) return;
    {
        Room *room = object->room();
        if (room) {
            room->removeTenant(object.get());
        }
    }
    {
        shared_ptr<ModelSceneNode> sceneNode(object->getModelSceneNode());
        if (sceneNode) {
            _game->sceneGraph().removeRoot(sceneNode);
        }
    }
    {
        auto maybeObject = find_if(_objects.begin(), _objects.end(), [&object](auto &o) { return o.get() == object.get(); });
        if (maybeObject != _objects.end()) {
            _objects.erase(maybeObject);
        }
    }
    _objectById.erase(objectId);
    {
        auto maybeTagObjects = _objectsByTag.find(object->tag());
        if (maybeTagObjects != _objectsByTag.end()) {
            ObjectList &tagObjects = maybeTagObjects->second;
            auto maybeObject = find_if(tagObjects.begin(), tagObjects.end(), [&object](auto &o) { return o.get() == object.get(); });
            if (maybeObject != tagObjects.end()) {
                tagObjects.erase(maybeObject);
            }
            if (tagObjects.empty()) {
                _objectsByTag.erase(maybeTagObjects);
            }
        }
    }
    {
        ObjectList &typeObjects = _objectsByType.find(object->type())->second;
        auto maybeObject = find_if(typeObjects.begin(), typeObjects.end(), [&object](auto &o) { return o.get() == object.get(); });
        if (maybeObject != typeObjects.end()) {
            typeObjects.erase(maybeObject);
        }
    }
}

shared_ptr<SpatialObject> Area::getObjectById(uint32_t id) const {
    auto object = _objectById.find(id);
    if (object == _objectById.end()) return nullptr;

    return object->second;
}

shared_ptr<SpatialObject> Area::getObjectByTag(const string &tag, int nth) const {
    auto objects = _objectsByTag.find(tag);
    if (objects == _objectsByTag.end()) return nullptr;
    if (nth >= objects->second.size()) return nullptr;

    return objects->second[nth];
}

ObjectList &Area::getObjectsByType(ObjectType type) {
    return _objectsByType.find(type)->second;
}

void Area::landObject(SpatialObject &object) {
    glm::vec3 position(object.position());
    Room *room = nullptr;

    if (getElevationAt(position, room, position.z, true, &object)) {
        object.setPosition(position);
        return;
    }
    for (int i = 0; i < 4; ++i) {
        float angle = i * glm::half_pi<float>();
        position = object.position() + glm::vec3(glm::sin(angle), glm::cos(angle), 0.0f);

        if (getElevationAt(position, room, position.z, true, &object)) {
            object.setPosition(position);
            return;
        }
    }
}

void Area::loadParty(const glm::vec3 &position, float facing) {
    Party &party = _game->party();

    for (int i = 0; i < party.getSize(); ++i) {
        shared_ptr<Creature> member(party.getMember(i));
        member->setPosition(position);
        member->setFacing(facing);

        landObject(*member);
        add(member);
    }
}

void Area::unloadParty() {
    Party &party = _game->party();

    for (int i = 0; i < party.getSize(); ++i) {
        doDestroyObject(party.getMember(i)->id());
    }
}

void Area::reloadParty() {
    shared_ptr<Creature> player(_game->party().player());
    loadParty(player->position(), player->getFacing());

    fill(_game->sceneGraph());
}

bool Area::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        default:
            return false;
    }
}

bool Area::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_Q:
            _objectSelector.selectNext(true);
            return true;

        case SDL_SCANCODE_E:
            _objectSelector.selectNext();
            return true;

        case SDL_SCANCODE_SLASH: {
            auto object = _objectSelector.selectedObject();
            if (object) {
                printDebugInfo(*object);
            }
            return true;
        }

        default:
            return false;
    }
}

void Area::printDebugInfo(const SpatialObject &object) {
    ostringstream ss;
    ss << boost::format("tag='%s'") % object.tag();
    ss << boost::format(",pos=[%0.2f,%0.2f,%0.2f]") % object.position().x % object.position().y % object.position().z;
    ss << boost::format(",model='%s'") % object.getModelSceneNode()->getName();

    debug("Selected object: " + ss.str());
}

bool Area::getElevationAt(const glm::vec2 &position, Room *&room, float &z, bool creatures, const SpatialObject *except) const {
    // Test AABB of alive creatures
    if (creatures) {
        RaycastProperties props;
        props.flags |= RaycastFlags::aabb | RaycastFlags::alive;
        props.origin = glm::vec3(position, kElevationTestZ);
        props.direction = glm::vec3(0.0f, 0.0f, -1.0f);
        props.objectTypes = { ObjectType::Creature };
        props.except = except;
        props.distance = 2.0f * kElevationTestZ;

        RaycastResult result;

        if (_collisionDetector.raycast(props, result)) return false;
    }

    // Test non-walkable faces of placeables
    {
        RaycastProperties props;
        props.origin = glm::vec3(position, kElevationTestZ);
        props.direction = glm::vec3(0.0f, 0.0f, -1.0f);
        props.objectTypes = { ObjectType::Placeable };
        props.distance = 2.0f * kElevationTestZ;

        RaycastResult result;

        if (_collisionDetector.raycast(props, result)) return false;
    }

    // Test walkable faces of rooms
    {
        RaycastProperties props;
        props.flags = RaycastFlags::rooms | RaycastFlags::walkable;
        props.origin = glm::vec3(position, kElevationTestZ);
        props.direction = glm::vec3(0.0f, 0.0f, -1.0f);
        props.distance = 2.0f * kElevationTestZ;

        RaycastResult result;

        if (_collisionDetector.raycast(props, result)) {
            room = result.room;
            z = result.intersection.z;
            return true;
        }
    }

    return false;
}

void Area::update(float dt) {
    doDestroyObjects();
    updateVisibility();
    updateSounds();

    _objectSelector.update();

    if (!_game->isPaused()) {
        Object::update(dt);

        _actionExecutor.executeActions(_game->module()->area(), dt);

        for (auto &room : _rooms) {
            room.second->update(dt);
        }

        for (auto &object : _objects) {
            object->update(dt);

            if (!object->isDead()) {
                _actionExecutor.executeActions(object, dt);
            }
        }

        _perception.update(dt);

        updateHeartbeat(dt);
    }
}

bool Area::moveCreature(const shared_ptr<Creature> &creature, const glm::vec2 &dir, bool run, float dt) {
    float facing = -glm::atan(dir.x, dir.y);
    creature->setFacing(facing);

    float speed = run ? creature->runSpeed() : creature->walkSpeed();
    float speedDt = speed * dt;

    glm::vec3 dest(creature->position());
    dest.x += dir.x * speedDt;
    dest.y += dir.y * speedDt;

    // If obstacle is found once, try moving south east
    if (getCreatureObstacle(*creature, dest)) {
        // TODO: possibly use the intersected face normal?
        facing -= 0.75f * glm::pi<float>();
        glm::vec2 right(glm::normalize(glm::vec2(-glm::sin(facing), glm::cos(facing))));

        dest = creature->position();
        dest.x += right.x * speedDt;
        dest.y += right.y * speedDt;

        // If obstacle is found twice, abort movement
        if (getCreatureObstacle(*creature, dest)) return false;
    }

    return doMoveCreature(creature, dest);
}

bool Area::doMoveCreature(const shared_ptr<Creature> &creature, const glm::vec3 &dest) {
    float z;
    Room *room;

    if (getElevationAt(dest, room, z)) {
        const Room *oldRoom = creature->room();

        creature->setRoom(room);
        creature->setPosition(glm::vec3(dest.x, dest.y, z));

        if (creature == _game->party().getLeader()) {
            onPartyLeaderMoved(room != oldRoom);
        }

        checkTriggersIntersection(creature);

        return true;
    }

    return false;
}

bool Area::moveCreatureTowards(const shared_ptr<Creature> &creature, const glm::vec2 &dest, bool run, float dt) {
    glm::vec2 delta(dest - glm::vec2(creature->position()));
    glm::vec2 dir(glm::normalize(delta));
    return moveCreature(creature, dir, run, dt);
}

void Area::runSpawnScripts() {
    for (auto &creature : _objectsByType[ObjectType::Creature]) {
        static_cast<Creature &>(*creature).runSpawnScript();
    }
}

void Area::runOnEnterScript() {
    if (_onEnter.empty()) return;

    auto player = _game->party().player();
    if (!player) return;

    _game->scriptRunner().run(_onEnter, _id, player->id());
}

void Area::runOnExitScript() {
    if (_onExit.empty()) return;

    auto player = _game->party().player();
    if (!player) return;

    _game->scriptRunner().run(_onExit, _id, player->id());
}

shared_ptr<SpatialObject> Area::getObjectAt(int x, int y) const {
    Camera *camera = _game->getActiveCamera();
    shared_ptr<CameraSceneNode> sceneNode(camera->sceneNode());

    const GraphicsOptions &opts = _game->options().graphics;
    glm::vec4 viewport(0.0f, 0.0f, opts.width, opts.height);
    glm::vec3 fromWorld(glm::unProject(glm::vec3(x, opts.height - y, 0.0f), sceneNode->view(), sceneNode->projection(), viewport));
    glm::vec3 toWorld(glm::unProject(glm::vec3(x, opts.height - y, 1.0f), sceneNode->view(), sceneNode->projection(), viewport));

    shared_ptr<Creature> partyLeader(_game->party().getLeader());

    RaycastProperties props;
    props.flags = RaycastFlags::aabb | RaycastFlags::selectable;
    props.origin = fromWorld;
    props.direction = glm::normalize(toWorld - fromWorld);
    props.objectTypes = { ObjectType::Creature, ObjectType::Door, ObjectType::Placeable };
    props.except = partyLeader.get();
    props.distance = kSelectionDistance;

    RaycastResult result;

    if (_collisionDetector.raycast(props, result)) {
        return result.object;
    }

    return nullptr;
}

void Area::destroyObject(const SpatialObject &object) {
    _objectsToDestroy.insert(object.id());
}

// Adapted from https://math.stackexchange.com/q/18686
static glm::vec3 getRandomPointInTriangle(const Walkmesh::Face &face) {
    float r1sqrt = glm::sqrt(random(0.0f, 1.0f));
    float r2 = random(0.0f, 1.0f);

    return (1.0f - r1sqrt) * face.vertices[0] + r1sqrt * (1.0f - r2) * face.vertices[1] + r2 * r1sqrt * face.vertices[2];
}

void Area::fill(SceneGraph &sceneGraph) {
    sceneGraph.clear();

    // Area properties

    sceneGraph.setAmbientLightColor(_ambientColor);
    sceneGraph.setFogEnabled(_fogEnabled);
    sceneGraph.setFogNear(_fogNear);
    sceneGraph.setFogFar(_fogFar);
    sceneGraph.setFogColor(_fogColor);

    shared_ptr<GrassSceneNode> grass;
    if (_grass.texture) {
        grass = make_shared<GrassSceneNode>(&sceneGraph, _grass.texture, glm::vec2(_grass.quadSize));
        sceneGraph.setGrass(grass);
    }

    // Room models

    for (auto &room : _rooms) {
        shared_ptr<ModelSceneNode> sceneNode(room.second->model());
        if (sceneNode) {
            sceneGraph.addRoot(sceneNode);
        }
        if (grass) {
            shared_ptr<Walkmesh> walkmesh(room.second->walkmesh());
            if (walkmesh) {
                for (auto &face : walkmesh->grassFaces()) {
                    for (int i = 0; i < getNumGrassClusters(face); ++i) {
                        GrassCluster cluster;
                        cluster.position = getRandomPointInTriangle(face);
                        cluster.variant = getRandomGrassVariant();
                        grass->addCluster(move(cluster));
                    }
                }
            }
        }
    }

    // Objects

    for (auto &object : _objects) {
        shared_ptr<SceneNode> sceneNode(object->sceneNode());
        if (sceneNode) {
            sceneGraph.addRoot(sceneNode);
        }
    }

    // Path points

    if (g_debugPath) {
        shared_ptr<Mesh> cubeMesh(Meshes::instance().getCube());
        for (auto &vert : _pathfinder.vertices()) {
            auto cubeNode = make_shared<MeshSceneNode>(&sceneGraph, cubeMesh);
            cubeNode->setLocalTransform(glm::translate(glm::mat4(1.0f), vert));
            sceneGraph.addRoot(cubeNode);
        }
    }
}

int Area::getNumGrassClusters(const Walkmesh::Face &face) const {
    return static_cast<int>(glm::round(kGrassDensityFactor * _grass.density * face.area));
}

int Area::getRandomGrassVariant() const {
    float sum = _grass.probabilities[0] + _grass.probabilities[1] + _grass.probabilities[2] + _grass.probabilities[3];
    float val = random(0.0f, 1.0f) * sum;
    float upper = 0.0f;

    for (int i = 0; i < 3; ++i) {
        upper += _grass.probabilities[i];
        if (val < upper) return i;
    }

    return 3;
}

glm::vec3 Area::getSelectableScreenCoords(const shared_ptr<SpatialObject> &object, const glm::mat4 &projection, const glm::mat4 &view) const {
    static glm::vec4 viewport(0.0f, 0.0f, 1.0f, 1.0f);

    glm::vec3 position(object->getSelectablePosition());

    return glm::project(position, view, projection, viewport);
}

void Area::update3rdPersonCameraFacing() {
    shared_ptr<SpatialObject> partyLeader(_game->party().getLeader());
    if (!partyLeader) return;

    _thirdPersonCamera->setFacing(partyLeader->getFacing());
}

void Area::startDialog(const shared_ptr<SpatialObject> &object, const string &resRef) {
    string finalResRef(resRef);
    if (resRef.empty()) finalResRef = object->conversation();
    if (resRef.empty()) return;

    _game->startDialog(object, finalResRef);
}

void Area::onPartyLeaderMoved(bool roomChanged) {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    if (!partyLeader) return;

    if (roomChanged) {
        updateRoomVisibility();
    }
    update3rdPersonCameraTarget();
    _objectSelector.selectNearest();
}

void Area::updateRoomVisibility() {
    shared_ptr<Creature> partyLeader(_game->party().getLeader());
    Room *leaderRoom = partyLeader ? partyLeader->room() : nullptr;
    bool allVisible = _game->cameraType() != CameraType::ThirdPerson || !leaderRoom;

    if (allVisible) {
        for (auto &room : _rooms) {
            room.second->setVisible(true);
        }
    } else {
        auto adjRoomNames = _visibility.equal_range(leaderRoom->name());
        for (auto &room : _rooms) {
            // Room is visible if either of the following is true:
            // 1. party leader is not in a room
            // 2. this room is the party leaders room
            // 3. this room is adjacent to the party leaders room
            bool visible = !leaderRoom || room.second.get() == leaderRoom;
            if (!visible) {
                for (auto adjRoom = adjRoomNames.first; adjRoom != adjRoomNames.second; adjRoom++) {
                    if (adjRoom->second == room.first) {
                        visible = true;
                        break;
                    }
                }
            }
            room.second->setVisible(visible);
        }
    }
}

void Area::update3rdPersonCameraTarget() {
    shared_ptr<SpatialObject> partyLeader(_game->party().getLeader());
    if (!partyLeader) return;

    glm::vec3 position;

    if (partyLeader->getModelSceneNode()->getNodeAbsolutePosition("camerahook", position)) {
        position += partyLeader->position();
    } else {
        position = partyLeader->position();
    }
    _thirdPersonCamera->setTargetPosition(position);
}

void Area::updateVisibility() {
    if (_game->cameraType() != CameraType::ThirdPerson) {
        updateRoomVisibility();
    }
    cullObjects();
}

void Area::cullObjects() {
    static glm::vec4 viewport(-1.0f, -1.0f, 1.0f, 1.0f);

    Camera *camera = _game->getActiveCamera();
    if (!camera) return;

    shared_ptr<CameraSceneNode> cameraNode(camera->sceneNode());

    for (auto &object : _objects) {
        if (!object->visible()) continue;

        shared_ptr<ModelSceneNode> model(object->getModelSceneNode());
        if (!model) continue;

        bool culledOut = false;

        // Stunts must never be culled out
        if (!object->isStuntMode()) {
            glm::vec3 objectCenter(model->getCenterOfAABB());
            glm::vec3 cameraPosition(cameraNode->absoluteTransform()[3]);
            float distanceToCamera2 = glm::distance2(objectCenter, cameraPosition);
            float drawDistance2 = object->drawDistance() * object->drawDistance();

            // Draw distance culling
            if (distanceToCamera2 > drawDistance2) {
                culledOut = true;
            } else {
                // Frustum culling
                AABB aabb(model->aabb() * model->absoluteTransform());
                if (!cameraNode->isInFrustum(aabb)) {
                    culledOut = true;
                }
            }
        }

        model->setCulledOut(culledOut);
    }
}

void Area::updateSounds() {
    glm::vec3 refPosition;
    if (_game->cameraType() == CameraType::ThirdPerson) {
        refPosition = _game->party().getLeader()->position();
    } else {
        refPosition = _game->getActiveCamera()->sceneNode()->absoluteTransform()[3];
    }

    vector<pair<Sound *, float>> soundDistances;

    for (auto &sound : _objectsByType[ObjectType::Sound]) {
        Sound *soundPtr = static_cast<Sound *>(sound.get());
        soundPtr->setAudible(false);

        if (!soundPtr->isActive()) continue;

        float maxDist2 = soundPtr->maxDistance();
        maxDist2 *= maxDist2;

        float dist2 = soundPtr->getDistanceTo2(refPosition);
        if (dist2 > maxDist2) continue;

        soundDistances.push_back(make_pair(soundPtr, dist2));
    }

    sort(soundDistances.begin(), soundDistances.end(), [](auto &left, auto &right) {
        int leftPriority = left.first->priority();
        int rightPriority = right.first->priority();

        if (leftPriority < rightPriority) return true;
        if (leftPriority > rightPriority) return false;

        return left.second < right.second;
    });
    if (soundDistances.size() > kMaxSoundCount) {
        soundDistances.erase(soundDistances.begin() + kMaxSoundCount, soundDistances.end());
    }
    for (auto &sound : soundDistances) {
        sound.first->setAudible(true);
    }
}

void Area::checkTriggersIntersection(const shared_ptr<SpatialObject> &triggerrer) {
    glm::vec2 position2d(triggerrer->position());

    for (auto &object : _objectsByType[ObjectType::Trigger]) {
        auto trigger = static_pointer_cast<Trigger>(object);
        if (trigger->getDistanceTo2(position2d) > kDefaultRaycastDistance * kDefaultRaycastDistance) continue;
        if (trigger->isTenant(triggerrer) || !trigger->isIn(position2d)) continue;

        debug(boost::format("Area: trigger '%s' triggerred by '%s'") % trigger->tag() % triggerrer->tag());
        trigger->addTenant(triggerrer);

        if (!trigger->linkedToModule().empty()) {
            _game->scheduleModuleTransition(trigger->linkedToModule(), trigger->linkedTo());
            return;
        }
        if (!trigger->getOnEnter().empty()) {
            _game->scriptRunner().run(trigger->getOnEnter(), trigger->id(), triggerrer->id());
        }
    }
}

void Area::updateHeartbeat(float dt) {
    if (_heartbeatTimer.advance(dt)) {
        if (!_onHeartbeat.empty()) {
            _game->scriptRunner().run(_onHeartbeat, _id);
        }
        for (auto &object : _objects) {
            string heartbeat(object->getOnHeartbeat());
            if (!heartbeat.empty()) {
                _game->scriptRunner().run(heartbeat, object->id());
            }
        }
        _heartbeatTimer.reset(kHeartbeatInterval);
    }
}

Camera &Area::getCamera(CameraType type) {
    switch (type) {
        case CameraType::FirstPerson:
            return *_firstPersonCamera;
        case CameraType::ThirdPerson:
            return *_thirdPersonCamera;
        case CameraType::Static:
            return *_staticCamera;
        case CameraType::Animated:
            return *_animatedCamera;
        case CameraType::Dialog:
            return *_dialogCamera;
        default:
            throw invalid_argument("Unsupported camera type: " + to_string(static_cast<int>(type)));
    }
}

void Area::setStaticCamera(int cameraId) {
    for (auto &object : _objectsByType[ObjectType::Camera]) {
        PlaceableCamera &camera = static_cast<PlaceableCamera &>(*object);
        if (camera.cameraId() == cameraId) {
            _staticCamera->setObject(camera);
            break;
        }
    }
}

void Area::setThirdPartyCameraStyle(CameraStyleType type) {
    switch (type) {
        case CameraStyleType::Combat:
            _thirdPersonCamera->setStyle(_camStyleCombat);
            break;
        default:
            _thirdPersonCamera->setStyle(_camStyleDefault);
            break;
    }
}

void Area::setStealthXPEnabled(bool value) {
    _stealthXPEnabled = value;
}

void Area::setMaxStealthXP(int value) {
    _maxStealthXP = value;
}

void Area::setCurrentStealthXP(int value) {
    _currentStealthXP = value;
}

void Area::setStealthXPDecrement(int value) {
    _stealthXPDecrement = value;
}

void Area::setUnescapable(bool value) {
    _unescapable = value;
}

shared_ptr<Object> Area::createObject(ObjectType type, const string &blueprintResRef, const shared_ptr<Location> &location) {
    if (!location) {
        throw invalid_argument("location must not be null");
    }
    shared_ptr<Object> object;

    switch (type) {
        case ObjectType::Item: {
            auto item = _game->objectFactory().newItem();
            item->loadFromBlueprint(blueprintResRef);
            object = move(item);
            break;
        }
        case ObjectType::Creature: {
            auto creature = _game->objectFactory().newCreature();
            creature->loadFromBlueprint(blueprintResRef);
            creature->setPosition(location->position());
            creature->setFacing(location->facing());
            object = move(creature);
            break;
        }
        case ObjectType::Placeable: {
            auto placeable = _game->objectFactory().newPlaceable();
            placeable->loadFromBlueprint(blueprintResRef);
            object = move(placeable);
            break;
        }
        default:
            warn("Area: createObject: unsupported object type: " + to_string(static_cast<int>(type)));
            break;
    }
    if (!object) return nullptr;

    auto spatial = dynamic_pointer_cast<SpatialObject>(object);
    if (spatial) {
        add(spatial);
        auto model = spatial->getModelSceneNode();
        if (model) {
            _game->sceneGraph().addRoot(model);
        }
        auto creature = ObjectConverter::toCreature(spatial);
        if (creature) {
            creature->runSpawnScript();
        }
    }

    return move(object);
}

shared_ptr<SpatialObject> Area::getNearestObject(const glm::vec3 &origin, int nth, const std::function<bool(const std::shared_ptr<SpatialObject> &)> &predicate) {
    vector<pair<shared_ptr<SpatialObject>, float>> candidates;

    for (auto &object : _objects) {
        if (predicate(object)) {
            candidates.push_back(make_pair(object, object->getDistanceTo2(origin)));
        }
    }
    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) { return left.second < right.second; });

    int candidateCount = static_cast<int>(candidates.size());
    if (nth >= candidateCount) {
        debug(boost::format("Area: getNearestObject: nth is out of bounds: %d/%d") % nth % candidateCount, 2);
        return nullptr;
    }

    return candidates[nth].first;
}

} // namespace game

} // namespace reone
