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

#include "../../common/collectionutil.h"
#include "../../resource/types.h"
#include "../../script/routine.h"
#include "../../script/types.h"
#include "../../script/variable.h"

#define REO_DECL_ROUTINE(x) script::Variable (x)(const VariablesList &args, script::ExecutionContext &ctx);

namespace reone {

namespace game {

class Creature;
class Door;
class Effect;
class Event;
class Game;
class Item;
class Location;
class Object;
class Sound;
class SpatialObject;
class Talent;

class Routines : public script::IRoutineProvider, boost::noncopyable {
public:
    Routines(Game &game);
    ~Routines();

    void init();
    void deinit();

    const script::Routine &get(int index) override;

private:
    typedef std::vector<script::VariableType> VariableTypesList;
    typedef std::vector<script::Variable> VariablesList;

    Game &_game;

    std::vector<script::Routine> _routines;

    void add(std::string name, script::VariableType retType, VariableTypesList argTypes);

    template <class T>
    void add(
        std::string name,
        script::VariableType retType,
        VariableTypesList argTypes,
        const T &fn) {

        _routines.emplace_back(std::move(name), retType, std::move(argTypes), std::bind(fn, this, std::placeholders::_1, std::placeholders::_2));
    }

    void addKotorRoutines();
    void addTslRoutines();

    // Helper functions

    bool getBool(const VariablesList &args, int index, bool defValue = false) const;
    int getInt(const VariablesList &args, int index, int defValue = 0) const;
    float getFloat(const VariablesList &args, int index, float defValue = 0.0f) const;
    std::string getString(const VariablesList &args, int index, std::string defValue = "") const;
    glm::vec3 getVector(const VariablesList &args, int index, glm::vec3 defValue = glm::vec3(0.0f)) const;
    std::shared_ptr<script::ExecutionContext> getAction(const VariablesList &args, int index) const;

    std::shared_ptr<Object> getCaller(script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getCallerAsSpatial(script::ExecutionContext &ctx) const;
    std::shared_ptr<Creature> getCallerAsCreature(script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getTriggerrer(script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getObject(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Object> getObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getSpatialObject(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<SpatialObject> getSpatialObjectOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Creature> getCreature(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Creature> getCreatureOrCaller(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Door> getDoor(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Item> getItem(const VariablesList &args, int index, script::ExecutionContext &ctx) const;
    std::shared_ptr<Sound> getSound(const VariablesList &args, int index, script::ExecutionContext &ctx) const;

    std::shared_ptr<Effect> getEffect(const VariablesList &args, int index) const;
    std::shared_ptr<Event> getEvent(const VariablesList &args, int index) const;
    std::shared_ptr<Location> getLocationEngineType(const VariablesList &args, int index) const;
    std::shared_ptr<Talent> getTalent(const VariablesList &args, int index) const;

    template <class T>
    inline T getEnum(const VariablesList &args, int index) const {
        if (isOutOfRange(args, index)) {
            throw std::out_of_range("index is out of range");
        }
        return static_cast<T>(args[index].intValue);
    }

    template <class T>
    inline T getEnum(const VariablesList &args, int index, T defValue) const {
        return isOutOfRange(args, index) ?
            std::move(defValue) :
            static_cast<T>(args[index].intValue);
    }

    // END Helper functions

    // Routine implementations

    REO_DECL_ROUTINE(abs)
    REO_DECL_ROUTINE(acos)
    REO_DECL_ROUTINE(actionAttack)
    REO_DECL_ROUTINE(actionBarkString)
    REO_DECL_ROUTINE(actionCastFakeSpellAtLocation)
    REO_DECL_ROUTINE(actionCastFakeSpellAtObject)
    REO_DECL_ROUTINE(actionCastSpellAtLocation)
    REO_DECL_ROUTINE(actionCastSpellAtObject)
    REO_DECL_ROUTINE(actionCloseDoor)
    REO_DECL_ROUTINE(actionDoCommand)
    REO_DECL_ROUTINE(actionEquipItem)
    REO_DECL_ROUTINE(actionEquipMostDamagingMelee)
    REO_DECL_ROUTINE(actionEquipMostDamagingRanged)
    REO_DECL_ROUTINE(actionEquipMostEffectiveArmor)
    REO_DECL_ROUTINE(actionFollowLeader)
    REO_DECL_ROUTINE(actionFollowOwner)
    REO_DECL_ROUTINE(actionForceFollowObject)
    REO_DECL_ROUTINE(actionForceMoveToLocation)
    REO_DECL_ROUTINE(actionForceMoveToObject)
    REO_DECL_ROUTINE(actionGiveItem)
    REO_DECL_ROUTINE(actionInteractObject)
    REO_DECL_ROUTINE(actionJumpToLocation)
    REO_DECL_ROUTINE(actionJumpToObject)
    REO_DECL_ROUTINE(actionLockObject)
    REO_DECL_ROUTINE(actionMoveAwayFromLocation)
    REO_DECL_ROUTINE(actionMoveAwayFromObject)
    REO_DECL_ROUTINE(actionMoveToLocation)
    REO_DECL_ROUTINE(actionMoveToObject)
    REO_DECL_ROUTINE(actionOpenDoor)
    REO_DECL_ROUTINE(actionPauseConversation)
    REO_DECL_ROUTINE(actionPickUpItem)
    REO_DECL_ROUTINE(actionPlayAnimation)
    REO_DECL_ROUTINE(actionPutDownItem)
    REO_DECL_ROUTINE(actionRandomWalk)
    REO_DECL_ROUTINE(actionResumeConversation)
    REO_DECL_ROUTINE(actionSpeakString)
    REO_DECL_ROUTINE(actionSpeakStringByStrRef)
    REO_DECL_ROUTINE(actionStartConversation)
    REO_DECL_ROUTINE(actionSurrenderToEnemies)
    REO_DECL_ROUTINE(actionSwitchWeapons)
    REO_DECL_ROUTINE(actionTakeItem)
    REO_DECL_ROUTINE(actionUnequipItem)
    REO_DECL_ROUTINE(actionUnlockObject)
    REO_DECL_ROUTINE(actionUseFeat)
    REO_DECL_ROUTINE(actionUseSkill)
    REO_DECL_ROUTINE(actionUseTalentAtLocation)
    REO_DECL_ROUTINE(actionUseTalentOnObject)
    REO_DECL_ROUTINE(actionWait)
    REO_DECL_ROUTINE(addAvailableNPCByObject)
    REO_DECL_ROUTINE(addAvailableNPCByTemplate)
    REO_DECL_ROUTINE(addAvailablePUPByObject)
    REO_DECL_ROUTINE(addAvailablePUPByTemplate)
    REO_DECL_ROUTINE(addBonusForcePoints)
    REO_DECL_ROUTINE(addJournalQuestEntry)
    REO_DECL_ROUTINE(addJournalWorldEntry)
    REO_DECL_ROUTINE(addJournalWorldEntryStrref)
    REO_DECL_ROUTINE(addMultiClass)
    REO_DECL_ROUTINE(addPartyMember)
    REO_DECL_ROUTINE(addPartyPuppet)
    REO_DECL_ROUTINE(addToParty)
    REO_DECL_ROUTINE(adjustAlignment)
    REO_DECL_ROUTINE(adjustCreatureAttributes)
    REO_DECL_ROUTINE(adjustCreatureSkills)
    REO_DECL_ROUTINE(adjustReputation)
    REO_DECL_ROUTINE(ambientSoundChangeDay)
    REO_DECL_ROUTINE(ambientSoundChangeNight)
    REO_DECL_ROUTINE(ambientSoundPlay)
    REO_DECL_ROUTINE(ambientSoundSetDayVolume)
    REO_DECL_ROUTINE(ambientSoundSetNightVolume)
    REO_DECL_ROUTINE(ambientSoundStop)
    REO_DECL_ROUTINE(angleToVector)
    REO_DECL_ROUTINE(applyEffectAtLocation)
    REO_DECL_ROUTINE(applyEffectToObject)
    REO_DECL_ROUTINE(asin)
    REO_DECL_ROUTINE(assignCommand)
    REO_DECL_ROUTINE(assignPUP)
    REO_DECL_ROUTINE(atan)
    REO_DECL_ROUTINE(aurPostString)
    REO_DECL_ROUTINE(awardStealthXP)
    REO_DECL_ROUTINE(barkString)
    REO_DECL_ROUTINE(beginConversation)
    REO_DECL_ROUTINE(cancelCombat)
    REO_DECL_ROUTINE(cancelPostDialogCharacterSwitch)
    REO_DECL_ROUTINE(changeFaction)
    REO_DECL_ROUTINE(changeFactionByFaction)
    REO_DECL_ROUTINE(changeItemCost)
    REO_DECL_ROUTINE(changeObjectAppearance)
    REO_DECL_ROUTINE(changeToStandardFaction)
    REO_DECL_ROUTINE(clearAllActions)
    REO_DECL_ROUTINE(clearAllEffects)
    REO_DECL_ROUTINE(cos)
    REO_DECL_ROUTINE(createItemOnFloor)
    REO_DECL_ROUTINE(createItemOnObject)
    REO_DECL_ROUTINE(createObject)
    REO_DECL_ROUTINE(creatureFlourishWeapon)
    REO_DECL_ROUTINE(cutsceneAttack)
    REO_DECL_ROUTINE(cutsceneMove)
    REO_DECL_ROUTINE(d10)
    REO_DECL_ROUTINE(d100)
    REO_DECL_ROUTINE(d12)
    REO_DECL_ROUTINE(d2)
    REO_DECL_ROUTINE(d20)
    REO_DECL_ROUTINE(d3)
    REO_DECL_ROUTINE(d4)
    REO_DECL_ROUTINE(d6)
    REO_DECL_ROUTINE(d8)
    REO_DECL_ROUTINE(decrementGlobalNumber)
    REO_DECL_ROUTINE(delayCommand)
    REO_DECL_ROUTINE(deleteJournalWorldAllEntries)
    REO_DECL_ROUTINE(deleteJournalWorldEntry)
    REO_DECL_ROUTINE(deleteJournalWorldEntryStrref)
    REO_DECL_ROUTINE(destroyObject)
    REO_DECL_ROUTINE(detonateMine)
    REO_DECL_ROUTINE(disableHealthRegen)
    REO_DECL_ROUTINE(disableMap)
    REO_DECL_ROUTINE(disableVideoEffect)
    REO_DECL_ROUTINE(displayDatapad)
    REO_DECL_ROUTINE(displayFeedBackText)
    REO_DECL_ROUTINE(displayMessageBox)
    REO_DECL_ROUTINE(doDoorAction)
    REO_DECL_ROUTINE(doPlaceableObjectAction)
    REO_DECL_ROUTINE(doSinglePlayerAutoSave)
    REO_DECL_ROUTINE(duplicateHeadAppearance)
    REO_DECL_ROUTINE(effectACDecrease)
    REO_DECL_ROUTINE(effectACIncrease)
    REO_DECL_ROUTINE(effectAbilityDecrease)
    REO_DECL_ROUTINE(effectAbilityIncrease)
    REO_DECL_ROUTINE(effectAreaOfEffect)
    REO_DECL_ROUTINE(effectAssuredDeflection)
    REO_DECL_ROUTINE(effectAssuredHit)
    REO_DECL_ROUTINE(effectAttackDecrease)
    REO_DECL_ROUTINE(effectAttackIncrease)
    REO_DECL_ROUTINE(effectBeam)
    REO_DECL_ROUTINE(effectBlasterDeflectionDecrease)
    REO_DECL_ROUTINE(effectBlasterDeflectionIncrease)
    REO_DECL_ROUTINE(effectBlind)
    REO_DECL_ROUTINE(effectBodyFuel)
    REO_DECL_ROUTINE(effectChoke)
    REO_DECL_ROUTINE(effectConcealment)
    REO_DECL_ROUTINE(effectConfused)
    REO_DECL_ROUTINE(effectCrush)
    REO_DECL_ROUTINE(effectCutSceneHorrified)
    REO_DECL_ROUTINE(effectCutSceneParalyze)
    REO_DECL_ROUTINE(effectCutSceneStunned)
    REO_DECL_ROUTINE(effectDamage)
    REO_DECL_ROUTINE(effectDamageDecrease)
    REO_DECL_ROUTINE(effectDamageForcePoints)
    REO_DECL_ROUTINE(effectDamageImmunityDecrease)
    REO_DECL_ROUTINE(effectDamageImmunityIncrease)
    REO_DECL_ROUTINE(effectDamageIncrease)
    REO_DECL_ROUTINE(effectDamageReduction)
    REO_DECL_ROUTINE(effectDamageResistance)
    REO_DECL_ROUTINE(effectDamageShield)
    REO_DECL_ROUTINE(effectDeath)
    REO_DECL_ROUTINE(effectDisguise)
    REO_DECL_ROUTINE(effectDispelMagicAll)
    REO_DECL_ROUTINE(effectDispelMagicBest)
    REO_DECL_ROUTINE(effectDroidConfused)
    REO_DECL_ROUTINE(effectDroidScramble)
    REO_DECL_ROUTINE(effectDroidStun)
    REO_DECL_ROUTINE(effectEntangle)
    REO_DECL_ROUTINE(effectFPRegenModifier)
    REO_DECL_ROUTINE(effectFactionModifier)
    REO_DECL_ROUTINE(effectForceBody)
    REO_DECL_ROUTINE(effectForceDrain)
    REO_DECL_ROUTINE(effectForceFizzle)
    REO_DECL_ROUTINE(effectForceJump)
    REO_DECL_ROUTINE(effectForcePushTargeted)
    REO_DECL_ROUTINE(effectForcePushed)
    REO_DECL_ROUTINE(effectForceResistanceDecrease)
    REO_DECL_ROUTINE(effectForceResistanceIncrease)
    REO_DECL_ROUTINE(effectForceResisted)
    REO_DECL_ROUTINE(effectForceShield)
    REO_DECL_ROUTINE(effectForceSight)
    REO_DECL_ROUTINE(effectFrightened)
    REO_DECL_ROUTINE(effectFury)
    REO_DECL_ROUTINE(effectHaste)
    REO_DECL_ROUTINE(effectHeal)
    REO_DECL_ROUTINE(effectHealForcePoints)
    REO_DECL_ROUTINE(effectHitPointChangeWhenDying)
    REO_DECL_ROUTINE(effectHorrified)
    REO_DECL_ROUTINE(effectImmunity)
    REO_DECL_ROUTINE(effectInvisibility)
    REO_DECL_ROUTINE(effectKnockdown)
    REO_DECL_ROUTINE(effectLightsaberThrow)
    REO_DECL_ROUTINE(effectLinkEffects)
    REO_DECL_ROUTINE(effectMindTrick)
    REO_DECL_ROUTINE(effectMissChance)
    REO_DECL_ROUTINE(effectModifyAttacks)
    REO_DECL_ROUTINE(effectMovementSpeedDecrease)
    REO_DECL_ROUTINE(effectMovementSpeedIncrease)
    REO_DECL_ROUTINE(effectParalyze)
    REO_DECL_ROUTINE(effectPoison)
    REO_DECL_ROUTINE(effectPsychicStatic)
    REO_DECL_ROUTINE(effectRegenerate)
    REO_DECL_ROUTINE(effectResurrection)
    REO_DECL_ROUTINE(effectSavingThrowDecrease)
    REO_DECL_ROUTINE(effectSavingThrowIncrease)
    REO_DECL_ROUTINE(effectSeeInvisible)
    REO_DECL_ROUTINE(effectSkillDecrease)
    REO_DECL_ROUTINE(effectSkillIncrease)
    REO_DECL_ROUTINE(effectSleep)
    REO_DECL_ROUTINE(effectSpellImmunity)
    REO_DECL_ROUTINE(effectSpellLevelAbsorption)
    REO_DECL_ROUTINE(effectStunned)
    REO_DECL_ROUTINE(effectTemporaryForcePoints)
    REO_DECL_ROUTINE(effectTemporaryHitpoints)
    REO_DECL_ROUTINE(effectTimeStop)
    REO_DECL_ROUTINE(effectTrueSeeing)
    REO_DECL_ROUTINE(effectVPRegenModifier)
    REO_DECL_ROUTINE(effectVisualEffect)
    REO_DECL_ROUTINE(effectWhirlWind)
    REO_DECL_ROUTINE(enableRain)
    REO_DECL_ROUTINE(enableRendering)
    REO_DECL_ROUTINE(enableVideoEffect)
    REO_DECL_ROUTINE(endGame)
    REO_DECL_ROUTINE(eventActivateItem)
    REO_DECL_ROUTINE(eventConversation)
    REO_DECL_ROUTINE(eventSpellCastAt)
    REO_DECL_ROUTINE(eventUserDefined)
    REO_DECL_ROUTINE(executeScript)
    REO_DECL_ROUTINE(exploreAreaForPlayer)
    REO_DECL_ROUTINE(exportAllCharacters)
    REO_DECL_ROUTINE(extraordinaryEffect)
    REO_DECL_ROUTINE(fabs)
    REO_DECL_ROUTINE(faceObjectAwayFromObject)
    REO_DECL_ROUTINE(feetToMeters)
    REO_DECL_ROUTINE(findSubString)
    REO_DECL_ROUTINE(floatToInt)
    REO_DECL_ROUTINE(floatToString)
    REO_DECL_ROUTINE(floatingTextStrRefOnCreature)
    REO_DECL_ROUTINE(floatingTextStringOnCreature)
    REO_DECL_ROUTINE(forceHeartbeat)
    REO_DECL_ROUTINE(fortitudeSave)
    REO_DECL_ROUTINE(getAC)
    REO_DECL_ROUTINE(getAbilityModifier)
    REO_DECL_ROUTINE(getAbilityScore)
    REO_DECL_ROUTINE(getAlignmentGoodEvil)
    REO_DECL_ROUTINE(getAppearanceType)
    REO_DECL_ROUTINE(getArea)
    REO_DECL_ROUTINE(getAreaOfEffectCreator)
    REO_DECL_ROUTINE(getAreaUnescapable)
    REO_DECL_ROUTINE(getAttackTarget)
    REO_DECL_ROUTINE(getAttemptedAttackTarget)
    REO_DECL_ROUTINE(getAttemptedMovementTarget)
    REO_DECL_ROUTINE(getAttemptedSpellTarget)
    REO_DECL_ROUTINE(getBaseItemType)
    REO_DECL_ROUTINE(getBlockingCreature)
    REO_DECL_ROUTINE(getBlockingDoor)
    REO_DECL_ROUTINE(getBonusForcePoints)
    REO_DECL_ROUTINE(getButtonMashCheck)
    REO_DECL_ROUTINE(getCasterLevel)
    REO_DECL_ROUTINE(getCategoryFromTalent)
    REO_DECL_ROUTINE(getChallengeRating)
    REO_DECL_ROUTINE(getCheatCode)
    REO_DECL_ROUTINE(getChemicalPieceValue)
    REO_DECL_ROUTINE(getChemicals)
    REO_DECL_ROUTINE(getClassByPosition)
    REO_DECL_ROUTINE(getClickingObject)
    REO_DECL_ROUTINE(getCombatActionsPending)
    REO_DECL_ROUTINE(getCommandable)
    REO_DECL_ROUTINE(getCreatureHasTalent)
    REO_DECL_ROUTINE(getCreatureMovmentType)
    REO_DECL_ROUTINE(getCreatureSize)
    REO_DECL_ROUTINE(getCreatureTalentBest)
    REO_DECL_ROUTINE(getCreatureTalentRandom)
    REO_DECL_ROUTINE(getCurrentAction)
    REO_DECL_ROUTINE(getCurrentForcePoints)
    REO_DECL_ROUTINE(getCurrentHitPoints)
    REO_DECL_ROUTINE(getCurrentStealthXP)
    REO_DECL_ROUTINE(getDamageDealtByType)
    REO_DECL_ROUTINE(getDifficultyModifier)
    REO_DECL_ROUTINE(getDistanceBetween)
    REO_DECL_ROUTINE(getDistanceBetween2D)
    REO_DECL_ROUTINE(getDistanceBetweenLocations)
    REO_DECL_ROUTINE(getDistanceBetweenLocations2D)
    REO_DECL_ROUTINE(getDistanceToObject)
    REO_DECL_ROUTINE(getDistanceToObject2D)
    REO_DECL_ROUTINE(getEffectCreator)
    REO_DECL_ROUTINE(getEffectDurationType)
    REO_DECL_ROUTINE(getEffectSpellId)
    REO_DECL_ROUTINE(getEffectSubType)
    REO_DECL_ROUTINE(getEffectType)
    REO_DECL_ROUTINE(getEncounterActive)
    REO_DECL_ROUTINE(getEncounterDifficulty)
    REO_DECL_ROUTINE(getEncounterSpawnsCurrent)
    REO_DECL_ROUTINE(getEncounterSpawnsMax)
    REO_DECL_ROUTINE(getEnteringObject)
    REO_DECL_ROUTINE(getExitingObject)
    REO_DECL_ROUTINE(getFacing)
    REO_DECL_ROUTINE(getFacingFromLocation)
    REO_DECL_ROUTINE(getFactionAverageGoodEvilAlignment)
    REO_DECL_ROUTINE(getFactionAverageLevel)
    REO_DECL_ROUTINE(getFactionAverageReputation)
    REO_DECL_ROUTINE(getFactionAverageXP)
    REO_DECL_ROUTINE(getFactionBestAC)
    REO_DECL_ROUTINE(getFactionEqual)
    REO_DECL_ROUTINE(getFactionGold)
    REO_DECL_ROUTINE(getFactionLeader)
    REO_DECL_ROUTINE(getFactionLeastDamagedMember)
    REO_DECL_ROUTINE(getFactionMostDamagedMember)
    REO_DECL_ROUTINE(getFactionMostFrequentClass)
    REO_DECL_ROUTINE(getFactionStrongestMember)
    REO_DECL_ROUTINE(getFactionWeakestMember)
    REO_DECL_ROUTINE(getFactionWorstAC)
    REO_DECL_ROUTINE(getFeatAcquired)
    REO_DECL_ROUTINE(getFirstAttacker)
    REO_DECL_ROUTINE(getFirstEffect)
    REO_DECL_ROUTINE(getFirstFactionMember)
    REO_DECL_ROUTINE(getFirstInPersistentObject)
    REO_DECL_ROUTINE(getFirstItemInInventory)
    REO_DECL_ROUTINE(getFirstObjectInArea)
    REO_DECL_ROUTINE(getFirstObjectInShape)
    REO_DECL_ROUTINE(getFirstPC)
    REO_DECL_ROUTINE(getFortitudeSavingThrow)
    REO_DECL_ROUTINE(getFoundEnemyCreature)
    REO_DECL_ROUTINE(getGameDifficulty)
    REO_DECL_ROUTINE(getGender)
    REO_DECL_ROUTINE(getGlobalBoolean)
    REO_DECL_ROUTINE(getGlobalLocation)
    REO_DECL_ROUTINE(getGlobalNumber)
    REO_DECL_ROUTINE(getGlobalString)
    REO_DECL_ROUTINE(getGoingToBeAttackedBy)
    REO_DECL_ROUTINE(getGold)
    REO_DECL_ROUTINE(getGoldPieceValue)
    REO_DECL_ROUTINE(getGoodEvilValue)
    REO_DECL_ROUTINE(getHasFeat)
    REO_DECL_ROUTINE(getHasFeatEffect)
    REO_DECL_ROUTINE(getHasInventory)
    REO_DECL_ROUTINE(getHasSkill)
    REO_DECL_ROUTINE(getHasSpell)
    REO_DECL_ROUTINE(getHasSpellEffect)
    REO_DECL_ROUTINE(getHealTarget)
    REO_DECL_ROUTINE(getHitDice)
    REO_DECL_ROUTINE(getIdFromTalent)
    REO_DECL_ROUTINE(getIdentified)
    REO_DECL_ROUTINE(getInfluence)
    REO_DECL_ROUTINE(getInventoryDisturbItem)
    REO_DECL_ROUTINE(getInventoryDisturbType)
    REO_DECL_ROUTINE(getIsConversationActive)
    REO_DECL_ROUTINE(getIsDawn)
    REO_DECL_ROUTINE(getIsDay)
    REO_DECL_ROUTINE(getIsDead)
    REO_DECL_ROUTINE(getIsDebilitated)
    REO_DECL_ROUTINE(getIsDoorActionPossible)
    REO_DECL_ROUTINE(getIsDusk)
    REO_DECL_ROUTINE(getIsEffectValid)
    REO_DECL_ROUTINE(getIsEncounterCreature)
    REO_DECL_ROUTINE(getIsEnemy)
    REO_DECL_ROUTINE(getIsFriend)
    REO_DECL_ROUTINE(getIsImmune)
    REO_DECL_ROUTINE(getIsInCombat)
    REO_DECL_ROUTINE(getIsInConversation)
    REO_DECL_ROUTINE(getIsLinkImmune)
    REO_DECL_ROUTINE(getIsListening)
    REO_DECL_ROUTINE(getIsLiveContentAvailable)
    REO_DECL_ROUTINE(getIsNeutral)
    REO_DECL_ROUTINE(getIsNight)
    REO_DECL_ROUTINE(getIsObjectValid)
    REO_DECL_ROUTINE(getIsOpen)
    REO_DECL_ROUTINE(getIsPC)
    REO_DECL_ROUTINE(getIsPartyLeader)
    REO_DECL_ROUTINE(getIsPlaceableObjectActionPossible)
    REO_DECL_ROUTINE(getIsPlayableRacialType)
    REO_DECL_ROUTINE(getIsPlayerMadeCharacter)
    REO_DECL_ROUTINE(getIsPoisoned)
    REO_DECL_ROUTINE(getIsPuppet)
    REO_DECL_ROUTINE(getIsTalentValid)
    REO_DECL_ROUTINE(getIsTrapped)
    REO_DECL_ROUTINE(getIsWeaponEffective)
    REO_DECL_ROUTINE(getIsXBox)
    REO_DECL_ROUTINE(getItemACValue)
    REO_DECL_ROUTINE(getItemActivated)
    REO_DECL_ROUTINE(getItemActivatedTarget)
    REO_DECL_ROUTINE(getItemActivatedTargetLocation)
    REO_DECL_ROUTINE(getItemActivator)
    REO_DECL_ROUTINE(getItemComponent)
    REO_DECL_ROUTINE(getItemComponentPieceValue)
    REO_DECL_ROUTINE(getItemHasItemProperty)
    REO_DECL_ROUTINE(getItemInSlot)
    REO_DECL_ROUTINE(getItemPossessedBy)
    REO_DECL_ROUTINE(getItemPossessor)
    REO_DECL_ROUTINE(getItemStackSize)
    REO_DECL_ROUTINE(getJournalEntry)
    REO_DECL_ROUTINE(getJournalQuestExperience)
    REO_DECL_ROUTINE(getLastAssociateCommand)
    REO_DECL_ROUTINE(getLastAttackAction)
    REO_DECL_ROUTINE(getLastAttackMode)
    REO_DECL_ROUTINE(getLastAttackResult)
    REO_DECL_ROUTINE(getLastAttackType)
    REO_DECL_ROUTINE(getLastAttacker)
    REO_DECL_ROUTINE(getLastClosedBy)
    REO_DECL_ROUTINE(getLastCombatFeatUsed)
    REO_DECL_ROUTINE(getLastConversation)
    REO_DECL_ROUTINE(getLastDamager)
    REO_DECL_ROUTINE(getLastDisarmed)
    REO_DECL_ROUTINE(getLastDisturbed)
    REO_DECL_ROUTINE(getLastForcePowerUsed)
    REO_DECL_ROUTINE(getLastForfeitViolation)
    REO_DECL_ROUTINE(getLastHostileActor)
    REO_DECL_ROUTINE(getLastHostileTarget)
    REO_DECL_ROUTINE(getLastItemEquipped)
    REO_DECL_ROUTINE(getLastKiller)
    REO_DECL_ROUTINE(getLastLocked)
    REO_DECL_ROUTINE(getLastOpenedBy)
    REO_DECL_ROUTINE(getLastPazaakResult)
    REO_DECL_ROUTINE(getLastPerceived)
    REO_DECL_ROUTINE(getLastPerceptionHeard)
    REO_DECL_ROUTINE(getLastPerceptionInaudible)
    REO_DECL_ROUTINE(getLastPerceptionSeen)
    REO_DECL_ROUTINE(getLastPerceptionVanished)
    REO_DECL_ROUTINE(getLastPlayerDied)
    REO_DECL_ROUTINE(getLastPlayerDying)
    REO_DECL_ROUTINE(getLastRespawnButtonPresser)
    REO_DECL_ROUTINE(getLastSpeaker)
    REO_DECL_ROUTINE(getLastSpell)
    REO_DECL_ROUTINE(getLastSpellCaster)
    REO_DECL_ROUTINE(getLastSpellHarmful)
    REO_DECL_ROUTINE(getLastTrapDetected)
    REO_DECL_ROUTINE(getLastUnlocked)
    REO_DECL_ROUTINE(getLastUsedBy)
    REO_DECL_ROUTINE(getLastWeaponUsed)
    REO_DECL_ROUTINE(getLevelByClass)
    REO_DECL_ROUTINE(getLevelByPosition)
    REO_DECL_ROUTINE(getListenPatternNumber)
    REO_DECL_ROUTINE(getLoadFromSaveGame)
    REO_DECL_ROUTINE(getLocalBoolean)
    REO_DECL_ROUTINE(getLocalNumber)
    REO_DECL_ROUTINE(getLocation)
    REO_DECL_ROUTINE(getLockKeyRequired)
    REO_DECL_ROUTINE(getLockKeyTag)
    REO_DECL_ROUTINE(getLockLockDC)
    REO_DECL_ROUTINE(getLockLockable)
    REO_DECL_ROUTINE(getLockUnlockDC)
    REO_DECL_ROUTINE(getLocked)
    REO_DECL_ROUTINE(getMatchedSubstring)
    REO_DECL_ROUTINE(getMatchedSubstringsCount)
    REO_DECL_ROUTINE(getMaxForcePoints)
    REO_DECL_ROUTINE(getMaxHitPoints)
    REO_DECL_ROUTINE(getMaxStealthXP)
    REO_DECL_ROUTINE(getMetaMagicFeat)
    REO_DECL_ROUTINE(getMinOneHP)
    REO_DECL_ROUTINE(getModule)
    REO_DECL_ROUTINE(getModuleFileName)
    REO_DECL_ROUTINE(getModuleItemAcquired)
    REO_DECL_ROUTINE(getModuleItemAcquiredFrom)
    REO_DECL_ROUTINE(getModuleItemLost)
    REO_DECL_ROUTINE(getModuleItemLostBy)
    REO_DECL_ROUTINE(getModuleName)
    REO_DECL_ROUTINE(getMovementRate)
    REO_DECL_ROUTINE(getNPCAIStyle)
    REO_DECL_ROUTINE(getNPCSelectability)
    REO_DECL_ROUTINE(getName)
    REO_DECL_ROUTINE(getNearestCreature)
    REO_DECL_ROUTINE(getNearestCreatureToLocation)
    REO_DECL_ROUTINE(getNearestObject)
    REO_DECL_ROUTINE(getNearestObjectByTag)
    REO_DECL_ROUTINE(getNearestObjectToLocation)
    REO_DECL_ROUTINE(getNearestTrapToObject)
    REO_DECL_ROUTINE(getNextAttacker)
    REO_DECL_ROUTINE(getNextEffect)
    REO_DECL_ROUTINE(getNextFactionMember)
    REO_DECL_ROUTINE(getNextInPersistentObject)
    REO_DECL_ROUTINE(getNextItemInInventory)
    REO_DECL_ROUTINE(getNextObjectInArea)
    REO_DECL_ROUTINE(getNextObjectInShape)
    REO_DECL_ROUTINE(getNextPC)
    REO_DECL_ROUTINE(getNumStackedItems)
    REO_DECL_ROUTINE(getObjectByTag)
    REO_DECL_ROUTINE(getObjectHeard)
    REO_DECL_ROUTINE(getObjectPersonalSpace)
    REO_DECL_ROUTINE(getObjectSeen)
    REO_DECL_ROUTINE(getObjectType)
    REO_DECL_ROUTINE(getOwnerDemolitionsSkill)
    REO_DECL_ROUTINE(getPCLevellingUp)
    REO_DECL_ROUTINE(getPCSpeaker)
    REO_DECL_ROUTINE(getPUPOwner)
    REO_DECL_ROUTINE(getPartyAIStyle)
    REO_DECL_ROUTINE(getPartyLeader)
    REO_DECL_ROUTINE(getPartyMemberByIndex)
    REO_DECL_ROUTINE(getPartyMemberCount)
    REO_DECL_ROUTINE(getPlaceableIllumination)
    REO_DECL_ROUTINE(getPlanetAvailable)
    REO_DECL_ROUTINE(getPlanetSelectable)
    REO_DECL_ROUTINE(getPlayerRestrictMode)
    REO_DECL_ROUTINE(getPlotFlag)
    REO_DECL_ROUTINE(getPosition)
    REO_DECL_ROUTINE(getPositionFromLocation)
    REO_DECL_ROUTINE(getRacialSubType)
    REO_DECL_ROUTINE(getRacialType)
    REO_DECL_ROUTINE(getRandomDestination)
    REO_DECL_ROUTINE(getReflexAdjustedDamage)
    REO_DECL_ROUTINE(getReflexSavingThrow)
    REO_DECL_ROUTINE(getReputation)
    REO_DECL_ROUTINE(getRunScriptVar)
    REO_DECL_ROUTINE(getScriptParameter)
    REO_DECL_ROUTINE(getScriptStringParameter)
    REO_DECL_ROUTINE(getSelectedPlanet)
    REO_DECL_ROUTINE(getSkillRank)
    REO_DECL_ROUTINE(getSkillRankBase)
    REO_DECL_ROUTINE(getSoloMode)
    REO_DECL_ROUTINE(getSpellAcquired)
    REO_DECL_ROUTINE(getSpellBaseForcePointCost)
    REO_DECL_ROUTINE(getSpellCastItem)
    REO_DECL_ROUTINE(getSpellForcePointCost)
    REO_DECL_ROUTINE(getSpellFromMask)
    REO_DECL_ROUTINE(getSpellId)
    REO_DECL_ROUTINE(getSpellSaveDC)
    REO_DECL_ROUTINE(getSpellTarget)
    REO_DECL_ROUTINE(getSpellTargetLocation)
    REO_DECL_ROUTINE(getSpellTargetObject)
    REO_DECL_ROUTINE(getStandardFaction)
    REO_DECL_ROUTINE(getStartingLocation)
    REO_DECL_ROUTINE(getStealthXPDecrement)
    REO_DECL_ROUTINE(getStealthXPEnabled)
    REO_DECL_ROUTINE(getStrRefSoundDuration)
    REO_DECL_ROUTINE(getStringByStrRef)
    REO_DECL_ROUTINE(getStringLeft)
    REO_DECL_ROUTINE(getStringLength)
    REO_DECL_ROUTINE(getStringLowerCase)
    REO_DECL_ROUTINE(getStringRight)
    REO_DECL_ROUTINE(getStringUpperCase)
    REO_DECL_ROUTINE(getSubRace)
    REO_DECL_ROUTINE(getSubScreenID)
    REO_DECL_ROUTINE(getSubString)
    REO_DECL_ROUTINE(getTag)
    REO_DECL_ROUTINE(getTimeHour)
    REO_DECL_ROUTINE(getTimeMillisecond)
    REO_DECL_ROUTINE(getTimeMinute)
    REO_DECL_ROUTINE(getTimeSecond)
    REO_DECL_ROUTINE(getTotalDamageDealt)
    REO_DECL_ROUTINE(getTransitionTarget)
    REO_DECL_ROUTINE(getTrapBaseType)
    REO_DECL_ROUTINE(getTrapCreator)
    REO_DECL_ROUTINE(getTrapDetectDC)
    REO_DECL_ROUTINE(getTrapDetectable)
    REO_DECL_ROUTINE(getTrapDetectedBy)
    REO_DECL_ROUTINE(getTrapDisarmDC)
    REO_DECL_ROUTINE(getTrapDisarmable)
    REO_DECL_ROUTINE(getTrapFlagged)
    REO_DECL_ROUTINE(getTrapKeyTag)
    REO_DECL_ROUTINE(getTrapOneShot)
    REO_DECL_ROUTINE(getTypeFromTalent)
    REO_DECL_ROUTINE(getUserActionsPending)
    REO_DECL_ROUTINE(getUserDefinedEventNumber)
    REO_DECL_ROUTINE(getWasForcePowerSuccessful)
    REO_DECL_ROUTINE(getWaypointByTag)
    REO_DECL_ROUTINE(getWeaponRanged)
    REO_DECL_ROUTINE(getWillSavingThrow)
    REO_DECL_ROUTINE(getXP)
    REO_DECL_ROUTINE(giveGoldToCreature)
    REO_DECL_ROUTINE(giveItem)
    REO_DECL_ROUTINE(givePlotXP)
    REO_DECL_ROUTINE(giveXPToCreature)
    REO_DECL_ROUTINE(grantFeat)
    REO_DECL_ROUTINE(grantSpell)
    REO_DECL_ROUTINE(hasLineOfSight)
    REO_DECL_ROUTINE(holdWorldFadeInForDialog)
    REO_DECL_ROUTINE(hoursToSeconds)
    REO_DECL_ROUTINE(incrementGlobalNumber)
    REO_DECL_ROUTINE(insertString)
    REO_DECL_ROUTINE(intToFloat)
    REO_DECL_ROUTINE(intToHexString)
    REO_DECL_ROUTINE(intToString)
    REO_DECL_ROUTINE(isAvailableCreature)
    REO_DECL_ROUTINE(isCreditSequenceInProgress)
    REO_DECL_ROUTINE(isFormActive)
    REO_DECL_ROUTINE(isInTotalDefense)
    REO_DECL_ROUTINE(isMeditating)
    REO_DECL_ROUTINE(isMoviePlaying)
    REO_DECL_ROUTINE(isNPCPartyMember)
    REO_DECL_ROUTINE(isObjectPartyMember)
    REO_DECL_ROUTINE(isRunning)
    REO_DECL_ROUTINE(isStealthed)
    REO_DECL_ROUTINE(jumpToLocation)
    REO_DECL_ROUTINE(jumpToObject)
    REO_DECL_ROUTINE(location)
    REO_DECL_ROUTINE(log)
    REO_DECL_ROUTINE(magicalEffect)
    REO_DECL_ROUTINE(modifyFortitudeSavingThrowBase)
    REO_DECL_ROUTINE(modifyInfluence)
    REO_DECL_ROUTINE(modifyReflexSavingThrowBase)
    REO_DECL_ROUTINE(modifyWillSavingThrowBase)
    REO_DECL_ROUTINE(musicBackgroundChangeDay)
    REO_DECL_ROUTINE(musicBackgroundChangeNight)
    REO_DECL_ROUTINE(musicBackgroundGetBattleTrack)
    REO_DECL_ROUTINE(musicBackgroundGetDayTrack)
    REO_DECL_ROUTINE(musicBackgroundGetNightTrack)
    REO_DECL_ROUTINE(musicBackgroundPlay)
    REO_DECL_ROUTINE(musicBackgroundSetDelay)
    REO_DECL_ROUTINE(musicBackgroundStop)
    REO_DECL_ROUTINE(musicBattleChange)
    REO_DECL_ROUTINE(musicBattlePlay)
    REO_DECL_ROUTINE(musicBattleStop)
    REO_DECL_ROUTINE(noClicksFor)
    REO_DECL_ROUTINE(objectToString)
    REO_DECL_ROUTINE(openStore)
    REO_DECL_ROUTINE(pauseGame)
    REO_DECL_ROUTINE(playAnimation)
    REO_DECL_ROUTINE(playMovie)
    REO_DECL_ROUTINE(playMovieQueue)
    REO_DECL_ROUTINE(playOverlayAnimation)
    REO_DECL_ROUTINE(playPazaak)
    REO_DECL_ROUTINE(playRoomAnimation)
    REO_DECL_ROUTINE(playRumblePattern)
    REO_DECL_ROUTINE(playSound)
    REO_DECL_ROUTINE(playVisualAreaEffect)
    REO_DECL_ROUTINE(popUpDeathGUIPanel)
    REO_DECL_ROUTINE(popUpGUIPanel)
    REO_DECL_ROUTINE(pow)
    REO_DECL_ROUTINE(printFloat)
    REO_DECL_ROUTINE(printInteger)
    REO_DECL_ROUTINE(printObject)
    REO_DECL_ROUTINE(printString)
    REO_DECL_ROUTINE(printVector)
    REO_DECL_ROUTINE(queueMovie)
    REO_DECL_ROUTINE(random)
    REO_DECL_ROUTINE(randomName)
    REO_DECL_ROUTINE(rebuildPartyTable)
    REO_DECL_ROUTINE(reflexSave)
    REO_DECL_ROUTINE(removeAvailableNPC)
    REO_DECL_ROUTINE(removeEffect)
    REO_DECL_ROUTINE(removeEffectByExactMatch)
    REO_DECL_ROUTINE(removeEffectByID)
    REO_DECL_ROUTINE(removeFromParty)
    REO_DECL_ROUTINE(removeHeartbeat)
    REO_DECL_ROUTINE(removeJournalQuestEntry)
    REO_DECL_ROUTINE(removeNPCFromPartyToBase)
    REO_DECL_ROUTINE(removePartyMember)
    REO_DECL_ROUTINE(resetCreatureAILevel)
    REO_DECL_ROUTINE(resetDialogState)
    REO_DECL_ROUTINE(resistForce)
    REO_DECL_ROUTINE(revealMap)
    REO_DECL_ROUTINE(roundsToSeconds)
    REO_DECL_ROUTINE(saveNPCByObject)
    REO_DECL_ROUTINE(saveNPCState)
    REO_DECL_ROUTINE(savePUPByObject)
    REO_DECL_ROUTINE(sendMessageToPC)
    REO_DECL_ROUTINE(setAreaFogColor)
    REO_DECL_ROUTINE(setAreaTransitionBMP)
    REO_DECL_ROUTINE(setAreaUnescapable)
    REO_DECL_ROUTINE(setAssociateListenPatterns)
    REO_DECL_ROUTINE(setAvailableNPCId)
    REO_DECL_ROUTINE(setBonusForcePoints)
    REO_DECL_ROUTINE(setButtonMashCheck)
    REO_DECL_ROUTINE(setCameraFacing)
    REO_DECL_ROUTINE(setCameraMode)
    REO_DECL_ROUTINE(setCommandable)
    REO_DECL_ROUTINE(setCreatureAILevel)
    REO_DECL_ROUTINE(setCurrentForm)
    REO_DECL_ROUTINE(setCurrentStealthXP)
    REO_DECL_ROUTINE(setCustomToken)
    REO_DECL_ROUTINE(setDialogPlaceableCamera)
    REO_DECL_ROUTINE(setDisableTransit)
    REO_DECL_ROUTINE(setEffectIcon)
    REO_DECL_ROUTINE(setEncounterActive)
    REO_DECL_ROUTINE(setEncounterDifficulty)
    REO_DECL_ROUTINE(setEncounterSpawnsCurrent)
    REO_DECL_ROUTINE(setEncounterSpawnsMax)
    REO_DECL_ROUTINE(setFacing)
    REO_DECL_ROUTINE(setFacingPoint)
    REO_DECL_ROUTINE(setFadeUntilScript)
    REO_DECL_ROUTINE(setFakeCombatState)
    REO_DECL_ROUTINE(setForceAlwaysUpdate)
    REO_DECL_ROUTINE(setForcePowerUnsuccessful)
    REO_DECL_ROUTINE(setForfeitConditions)
    REO_DECL_ROUTINE(setFormation)
    REO_DECL_ROUTINE(setGlobalBoolean)
    REO_DECL_ROUTINE(setGlobalFadeIn)
    REO_DECL_ROUTINE(setGlobalFadeOut)
    REO_DECL_ROUTINE(setGlobalLocation)
    REO_DECL_ROUTINE(setGlobalNumber)
    REO_DECL_ROUTINE(setGlobalString)
    REO_DECL_ROUTINE(setGoodEvilValue)
    REO_DECL_ROUTINE(setHealTarget)
    REO_DECL_ROUTINE(setIdentified)
    REO_DECL_ROUTINE(setInfluence)
    REO_DECL_ROUTINE(setInputClass)
    REO_DECL_ROUTINE(setIsDestroyable)
    REO_DECL_ROUTINE(setListening)
    REO_DECL_ROUTINE(setItemNonEquippable)
    REO_DECL_ROUTINE(setItemStackSize)
    REO_DECL_ROUTINE(setJournalQuestEntryPicture)
    REO_DECL_ROUTINE(setKeepStealthInDialog)
    REO_DECL_ROUTINE(setLightsaberPowered)
    REO_DECL_ROUTINE(setListenPattern)
    REO_DECL_ROUTINE(setLocalBoolean)
    REO_DECL_ROUTINE(setLocalNumber)
    REO_DECL_ROUTINE(setLockHeadFollowInDialog)
    REO_DECL_ROUTINE(setLockOrientationInDialog)
    REO_DECL_ROUTINE(setLocked)
    REO_DECL_ROUTINE(setMapPinEnabled)
    REO_DECL_ROUTINE(setMaxHitPoints)
    REO_DECL_ROUTINE(setMaxStealthXP)
    REO_DECL_ROUTINE(setMinOneHP)
    REO_DECL_ROUTINE(setMusicVolume)
    REO_DECL_ROUTINE(setNPCAIStyle)
    REO_DECL_ROUTINE(setNPCSelectability)
    REO_DECL_ROUTINE(setOrientOnClick)
    REO_DECL_ROUTINE(setPartyAIStyle)
    REO_DECL_ROUTINE(setPartyLeader)
    REO_DECL_ROUTINE(setPlaceableIllumination)
    REO_DECL_ROUTINE(setPlanetAvailable)
    REO_DECL_ROUTINE(setPlanetSelectable)
    REO_DECL_ROUTINE(setPlayerRestrictMode)
    REO_DECL_ROUTINE(setPlotFlag)
    REO_DECL_ROUTINE(setReturnStrref)
    REO_DECL_ROUTINE(setSoloMode)
    REO_DECL_ROUTINE(setStealthXPDecrement)
    REO_DECL_ROUTINE(setStealthXPEnabled)
    REO_DECL_ROUTINE(setTime)
    REO_DECL_ROUTINE(setTrapDetectedBy)
    REO_DECL_ROUTINE(setTrapDisabled)
    REO_DECL_ROUTINE(setTutorialWindowsEnabled)
    REO_DECL_ROUTINE(setXP)
    REO_DECL_ROUTINE(shipBuild)
    REO_DECL_ROUTINE(showChemicalUpgradeScreen)
    REO_DECL_ROUTINE(showDemoScreen)
    REO_DECL_ROUTINE(showGalaxyMap)
    REO_DECL_ROUTINE(showLevelUpGUI)
    REO_DECL_ROUTINE(showPartySelectionGUI)
    REO_DECL_ROUTINE(showSwoopUpgradeScreen)
    REO_DECL_ROUTINE(showTutorialWindow)
    REO_DECL_ROUTINE(showUpgradeScreen)
    REO_DECL_ROUTINE(signalEvent)
    REO_DECL_ROUTINE(sin)
    REO_DECL_ROUTINE(soundObjectFadeAndStop)
    REO_DECL_ROUTINE(soundObjectGetFixedVariance)
    REO_DECL_ROUTINE(soundObjectGetPitchVariance)
    REO_DECL_ROUTINE(soundObjectGetVolume)
    REO_DECL_ROUTINE(soundObjectPlay)
    REO_DECL_ROUTINE(soundObjectSetFixedVariance)
    REO_DECL_ROUTINE(soundObjectSetPitchVariance)
    REO_DECL_ROUTINE(soundObjectSetPosition)
    REO_DECL_ROUTINE(soundObjectSetVolume)
    REO_DECL_ROUTINE(soundObjectStop)
    REO_DECL_ROUTINE(spawnAvailableNPC)
    REO_DECL_ROUTINE(spawnAvailablePUP)
    REO_DECL_ROUTINE(spawnMine)
    REO_DECL_ROUTINE(speakOneLinerConversation)
    REO_DECL_ROUTINE(speakString)
    REO_DECL_ROUTINE(sqrt)
    REO_DECL_ROUTINE(startCreditSequence)
    REO_DECL_ROUTINE(startNewModule)
    REO_DECL_ROUTINE(stopRumblePattern)
    REO_DECL_ROUTINE(stringToFloat)
    REO_DECL_ROUTINE(stringToInt)
    REO_DECL_ROUTINE(supernaturalEffect)
    REO_DECL_ROUTINE(suppressStatusSummaryEntry)
    REO_DECL_ROUTINE(surrenderByFaction)
    REO_DECL_ROUTINE(surrenderRetainBuffs)
    REO_DECL_ROUTINE(surrenderToEnemies)
    REO_DECL_ROUTINE(switchPlayerCharacter)
    REO_DECL_ROUTINE(takeGoldFromCreature)
    REO_DECL_ROUTINE(talentFeat)
    REO_DECL_ROUTINE(talentSkill)
    REO_DECL_ROUTINE(talentSpell)
    REO_DECL_ROUTINE(tan)
    REO_DECL_ROUTINE(testStringAgainstPattern)
    REO_DECL_ROUTINE(touchAttackMelee)
    REO_DECL_ROUTINE(touchAttackRanged)
    REO_DECL_ROUTINE(turnsToSeconds)
    REO_DECL_ROUTINE(unlockAllSongs)
    REO_DECL_ROUTINE(vectorCreate)
    REO_DECL_ROUTINE(vectorMagnitude)
    REO_DECL_ROUTINE(vectorNormalize)
    REO_DECL_ROUTINE(vectorToAngle)
    REO_DECL_ROUTINE(versusAlignmentEffect)
    REO_DECL_ROUTINE(versusRacialTypeEffect)
    REO_DECL_ROUTINE(versusTrapEffect)
    REO_DECL_ROUTINE(willSave)
    REO_DECL_ROUTINE(writeTimestampedLogEntry)
    REO_DECL_ROUTINE(yardsToMeters)
    REO_DECL_ROUTINE(yavinHackCloseDoor)

    // END Routine implementations
};

} // namespace game

} // namespace reone
