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

#include "creaturefinder.h"

#include <stdexcept>

#include "object/area.h"
#include "reputes.h"

using namespace std;

namespace reone {

namespace game {

CreatureFinder::CreatureFinder(Area *area) : _area(area) {
    if (!area) {
        throw invalid_argument("area must not be null");
    }
}

shared_ptr<Creature> CreatureFinder::getNearestCreature(const std::shared_ptr<SpatialObject> &target, const CreatureFinder::CriteriaList &criterias, int nth) {
    vector<pair<shared_ptr<Creature>, float>> candidates;

    for (auto &object : _area->getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(object);
        if (matchesCriterias(*creature, criterias, target)) {
            float distance2 = creature->getDistanceTo2(*target);
            candidates.push_back(make_pair(move(creature), distance2));
        }
    }

    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    return nth < candidates.size() ? candidates[nth].first : nullptr;
}

bool CreatureFinder::matchesCriterias(const Creature &creature, const CriteriaList &criterias, std::shared_ptr<SpatialObject> target) const {
    for (auto &criteria : criterias) {
        switch (criteria.first) {
            case CreatureType::Reputation: {
                auto reputation = static_cast<ReputationType>(criteria.second);
                switch (reputation) {
                    case ReputationType::Friend:
                        if (!target || !Reputes::instance().getIsFriend(creature, *static_pointer_cast<Creature>(target))) return false;
                        break;
                    case ReputationType::Enemy:
                        if (!target || !Reputes::instance().getIsEnemy(creature, *static_pointer_cast<Creature>(target))) return false;
                        break;
                    case ReputationType::Neutral:
                        if (!target || !Reputes::instance().getIsNeutral(creature, *static_pointer_cast<Creature>(target))) return false;
                        break;
                    default:
                        break;
                }
                break;
            }
            case CreatureType::Perception: {
                if (!target) return false;

                bool seen = creature.perception().seen.count(target) > 0;
                bool heard = creature.perception().heard.count(target) > 0;
                bool matches = false;
                auto perception = static_cast<PerceptionType>(criteria.second);
                switch (perception) {
                    case PerceptionType::SeenAndHeard:
                        matches = seen && heard;
                        break;
                    case PerceptionType::NotSeenAndNotHeard:
                        matches = !seen && !heard;
                        break;
                    case PerceptionType::HeardAndNotSeen:
                        matches = heard && !seen;
                        break;
                    case PerceptionType::SeenAndNotHeard:
                        matches = seen && !heard;
                        break;
                    case PerceptionType::NotHeard:
                        matches = !heard;
                        break;
                    case PerceptionType::Heard:
                        matches = heard;
                        break;
                    case PerceptionType::NotSeen:
                        matches = !seen;
                        break;
                    case PerceptionType::Seen:
                        matches = seen;
                        break;
                    default:
                        break;
                }
                if (!matches) return false;
                break;
            }
            default:
                // TODO: implement other criterias
                break;
        }
    }

    return true;
}

shared_ptr<Creature> CreatureFinder::getNearestCreatureToLocation(const Location &location, const CreatureFinder::CriteriaList &criterias, int nth) {
    vector<pair<shared_ptr<Creature>, float>> candidates;

    for (auto &object : _area->getObjectsByType(ObjectType::Creature)) {
        auto creature = static_pointer_cast<Creature>(object);
        if (matchesCriterias(*creature, criterias)) {
            float distance2 = creature->getDistanceTo2(location.position());
            candidates.push_back(make_pair(move(creature), distance2));
        }
    }

    sort(candidates.begin(), candidates.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });

    return nth < candidates.size() ? candidates[nth].first : nullptr;
}

} // namespace game

} // namespace reone
