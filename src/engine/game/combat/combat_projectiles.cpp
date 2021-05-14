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

/** @file
 *  Projectile-related combat functions.
 */

#include "combat.h"

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

#include "../game.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr char kModelEventDetonate[] = "detonate";
static constexpr float kProjectileSpeed = 16.0f;

void Combat::fireProjectile(const shared_ptr<Creature> &attacker, const shared_ptr<SpatialObject> &target, Round &round) {
    auto attackerModel = static_pointer_cast<ModelSceneNode>(attacker->sceneNode());
    auto targetModel = static_pointer_cast<ModelSceneNode>(target->sceneNode());
    if (!attackerModel || !targetModel) return;

    shared_ptr<Item> weapon(attacker->getEquippedItem(InventorySlot::rightWeapon));
    if (!weapon) return;

    shared_ptr<Item::AmmunitionType> ammunitionType(weapon->ammunitionType());
    if (!ammunitionType) return;

    auto weaponModel = static_pointer_cast<ModelSceneNode>(attackerModel->getAttachment("rhand"));
    if (!weaponModel) return;

    // Determine projectile position
    glm::vec3 projectilePos;
    shared_ptr<ModelNode> bulletHook(weaponModel->model()->getNodeByName("bullethook"));
    if (bulletHook) {
        projectilePos = weaponModel->absoluteTransform() * glm::vec4(bulletHook->restPosition(), 1.0f);
    } else {
        projectilePos = weaponModel->getOrigin();
    }

    // Determine projectile direction
    glm::vec3 projectileTarget;
    shared_ptr<ModelNode> impact(targetModel->model()->getNodeByName("impact"));
    if (impact) {
        projectileTarget = targetModel->absoluteTransform() * glm::vec4(impact->restPosition(), 1.0f);
    } else {
        projectileTarget = targetModel->getOrigin();
    }
    round.projectileDir = glm::normalize(projectileTarget - projectilePos);

    // Create and add a projectile to the scene graph
    round.projectile = make_shared<ModelSceneNode>(ammunitionType->model, ModelUsage::Projectile, &_game->sceneGraph());
    round.projectile->signalEvent(kModelEventDetonate);
    round.projectile->setLocalTransform(glm::translate(projectilePos));
    _game->sceneGraph().addRoot(round.projectile);

    // Play shot sound, if any
    weapon->playShotSound(0, projectilePos);
}

void Combat::updateProjectile(Round &round, float dt) {
    glm::vec3 position(round.projectile->absoluteTransform()[3]);
    position += kProjectileSpeed * round.projectileDir * dt;

    float facing = glm::half_pi<float>() - glm::atan(round.projectileDir.x, round.projectileDir.y);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform *= glm::eulerAngleZ(facing);

    round.projectile->setLocalTransform(transform);
}

void Combat::resetProjectile(Round &round) {
    if (round.projectile) {
        _game->sceneGraph().removeRoot(round.projectile);
        round.projectile.reset();
    }
}

} // namespace game

} // namespace reone
