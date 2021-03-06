/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2009-2010 TrinityZero <http://www.trinityzero.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "SharedDefines.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Unit.h"
#include "CreatureAI.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "SpellAuras.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "Pet.h"
#include "GameObject.h"
#include "GossipDef.h"
#include "Creature.h"
#include "Totem.h"
#include "CreatureAI.h"
#include "BattleGround.h"

#include "BattleGroundWS.h"
#include "OutdoorPvPMgr.h"
#include "VMapFactory.h"
#include "Language.h"
#include "SocialMgr.h"
#include "Util.h"
#include "TemporarySummon.h"
#include "ScriptCalls.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"

pEffect SpellEffects[TOTAL_SPELL_EFFECTS]=
{
    &Spell::EffectNULL,                                     //  0
    &Spell::EffectInstaKill,                                //  1 SPELL_EFFECT_INSTAKILL
    &Spell::EffectSchoolDMG,                                //  2 SPELL_EFFECT_SCHOOL_DAMAGE
    &Spell::EffectDummy,                                    //  3 SPELL_EFFECT_DUMMY
    &Spell::EffectUnused,                                   //  4 SPELL_EFFECT_PORTAL_TELEPORT          unused
    &Spell::EffectTeleportUnits,                            //  5 SPELL_EFFECT_TELEPORT_UNITS
    &Spell::EffectApplyAura,                                //  6 SPELL_EFFECT_APPLY_AURA
    &Spell::EffectEnvironmentalDMG,                         //  7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    &Spell::EffectPowerDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
    &Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
    &Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
    &Spell::EffectUnused,                                   // 11 SPELL_EFFECT_BIND
    &Spell::EffectNULL,                                     // 12 SPELL_EFFECT_PORTAL
    &Spell::EffectUnused,                                   // 13 SPELL_EFFECT_RITUAL_BASE              unused
    &Spell::EffectUnused,                                   // 14 SPELL_EFFECT_RITUAL_SPECIALIZE        unused
    &Spell::EffectUnused,                                   // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL   unused
    &Spell::EffectQuestComplete,                            // 16 SPELL_EFFECT_QUEST_COMPLETE
    &Spell::EffectWeaponDmg,                                // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    &Spell::EffectResurrect,                                // 18 SPELL_EFFECT_RESURRECT
    &Spell::EffectAddExtraAttacks,                          // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    &Spell::EffectUnused,                                   // 20 SPELL_EFFECT_DODGE                    one spell: Dodge
    &Spell::EffectUnused,                                   // 21 SPELL_EFFECT_EVADE                    one spell: Evade (DND)
    &Spell::EffectParry,                                    // 22 SPELL_EFFECT_PARRY
    &Spell::EffectBlock,                                    // 23 SPELL_EFFECT_BLOCK                    one spell: Block
    &Spell::EffectCreateItem,                               // 24 SPELL_EFFECT_CREATE_ITEM
    &Spell::EffectUnused,                                   // 25 SPELL_EFFECT_WEAPON
    &Spell::EffectUnused,                                   // 26 SPELL_EFFECT_DEFENSE                  one spell: Defense
    &Spell::EffectPersistentAA,                             // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    &Spell::EffectSummonType,                               // 28 SPELL_EFFECT_SUMMON
    &Spell::EffectMomentMove,                               // 29 SPELL_EFFECT_LEAP
    &Spell::EffectEnergize,                                 // 30 SPELL_EFFECT_ENERGIZE
    &Spell::EffectWeaponDmg,                                // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    &Spell::EffectTriggerMissileSpell,                      // 32 SPELL_EFFECT_TRIGGER_MISSILE
    &Spell::EffectOpenLock,                                 // 33 SPELL_EFFECT_OPEN_LOCK
    &Spell::EffectSummonChangeItem,                         // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    &Spell::EffectApplyAreaAura,                            // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    &Spell::EffectLearnSpell,                               // 36 SPELL_EFFECT_LEARN_SPELL
    &Spell::EffectUnused,                                   // 37 SPELL_EFFECT_SPELL_DEFENSE            one spell: SPELLDEFENSE (DND)
    &Spell::EffectDispel,                                   // 38 SPELL_EFFECT_DISPEL
    &Spell::EffectUnused,                                   // 39 SPELL_EFFECT_LANGUAGE
    &Spell::EffectDualWield,                                // 40 SPELL_EFFECT_DUAL_WIELD
    &Spell::EffectSummonWild,                               // 41 SPELL_EFFECT_SUMMON_WILD
    &Spell::EffectSummonGuardian,                           // 42 SPELL_EFFECT_SUMMON_GUARDIAN
    &Spell::EffectTeleUnitsFaceCaster,                      // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    &Spell::EffectLearnSkill,                               // 44 SPELL_EFFECT_SKILL_STEP
    &Spell::EffectAddHonor,                                 // 45 SPELL_EFFECT_ADD_HONOR                honor/pvp related
    &Spell::EffectNULL,                                     // 46 SPELL_EFFECT_SPAWN                    we must spawn pet there
    &Spell::EffectTradeSkill,                               // 47 SPELL_EFFECT_TRADE_SKILL
    &Spell::EffectUnused,                                   // 48 SPELL_EFFECT_STEALTH                  one spell: Base Stealth
    &Spell::EffectUnused,                                   // 49 SPELL_EFFECT_DETECT                   one spell: Detect
    &Spell::EffectTransmitted,                              // 50 SPELL_EFFECT_TRANS_DOOR
    &Spell::EffectUnused,                                   // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT       unused
    &Spell::EffectUnused,                                   // 52 SPELL_EFFECT_GUARANTEE_HIT            one spell: zzOLDCritical Shot
    &Spell::EffectEnchantItemPerm,                          // 53 SPELL_EFFECT_ENCHANT_ITEM
    &Spell::EffectEnchantItemTmp,                           // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    &Spell::EffectTameCreature,                             // 55 SPELL_EFFECT_TAMECREATURE
    &Spell::EffectSummonPet,                                // 56 SPELL_EFFECT_SUMMON_PET
    &Spell::EffectLearnPetSpell,                            // 57 SPELL_EFFECT_LEARN_PET_SPELL
    &Spell::EffectWeaponDmg,                                // 58 SPELL_EFFECT_WEAPON_DAMAGE
    &Spell::EffectOpenSecretSafe,                           // 59 SPELL_EFFECT_OPEN_LOCK_ITEM
    &Spell::EffectProficiency,                              // 60 SPELL_EFFECT_PROFICIENCY
    &Spell::EffectSendEvent,                                // 61 SPELL_EFFECT_SEND_EVENT
    &Spell::EffectPowerBurn,                                // 62 SPELL_EFFECT_POWER_BURN
    &Spell::EffectThreat,                                   // 63 SPELL_EFFECT_THREAT
    &Spell::EffectTriggerSpell,                             // 64 SPELL_EFFECT_TRIGGER_SPELL
    &Spell::EffectUnused,                                   // 65 SPELL_EFFECT_HEALTH_FUNNEL            unused
    &Spell::EffectUnused,                                   // 66 SPELL_EFFECT_POWER_FUNNEL             unused
    &Spell::EffectHealMaxHealth,                            // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    &Spell::EffectInterruptCast,                            // 68 SPELL_EFFECT_INTERRUPT_CAST
    &Spell::EffectDistract,                                 // 69 SPELL_EFFECT_DISTRACT
    &Spell::EffectPull,                                     // 70 SPELL_EFFECT_PULL                     one spell: Distract Move
    &Spell::EffectPickPocket,                               // 71 SPELL_EFFECT_PICKPOCKET
    &Spell::EffectAddFarsight,                              // 72 SPELL_EFFECT_ADD_FARSIGHT
    &Spell::EffectSummonPossessed,                          // 73 SPELL_EFFECT_SUMMON_POSSESSED
    &Spell::EffectSummonTotem,                              // 74 SPELL_EFFECT_SUMMON_TOTEM
    &Spell::EffectHealMechanical,                           // 75 SPELL_EFFECT_HEAL_MECHANICAL          one spell: Mechanical Patch Kit
    &Spell::EffectSummonObjectWild,                         // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    &Spell::EffectScriptEffect,                             // 77 SPELL_EFFECT_SCRIPT_EFFECT
    &Spell::EffectUnused,                                   // 78 SPELL_EFFECT_ATTACK
    &Spell::EffectSanctuary,                                // 79 SPELL_EFFECT_SANCTUARY
    &Spell::EffectAddComboPoints,                           // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    &Spell::EffectUnused,                                   // 81 SPELL_EFFECT_CREATE_HOUSE             one spell: Create House (TEST)
    &Spell::EffectNULL,                                     // 82 SPELL_EFFECT_BIND_SIGHT
    &Spell::EffectDuel,                                     // 83 SPELL_EFFECT_DUEL
    &Spell::EffectStuck,                                    // 84 SPELL_EFFECT_STUCK
    &Spell::EffectSummonPlayer,                             // 85 SPELL_EFFECT_SUMMON_PLAYER
    &Spell::EffectActivateObject,                           // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    &Spell::EffectSummonTotem,                              // 87 SPELL_EFFECT_SUMMON_TOTEM_SLOT1
    &Spell::EffectSummonTotem,                              // 88 SPELL_EFFECT_SUMMON_TOTEM_SLOT2
    &Spell::EffectSummonTotem,                              // 89 SPELL_EFFECT_SUMMON_TOTEM_SLOT3
    &Spell::EffectSummonTotem,                              // 90 SPELL_EFFECT_SUMMON_TOTEM_SLOT4
    &Spell::EffectUnused,                                   // 91 SPELL_EFFECT_THREAT_ALL               one spell: zzOLDBrainwash
    &Spell::EffectEnchantHeldItem,                          // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    &Spell::EffectUnused,                                   // 93 SPELL_EFFECT_SUMMON_PHANTASM
    &Spell::EffectSelfResurrect,                            // 94 SPELL_EFFECT_SELF_RESURRECT
    &Spell::EffectSkinning,                                 // 95 SPELL_EFFECT_SKINNING
    &Spell::EffectUnused,                                   // 96 SPELL_EFFECT_CHARGE
    &Spell::EffectSummonCritter,                            // 97 SPELL_EFFECT_SUMMON_CRITTER
    &Spell::EffectKnockBack,                                // 98 SPELL_EFFECT_KNOCK_BACK
    &Spell::EffectDisEnchant,                               // 99 SPELL_EFFECT_DISENCHANT
    &Spell::EffectInebriate,                                //100 SPELL_EFFECT_INEBRIATE
    &Spell::EffectFeedPet,                                  //101 SPELL_EFFECT_FEED_PET
    &Spell::EffectDismissPet,                               //102 SPELL_EFFECT_DISMISS_PET
    &Spell::EffectReputation,                               //103 SPELL_EFFECT_REPUTATION
    &Spell::EffectSummonObject,                             //104 SPELL_EFFECT_SUMMON_OBJECT_SLOT1
    &Spell::EffectSummonObject,                             //105 SPELL_EFFECT_SUMMON_OBJECT_SLOT2
    &Spell::EffectSummonObject,                             //106 SPELL_EFFECT_SUMMON_OBJECT_SLOT3
    &Spell::EffectSummonObject,                             //107 SPELL_EFFECT_SUMMON_OBJECT_SLOT4
    &Spell::EffectDispelMechanic,                           //108 SPELL_EFFECT_DISPEL_MECHANIC
    &Spell::EffectSummonDeadPet,                            //109 SPELL_EFFECT_SUMMON_DEAD_PET
    &Spell::EffectDestroyAllTotems,                         //110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    &Spell::EffectDurabilityDamage,                         //111 SPELL_EFFECT_DURABILITY_DAMAGE
    &Spell::EffectSummonDemon,                              //112 SPELL_EFFECT_SUMMON_DEMON
    &Spell::EffectResurrectNew,                             //113 SPELL_EFFECT_RESURRECT_NEW
    &Spell::EffectTaunt,                                    //114 SPELL_EFFECT_ATTACK_ME
    &Spell::EffectDurabilityDamagePCT,                      //115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    &Spell::EffectSkinPlayerCorpse,                         //116 SPELL_EFFECT_SKIN_PLAYER_CORPSE       one spell: Remove Insignia, bg usage, required special corpse flags...
    &Spell::EffectSpiritHeal,                               //117 SPELL_EFFECT_SPIRIT_HEAL              one spell: Spirit Heal
    &Spell::EffectSkill,                                    //118 SPELL_EFFECT_SKILL                    professions and more
    &Spell::EffectApplyAreaAura,                            //119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    &Spell::EffectUnused,                                   //120 SPELL_EFFECT_TELEPORT_GRAVEYARD       one spell: Graveyard Teleport Test
    &Spell::EffectWeaponDmg,                                //121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    &Spell::EffectUnused,                                   //122 SPELL_EFFECT_122                      unused
    &Spell::EffectSendTaxi,                                 //123 SPELL_EFFECT_SEND_TAXI                taxi/flight related (misc value is taxi path id)
    &Spell::EffectPlayerPull,                               //124 SPELL_EFFECT_PLAYER_PULL              opposite of knockback effect (pulls player twoard caster)
    &Spell::EffectModifyThreatPercent,                      //125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    &Spell::EffectStealBeneficialBuff,                      //126 SPELL_EFFECT_STEAL_BENEFICIAL_BUFF    spell steal effect?
    &Spell::EffectNULL,                                     //127 SPELL_EFFECT_PROSPECTING              Prospecting spell
    &Spell::EffectApplyAreaAura,                            //128 SPELL_EFFECT_APPLY_AREA_AURA_FRIEND
    &Spell::EffectApplyAreaAura,                            //129 SPELL_EFFECT_APPLY_AREA_AURA_ENEMY
    &Spell::EffectRedirectThreat,                           //130 SPELL_EFFECT_REDIRECT_THREAT
    &Spell::EffectUnused,                                   //131 SPELL_EFFECT_131                      used in some test spells
    &Spell::EffectNULL,                                     //132 SPELL_EFFECT_PLAY_MUSIC               sound id in misc value
    &Spell::EffectUnlearnSpecialization,                    //133 SPELL_EFFECT_UNLEARN_SPECIALIZATION   unlearn profession specialization
    &Spell::EffectKillCredit,                               //134 SPELL_EFFECT_KILL_CREDIT              misc value is creature entry
    &Spell::EffectNULL,                                     //135 SPELL_EFFECT_CALL_PET
    &Spell::EffectHealPct,                                  //136 SPELL_EFFECT_HEAL_PCT
    &Spell::EffectEnergisePct,                              //137 SPELL_EFFECT_ENERGIZE_PCT
    &Spell::EffectNULL,                                     //138 SPELL_EFFECT_138                      Leap
    &Spell::EffectUnused,                                   //139 SPELL_EFFECT_139                      unused
    &Spell::EffectForceCast,                                //140 SPELL_EFFECT_FORCE_CAST
    &Spell::EffectNULL,                                     //141 SPELL_EFFECT_141                      damage and reduce speed?
    &Spell::EffectTriggerSpellWithValue,                    //142 SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE
    &Spell::EffectApplyAreaAura,                            //143 SPELL_EFFECT_APPLY_AREA_AURA_OWNER
    &Spell::EffectKnockBack,                                //144 SPELL_EFFECT_KNOCK_BACK_2             Spectral Blast
    &Spell::EffectPlayerPull,                               //145 SPELL_EFFECT_145                      Black Hole Effect
    &Spell::EffectUnused,                                   //146 SPELL_EFFECT_146                      unused
    &Spell::EffectQuestFail,                                //147 SPELL_EFFECT_QUEST_FAIL               quest fail
    &Spell::EffectUnused,                                   //148 SPELL_EFFECT_148                      unused
    &Spell::EffectNULL,                                     //149 SPELL_EFFECT_149                      swoop
    &Spell::EffectUnused,                                   //150 SPELL_EFFECT_150                      unused
    &Spell::EffectTriggerRitualOfSummoning,                 //151 SPELL_EFFECT_TRIGGER_SPELL_2
    &Spell::EffectNULL,                                     //152 SPELL_EFFECT_152                      summon Refer-a-Friend
    &Spell::EffectNULL,                                     //153 SPELL_EFFECT_CREATE_PET               misc value is creature entry
};

void Spell::EffectNULL(uint32 /*i*/)
{
    sLog.outDebug("WORLD: Spell Effect DUMMY");
}

void Spell::EffectUnused(uint32 /*i*/)
{
    // NOT USED BY ANY SPELL OR USELESS OR IMPLEMENTED IN DIFFERENT WAY IN MANGOS
}

void Spell::EffectResurrectNew(uint32 i)
{
    if(!unitTarget || unitTarget->isAlive())
        return;

    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(!unitTarget->IsInWorld())
        return;

    Player* pTarget = ((Player*)unitTarget);

    if(pTarget->isRessurectRequested())       // already have one active request
        return;

    uint32 health = damage;
    uint32 mana = m_spellInfo->EffectMiscValue[i];
    pTarget->setResurrectRequestData(m_caster->GetGUID(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana);
    SendResurrectRequest(pTarget);
}

void Spell::EffectInstaKill(uint32 /*i*/)
{
    if( !unitTarget || !unitTarget->isAlive() )
        return;

    // Demonic Sacrifice
    if(m_spellInfo->Id==18788 && unitTarget->GetTypeId()==TYPEID_UNIT)
    {
        uint32 entry = unitTarget->GetEntry();
        uint32 spellID;
        switch(entry)
        {
            case   416: spellID=18789; break;               //imp
            case   417: spellID=18792; break;               //fellhunter
            case  1860: spellID=18790; break;               //void
            case  1863: spellID=18791; break;               //succubus
            default:
                sLog.outError("EffectInstaKill: Unhandled creature entry (%u) case.",entry);
                return;
        }

        m_caster->CastSpell(m_caster,spellID,true);
    }

    if(m_caster==unitTarget)                                // prevent interrupt message
        finish();

    WorldPacket data(SMSG_SPELLINSTAKILLLOG, (8 + 4));
    data << unitTarget->GetGUID();                    // Victim GUID
    data << uint32(m_spellInfo->Id);
    m_caster->SendMessageToSet(&data, true);

    uint32 health = unitTarget->GetHealth();
    m_caster->DealDamage(unitTarget, health, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
}

void Spell::EffectEnvironmentalDMG(uint32 i)
{
    uint32 absorb = 0;
    uint32 resist = 0;

    // Note: this hack with damage replace required until GO casting not implemented
    // environment damage spells already have around enemies targeting but this not help in case not existed GO casting support
    // currently each enemy selected explicitly and self cast damage, we prevent apply self casted spell bonuses/etc
    damage = m_spellInfo->EffectBasePoints[i]+m_spellInfo->EffectBaseDice[i];

    m_caster->CalcAbsorbResist(unitTarget,GetSpellSchoolMask(m_spellInfo), SPELL_DIRECT_DAMAGE, damage, &absorb, &resist);

    m_caster->SendSpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, damage, GetSpellSchoolMask(m_spellInfo), absorb, resist, false, 0, false);
    if(unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->EnvironmentalDamage(unitTarget->GetGUID(),DAMAGE_FIRE,damage);
}

void Spell::EffectSchoolDMG(uint32 effect_idx)
{
}

void Spell::SpellDamageSchoolDmg(uint32 effect_idx)
{
    if (unitTarget && unitTarget->isAlive())
    {
        switch (m_spellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                // Meteor like spells (divided damage to targets)
                if(m_customAttr & SPELL_ATTR_CU_SHARE_DAMAGE)
                {
                    uint32 count = 0;
                    for(std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin();ihit != m_UniqueTargetInfo.end();++ihit)
                        if(ihit->effectMask & (1<<effect_idx))
                            ++count;

                    damage /= count;                    // divide to all targets
                }
                switch (m_spellInfo->Id)                    // better way to check unknown
                {
                    // percent from health with min
                    case 25599:                             // Thundercrash
                    {
                        damage = unitTarget->GetHealth() / 2;
                        if (damage < 200)
                            { damage = 200; }
                        break;
                    }
                    // Judgement of Command
                    case 20467:    case 20963:    case 20964:    case 20965:    case 20966:
                    {
                        if(!unitTarget->hasUnitState(UNIT_STAT_STUNNED) && m_caster->GetTypeId() == TYPEID_PLAYER)
                            { damage /= 2; }
                        break;
                    }
                    // Chain Lightning (Simone the Seductress)
                    case 23206:
                    {
                        if (unitTarget->HasAura(20190,0))     // reduce damage by 75% if target has Aspect of the Wild (Rank 2)
                            damage *= 0.25;
                         break;
                    }
                    // T2 Set Bonus Judgement spell that shouldn't benefit from Spell Damage
                    case 23590:
                    {
                        if (damage >= 0)
                            { m_damage += damage; }

                        return;
                        break;
                    }
                }
                break;
            }
            case SPELLFAMILY_MAGE:
                break;
            case SPELLFAMILY_WARRIOR:
            {
                // Bloodthirst
                if (m_spellInfo->SpellIconID == 38 && m_spellInfo->SpellFamilyFlags & 0x2000000LL)
                {
                    damage = uint32(damage * (m_caster->GetTotalAttackPowerValue(BASE_ATTACK)) / 100);
                }
                // Shield Slam
                else if (m_spellInfo->SpellFamilyFlags & 0x100000000LL)
                    { damage += int32(m_caster->GetShieldBlockValue()); }
                break;
            }
            case SPELLFAMILY_WARLOCK:
            {
                // Conflagrate - consumes Immolate
                if (m_spellInfo->SpellFamilyFlags & 0x0000000000000200)
                {
                    // for caster applied auras only
                    Unit::AuraList const& mPeriodic = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraList::const_iterator i = mPeriodic.begin(); i != mPeriodic.end(); ++i)
                    {
                        if ((*i)->GetCasterGUID() == m_caster->GetGUID() &&
                            // Immolate
                            (*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_WARLOCK && ((*i)->GetSpellProto()->SpellFamilyFlags & 0x0000000000000004))
                        {
                            unitTarget->RemoveAurasByCasterSpell((*i)->GetId(), m_caster->GetGUID());
                            break;
                        }
                    }
                }
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                // Ferocious Bite
                if ((m_spellInfo->SpellFamilyFlags & 0x000800000) && m_spellInfo->SpellVisual == 6587)
                {
                    // converts each extra point of energy into ($f1+$AP/630) additional damage
                    float multiple = m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 630 + m_spellInfo->DmgMultiplier[effect_idx];
                    damage += int32(m_caster->GetPower(POWER_ENERGY) * multiple);
                    m_caster->SetPower(POWER_ENERGY, 0);
                }
                // Rake
                else if(m_spellInfo->SpellFamilyFlags & 0x0000000000001000LL)
                {
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
                }
                // Swipe
                else if(m_spellInfo->SpellFamilyFlags & 0x0010000000000000LL)
                {
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.08f);
                }
                break;
            }
            case SPELLFAMILY_ROGUE:
            {
                // Eviscerate
                if ((m_spellInfo->SpellFamilyFlags & 0x00020000LL) && m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    if (uint32 combo = ((Player*)m_caster)->GetComboPoints())
                    {
                        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * combo * 0.03f);
                    }
                }
                break;
            }
            case SPELLFAMILY_HUNTER:
                // Mongoose Bite
                if((m_spellInfo->SpellFamilyFlags & 0x000000002) && m_spellInfo->SpellVisual==342)
                {
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.2);
                }
                // Arcane Shot
                else if((m_spellInfo->Id == 3044) || (m_spellInfo->Id > 14280 && m_spellInfo->Id < 14288))
                {
                    damage += int32(m_caster->GetTotalAttackPowerValue(RANGED_ATTACK)*0.15);
                }
                // Explosive Trap Effect
                else if(m_spellInfo->SpellFamilyFlags & 0x00000004)
                {
                    damage += int32(m_caster->GetTotalAttackPowerValue(RANGED_ATTACK)*0.1);
                }
                break;
            case SPELLFAMILY_PALADIN:
                break;
        }

        if(m_originalCaster && damage > 0)
            damage = m_originalCaster->SpellDamageBonus(unitTarget, m_spellInfo, (uint32)damage, SPELL_DIRECT_DAMAGE);

        if (damage >= 0)
            { m_damage += damage; }
    }
}

void Spell::EffectDummy(uint32 i)
{
    if(!unitTarget && !gameObjTarget && !itemTarget)
        return;

    uint32 spell_id = 0;
    int32 bp = 0;

    // selection by spell family
    switch(m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id )
            {
                case 3360:  // Curse of the Eye
                {
                    if (!unitTarget)
                        return;

                    uint32 spell_id = (unitTarget->getGender() == GENDER_MALE) ? 10651 : 10653;

                    m_caster->CastSpell(unitTarget, spell_id, true);
                    return;
                }
                case 4050:  // Explosive Sheep
                {
                    Unit* pOwner = m_caster->GetCharmerOrOwner();
                    if (pOwner && pOwner->GetTypeId() == TYPEID_PLAYER)
                    {
                        uint32 spellid = m_caster->GetUInt32Value(UNIT_CREATED_BY_SPELL);
                        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellid);
                        if (spellInfo)
                            ((Player*)pOwner)->SendCooldownEvent(spellInfo);
                    }
                    return;
                }
                case 7671:  // Transformation (human<->worgen)
                {
                    if (!unitTarget)
                        return;

                    // Transform Visual
                    unitTarget->CastSpell(unitTarget, 24085, true);
                    return;
                }
                case 8063:  // Deviate Fish
                {
                    if(m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1,5))
                    {
                        case 1: spell_id = 8064; break;     // Sleepy
                        case 2: spell_id = 8065; break;     // Invigorate
                        case 3: spell_id = 8066; break;     // Shrink
                        case 4: spell_id = 8067; break;     // Party Time!
                        case 5: spell_id = 8068; break;     // Healthy Spirit
                    }
                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 8213:  // Savory Deviate Delight
                {
                    if(m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1,6))
                    {
                        // Flip Out - ninja
                        case 1: spell_id = (m_caster->getGender() == GENDER_MALE ? 8219 : 8220); break;
                        // Yaaarrrr - pirate
                        case 2: spell_id = (m_caster->getGender() == GENDER_MALE ? 8221 : 8222); break;
                        // Oops - goo
                        case 3: spell_id = 8223; break;
                        // Rapid Cast
                        case 4: spell_id = 8215; break;
                        // Cowardice
                        case 5: spell_id = 8224; break;
                        // Fake Death
                        case 6: spell_id = 8226; break;
                    }
                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 8344: // Universal Remote
                {
                    if (!m_originalCaster)
                        return;

                    // Random spell
                    uint32 randomSpell = urand(0, 2);
                    switch (randomSpell)
                    {
                        case 0:
                            m_originalCaster->CastSpell(unitTarget, 8345, true);
                            break;
                        case 1:
                            m_originalCaster->CastSpell(unitTarget, 8346, true);
                            break;
                        case 2:
                            unitTarget->CastSpell(unitTarget, 8599, true);
                            break;
                        default:
                            m_originalCaster->CastSpell(unitTarget, 8346, true);
                            break;
                    }
                    return;
                }
                case 8593:  // Symbol of life (restore creature to life)
                {
                    if(!unitTarget || unitTarget->GetTypeId()!=TYPEID_UNIT)
                        return;
                    ((Creature*)unitTarget)->setDeathState(JUST_ALIVED);
                    return;
                }
                case 8897:  // Destroy Rocket Boots
                {
                    m_caster->CastSpell(unitTarget, 8893, true);
                    m_caster->CastSpell(unitTarget, 13158, true);

                    return;
                }
                case 9976:  // Polly Eats the E.C.A.C.
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Summon Polly Jr.
                    unitTarget->CastSpell(unitTarget, 9998, true);

                    ((Creature*)unitTarget)->ForcedDespawn();
                    return;
                }
                case 11885: // Capture Treant
                case 11886: // Capture Wildkin
                case 11887: // Capture Hippogryph
                case 11888: // Capture Faerie Dragon
                case 11889: // Capture Mountain Giant
                case 12699: // Summon Screecher Spirit
                {
                    if (unitTarget->isDead() && unitTarget->GetTypeId() == TYPEID_UNIT)
                        ((Creature*)unitTarget)->ForcedDespawn();
                    return;
                }
                case 12162: // Deep wounds
                case 12850: // (now good common check for this spells)
                case 12868:
                {
                    if(!unitTarget)
                        return;

                    float damage;
                    // DW should benefit of attack power, damage percent mods etc.
                    // TODO: check if using offhand damage is correct and if it should be divided by 2
                    if (m_caster->haveOffhandWeapon() && m_caster->getAttackTimer(BASE_ATTACK) > m_caster->getAttackTimer(OFF_ATTACK))
                        damage = (m_caster->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + m_caster->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE))/2;
                    else
                        damage = (m_caster->GetFloatValue(UNIT_FIELD_MINDAMAGE) + m_caster->GetFloatValue(UNIT_FIELD_MAXDAMAGE))/2;

                    switch (m_spellInfo->Id)
                    {
                        case 12850: damage *= 0.2f; break;
                        case 12162: damage *= 0.4f; break;
                        case 12868: damage *= 0.6f; break;
                        default:
                            sLog.outError("Spell::EffectDummy: Spell %u not handled in DW",m_spellInfo->Id);
                            return;
                    };

                    int32 deepWoundsDotBasePoints0 = int32(damage / 4);
                    m_caster->CastCustomSpell(unitTarget, 12721, &deepWoundsDotBasePoints0, NULL, NULL, true, NULL);
                    return;
                }
                case 12975: //Last Stand
                {
                    int32 healthModSpellBasePoints0 = int32(m_caster->GetMaxHealth()*0.3);
                    m_caster->CastCustomSpell(m_caster, 12976, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
                    return;
                }
                case 13006: // Shrink ray
                {
                    if (!m_originalCaster)
                        return;
                    /* Source: http://www.wowwiki.com/Gnomish_Shrink_Ray
                        Not implemented yet:
                        Make every attacking enemy bigger
                        Make every attacking enemy smaller
                        Give shield-wielding enemies "Improved Blocking" for 30 seconds (this shows no visible effect other than target is hit by the ray)
                        13003 - Shrink Ray (single target)
                        13010 - Shrink (TARGET_ALL_PARTY_AROUND_CASTER)
                        13004 - Grow  (TARGET_ALL_PARTY_AROUND_CASTER)
                     */
                    uint32 r = urand(0, 99);
                    // Normal behavior
                    if (r > 15)
                        m_originalCaster->CastSpell(unitTarget, 13003, true);
                    else if (r > 13) // Make the user bigger
                    {
                        m_originalCaster->AddAura(13004,m_originalCaster);
                        m_originalCaster->SendSpellGo(m_originalCaster, 13004);
                    }
                    else if (r > 11) // Make the user smaller
                    {
                        m_originalCaster->AddAura(13010,m_originalCaster);
                        m_originalCaster->SendSpellGo(m_originalCaster, 13010);
                    }
                    else if (r > 9) // Make the target bigger
                    {
                        unitTarget->AddAura(13004,unitTarget);
                        m_originalCaster->SendSpellGo(unitTarget, 13004);
                    }
                    else if (r > 4) // Make the user's entire party smaller
                        m_originalCaster->CastSpell(unitTarget, 13010, true);
                    else // Make the user's entire party bigger
                        m_originalCaster->CastSpell(unitTarget, 13004, true);
                    break;
                }
                case 13120: // net-o-matic
                {
                    if(!unitTarget)
                        return;

                    uint32 spell_id = 0;

                    uint32 roll = urand(0, 99);

                    if(roll < 10)                            // 10% for 30 sec self root (off-like chance unknown)
                        spell_id = 16566;
                    else if(roll < 20)                       // 10% for 20 sec root, charge to target (off-like chance unknown)
                        spell_id = 13119;
                    else                                    // normal root
                        spell_id = 13099;

                    m_caster->CastSpell(unitTarget,spell_id,true,NULL);
                    return;
                }
                case 13180: // Gnomish Mind Control Cap
                {
                    if (!m_originalCaster)
                        return;

                    // [WowWiki] Possible malfunctions
                    // - Fails with no effect, but cooldown is trigged.
                    // - User becomes mind controlled by target. However, if the Gnomish Mind Control Cap is used while in a shapeshift form (such as cat form), this effect does not trigger.
                    switch (urand(0, 5))
                    {
                        case 0:
                            break;
                        case 1:
                            if (m_originalCaster->m_form == FORM_NONE)
                                unitTarget->CastSpell(m_originalCaster, 13181, true);
                            break;
                        default:
                            m_originalCaster->CastSpell(unitTarget, 13181, true);
                            break;
                    }
                    return;
                }
                case 13535: // Tame Beast
                {
                    if (!m_originalCaster || m_originalCaster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Creature* channelTarget = m_originalCaster->GetMap()->GetCreatureInMap(m_originalCaster->GetChannelObjectGuid());

                    if (!channelTarget)
                        return;

                    m_originalCaster->CastSpell(channelTarget, 13481, true, NULL, NULL, m_originalCasterGUID);
                    return;
                }
                case 13567: // Dummy Trigger
                {
                    // can be used for different aura triggering, so select by aura
                    if(!m_triggeredByAuraSpell || !unitTarget)
                        return;

                    switch(m_triggeredByAuraSpell->Id)
                    {
                        case 26467:                         // Persistent Shield
                            m_caster->CastCustomSpell(unitTarget, 26470, &damage, NULL, NULL, true);
                            break;
                        default:
                            sLog.outError("EffectDummy: Non-handled case for spell 13567 for triggered aura %u",m_triggeredByAuraSpell->Id);
                            break;
                    }
                    return;
                }
                case 14185: // Preparation Rogue
                {
                    if(m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    //immediately finishes the cooldown on certain Rogue abilities
                    const PlayerSpellMap& sp_list = ((Player *)m_caster)->GetSpellMap();
                    for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                    {
                        uint32 classspell = itr->first;
                        SpellEntry const *spellInfo = sSpellStore.LookupEntry(classspell);

                        // all spells with cooldown
                        if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE &&
                            spellInfo->Id != m_spellInfo->Id && GetSpellRecoveryTime(spellInfo) > 0)
                        {
                            ((Player*)m_caster)->RemoveSpellCooldown(classspell);

                            WorldPacket data(SMSG_CLEAR_COOLDOWN, (4+8));
                            data << uint32(classspell);
                            data << uint64(m_caster->GetGUID());
                            ((Player*)m_caster)->GetSession()->SendPacket(&data);
                        }
                    }
                    return;
                }
                case 14537: // Six Demon Bag
                {
                    if (!unitTarget)
                        return;

                    Unit* newTarget = unitTarget;
                    uint32 spell_id = 0;
                    uint32 roll = urand(0, 99);
                    if (roll < 25)                          // Fireball (25% chance)
                        spell_id = 15662;
                    else if (roll < 50)                     // Frostbolt (25% chance)
                        spell_id = 11538;
                    else if (roll < 70)                     // Chain Lighting (20% chance)
                        spell_id = 21179;
                    else if (roll < 77)                     // Polymorph (10% chance, 7% to target)
                        spell_id = 14621;
                    else if (roll < 80)                     // Polymorph (10% chance, 3% to self, backfire)
                    {
                        spell_id = 14621;
                        newTarget = m_caster;
                    }
                    else if (roll < 95)                     // Enveloping Winds (15% chance)
                        spell_id = 25189;
                    else                                    // Summon Felhund minion (5% chance)
                    {
                        spell_id = 14642;
                        newTarget = m_caster;
                    }

                    m_caster->CastSpell(newTarget, spell_id, true, m_CastItem);
                    return;
                }
                case 15998: // Capture Worg Pup
                {
                    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;

                    creatureTarget->ForcedDespawn();
                    return;
                }
                case 16589: // Noggenfogger Elixir
                {
                    if(m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1,3))
                    {
                        case 1: spell_id = 16595; break;
                        case 2: spell_id = 16593; break;
                        default:spell_id = 16591; break;
                    }

                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 17009: // Voodoo
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(0, 6))
                    {
                        case 0:
                            spell_id = 16707;
                            break;    // Hex
                        case 1:
                            spell_id = 16708;
                            break;    // Hex
                        case 2:
                            spell_id = 16709;
                            break;    // Hex
                        case 3:
                            spell_id = 16711;
                            break;    // Grow
                        case 4:
                            spell_id = 16712;
                            break;    // Special Brew
                        case 5:
                            spell_id = 16713;
                            break;    // Ghostly
                        case 6:
                            spell_id = 16716;
                            break;    // Launch
                    }

                    m_caster->CastSpell(unitTarget, spell_id, true, NULL, NULL, m_originalCasterGUID);
                    return;
                }
                case 17251: // Spirit Healer Res
                {
                    if(!unitTarget || !m_originalCaster)
                        return;

                    if(m_originalCaster->GetTypeId() == TYPEID_PLAYER)
                    {
                        WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
                        data << unitTarget->GetGUID();
                        ((Player*)m_originalCaster)->GetSession()->SendPacket( &data );
                    }
                    return;
                }
                case 17271: // Test Fetid Skull
                {
                    if(!itemTarget && m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    uint32 spell_id = roll_chance_i(50) ? 17269 : 17270;

                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 17770: // Wolfshead Helm Energy
                {
                    m_caster->CastSpell(m_caster, 29940, true, NULL);
                    return;
                }
                case 17950: // Shadow Portal
                {
                    if (!unitTarget)
                        return;

                    const uint32 spell_list[6] = {17863, 17939, 17943, 17944, 17946, 17948};

                    m_caster->CastSpell(unitTarget, spell_list[urand(0, 5)], true);
                    return;
                }
                case 18350: // Dummy Trigger
                {
                    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Need remove self if Lightning Shield not active
                    if (unitTarget->HasAura(324,0) || unitTarget->HasAura(325,0) || unitTarget->HasAura(905,0) || unitTarget->HasAura(945,0) || unitTarget->HasAura(8134,0) || unitTarget->HasAura(10431,0) || unitTarget->HasAura(10432,0))
                        return;

                    unitTarget->RemoveAurasDueToSpell(28820);
                    return;
                }
                case 19411: // Lava Bomb
                case 20474: // Lava Bomb
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 20494, true);
                    return;
                }
                case 19593: // Egg Explosion (Buru)
                {
                    if (!unitTarget)
                        return;

                    if (!m_originalCaster)
                        return;

                    float tempDamage = 0.0f;
                    float distance = m_originalCaster->GetDistance(unitTarget);
                    distance = 1.0 - (distance / 25.0f);

                    if (unitTarget->isAlive())
                    {
                        if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                        {
                            tempDamage = 500 * distance;
                            if (tempDamage < 100)
                                tempDamage = 100;
                        }
                        else
                        {
                            if (unitTarget->GetEntry() == 15370 && unitTarget->GetHealthPercent() > 20.0f)
                                tempDamage = 45000 * distance;
                            else
                                return;
                        }

                        m_originalCaster->DealDamage(unitTarget, tempDamage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                    return;
                }
                case 20572: // Blood Fury (racial)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        { return; }

                    m_caster->CastSpell(m_caster, 23230, true);

                    damage = uint32(damage * (m_caster->GetInt32Value(UNIT_FIELD_ATTACK_POWER)) / 100);
                    m_caster->CastCustomSpell(m_caster, 23234, &damage, NULL, NULL, true, NULL);

                    return;
                }
                case 20577: // Cannibalize
                    if (unitTarget)
                        m_caster->CastSpell(m_caster,20578,false,NULL);
                    return;
                case 23019: // Crystal Prison Dummy DND
                {
                    if(!unitTarget || !unitTarget->isAlive() || unitTarget->GetTypeId() != TYPEID_UNIT || ((Creature*)unitTarget)->isPet())
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;
                    if(creatureTarget->isPet())
                        return;


                    GameObject* Crystal_Prison = m_caster->SummonGameObject(179644, creatureTarget->GetPositionX(), creatureTarget->GetPositionY(), creatureTarget->GetPositionZ(), creatureTarget->GetOrientation(), 0, 0, 0, 0, creatureTarget->GetRespawnTime()-time(NULL));
                    if (!Crystal_Prison)
                    {
                        delete Crystal_Prison;
                        return;
                    }

                    Crystal_Prison->SetSpellId(m_spellInfo->Id);

                    creatureTarget->ForcedDespawn();

                    sLog.outDebug("SummonGameObject at SpellEfects.cpp EffectDummy for Spell 23019\n");

                    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
                    data << uint64(Crystal_Prison->GetGUID());
                    m_caster->SendMessageToSet(&data,true);

                    return;
                }
                case 23074: // Arc. Dragonling
                    if (!m_CastItem) return;
                    m_caster->CastSpell(m_caster,19804,true,m_CastItem);
                    return;
                case 23075: // Mithril Mechanical Dragonling
                    if (!m_CastItem) return;
                    m_caster->CastSpell(m_caster,12749,true,m_CastItem);
                    return;
                case 23076: // Mechanical Dragonling
                    if (!m_CastItem) return;
                    m_caster->CastSpell(m_caster,4073,true,m_CastItem);
                    return;
                case 23133: // Gnomish Battle Chicken
                    if (!m_CastItem) return;
                    m_caster->CastSpell(m_caster,13166,true,m_CastItem);
                    return;
                case 23134: // Goblin Bomb Dispenser
                {
                    if (!m_CastItem)
                        return;

                    uint32 roll = urand(0, 99);

                    if (roll < 10)
                        unitTarget->CastSpell(m_caster, 13261, true);

                    else
                        m_caster->CastSpell(m_caster, 13258, true, m_CastItem);

                    return;
                }
                case 23138: // Gate of Shazzrah
                {
                    if (!unitTarget)
                        return;

                    // Effect probably include a threat change, but it is unclear if fully
                    // reset or just forced upon target for teleport (SMSG_HIGHEST_THREAT_UPDATE)
                    m_caster->AddThreat(unitTarget,1000,SPELL_SCHOOL_MASK_NORMAL);

                    // Gate of Shazzrah
                    m_caster->CastSpell(unitTarget, 23139, true);
                    return;
                }
                case 23185: // Aura of Frost
                case 25044: // Aura of Nature
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->HasAura(25040,0))         // Mark of Nature
                        unitTarget->CastSpell(unitTarget, 25043, true);

                    if (unitTarget->HasAura(23182,0))         // Mark of Frost
                        unitTarget->CastSpell(unitTarget, 23186, true);

                    unitTarget->RemoveAurasAtMechanicImmunity(1 << (MECHANIC_BANDAGE - 1), 0);
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                    if (!unitTarget->IsStandState())
                        unitTarget->SetStandState(UNIT_STAND_STATE_STAND);

                    return;
                }
                case 23448: // Ultrasafe Transporter: Gadgetzan - backfires
                {
                    int32 r = irand(0, 119);
                    if ( r < 20 )                           // 1/6 polymorph
                        m_caster->CastSpell(m_caster,23444,true);
                    else if ( r < 100 )                     // 4/6 evil twin
                        m_caster->CastSpell(m_caster,23445,true);
                    else                                    // 1/6 miss the target
                        m_caster->CastSpell(m_caster,36902,true);
                    return;
                }
                case 23453: // Ultrasafe Transporter: Gadgetzan
                    if ( roll_chance_i(50) )                // success
                        m_caster->CastSpell(m_caster,23441,true);
                    else                                    // failure
                    {
                        if (roll_chance_i(50))
                            m_caster->CastSpell(m_caster, 23446, true);
                        else
                        {
                            if (m_caster->GetTypeId() == TYPEID_PLAYER)
                                ((Player*)m_caster)->TeleportTo(1, -7341.38f, -3908.11f, 150.7f, 0.51f);
                        }
                    }
                    return;
                case 23645: // Hourglass Sand
                    m_caster->RemoveAurasDueToSpell(23170);
                    return;
                case 23725: // Gift of Life (warrior bwl trinket)
                {
                    int32 GiftOfLifeSpellBasePoints0 = int32(m_caster->GetMaxHealth()*0.15);
                    m_caster->CastCustomSpell(m_caster, 23782, &GiftOfLifeSpellBasePoints0, NULL, NULL, true, NULL);
                    m_caster->CastCustomSpell(m_caster, 23783, &GiftOfLifeSpellBasePoints0, NULL, NULL, true, NULL);
                    return;
                }
                case 24531: // Refocus : "Instantly clears the cooldowns of Aimed Shot, Multishot, Volley, and Arcane Shot."
                {
                    if(m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    const PlayerSpellMap& sp_list = ((Player *)m_caster)->GetSpellMap();
                    for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                    {
                        uint32 classspell = itr->first;
                        SpellEntry const *spellInfo = sSpellStore.LookupEntry(classspell);

                        // all spells with cooldown
                        if ((spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && GetSpellRecoveryTime(spellInfo) > 0))
                        {
                            switch (classspell)
                            {
                                // Volley
                                case 1510:
                                case 14294:
                                case 14295:
                                // Multi-Shot
                                case 2643:
                                case 14288:
                                case 14289:
                                case 14290:
                                case 25294:
                                // Arcane Shot
                                case 3044:
                                case 14281:
                                case 14282:
                                case 14283:
                                case 14284:
                                case 14285:
                                case 14286:
                                case 14287:
                                // Aimed Shot
                                case 19434:
                                case 20900:
                                case 20901:
                                case 20902:
                                case 20903:
                                case 20904:
                                case 27632:
                                {
                                    ((Player*)m_caster)->RemoveSpellCooldown(classspell);

                                    WorldPacket data(SMSG_CLEAR_COOLDOWN, (4+8));
                                    data << uint32(classspell);
                                    data << uint64(m_caster->GetGUID());
                                    ((Player*)m_caster)->GetSession()->SendPacket(&data);
                                    break;
                                }
                            }
                        }
                    }
                    return;
                }
                case 24781: // Dream Fog
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT || !unitTarget)
                        return;
                    // TODO Note: Should actually not only AttackStart, but fixate on the target
                    ((Creature*)m_caster)->AI()->AttackStart(unitTarget);
                    return;
                }
                case 24930: // Hallow's End Treat
                {
                    uint32 spell_id = 0;

                    switch (urand(1, 4))
                    {
                        case 1:
                            spell_id = 24924;
                            break;    // Larger and Orange
                        case 2:
                            spell_id = 24925;
                            break;    // Skeleton
                        case 3:
                            spell_id = 24926;
                            break;    // Pirate
                        case 4:
                            spell_id = 24927;
                            break;    // Ghost
                    }

                    m_caster->CastSpell(m_caster, spell_id, true);
                    return;
                }
                case 25716 : // Force Self - Bow
                {
                    m_caster->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    return;
                }
                case 25720: //spell Place Loot / Quest 8606 Decoy!
                {
                    if (!m_originalCaster)
                        return;
                    if (m_originalCaster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    sLog.outString("Quest 8606 item Dummy Effect.");
                    Player* pPlayer = (Player*)m_originalCaster;
                    // Check if player has Narain's outfit
                    if (!pPlayer->HasAura(25688,0))
                        return;
                    // Launch event 9527
                    sWorld.ScriptsStart(sEventScripts, 9527, pPlayer, focusObject);
                    // Delete item 21041
                    pPlayer->DestroyItemCount(21041, -1, true, false);
                    return;
                }
                case 25860: // Reindeer Transformation
                {
                    if (!m_caster->HasAuraType(SPELL_AURA_MOUNTED))
                        return;

                    float speed = m_caster->GetSpeedRate(MOVE_RUN);

                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    //2 different spells used depending on mounted speed
                    if (speed >= 2.0f)
                        m_caster->CastSpell(m_caster, 25859, true); //100% ground Reindeer
                    else
                        m_caster->CastSpell(m_caster, 25858, true); //60% ground Reindeer

                    return;
                }
                case 25938: // Viscidus explode
                {
                    if (!m_caster)
                        return;

                    if (m_caster->GetHealthPercent() < 5.0f) // Only way to kill this boss
                    {
                        m_caster->CastSpell(m_caster, 26003, true);
                        return;
                    }

                    // Stop combat
                    m_caster->CombatStop();

                    // Fake death
                    //m_caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEING_DEATH);
                    m_caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_caster->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    m_caster->addUnitState(UNIT_STAT_DIED);

                    // Summon globs
                    m_caster->CastSpell(m_caster, 25885, true);

                    return;
                }
                case 26003: // Viscidus suicide
                {
                    if (!m_caster)
                        return;

                    if (!m_caster->isAlive())
                        return;

                    m_caster->CastSpell(m_caster, 26002, true);
                    return;
                }
                case 26399: // Despawn Tentacles (C'thun)
                {
                    if (!m_caster)
                        return;

                    if (unitTarget)
                        m_caster->DealDamage(unitTarget, unitTarget->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);

                    return;
                }
                case 26626: // Mana Burn Area
                {
                    if (unitTarget->GetTypeId() != TYPEID_UNIT || unitTarget->getPowerType() != POWER_MANA)
                        return;

                    m_caster->CastSpell(unitTarget, 25779, true);
                    return;
                }
                case 27517: // Summon Thelrin DND (Banner of Provocation)
                    m_caster->SummonCreature(16059, 590.6309, -181.061, -53.90, 5.33, TEMPSUMMON_DEAD_DESPAWN, 0);
                    break;
                case 27798 : //Nature's Bounty
                {
                    switch(unitTarget->getPowerType())
                    {
                        case POWER_RAGE :
                            unitTarget->EnergizeBySpell(unitTarget, 27798, 10, POWER_RAGE);
                        return;
                        case POWER_ENERGY :
                            unitTarget->EnergizeBySpell(unitTarget, 27798, 40, POWER_ENERGY);
                        return;
                        case POWER_MANA :
                            unitTarget->EnergizeBySpell(unitTarget, 27798, 300, POWER_MANA);
                        return;
                    }
                    return;
                }
                case 28006: // Arcane Cloaking
                {
                    if(unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER )
                        m_caster->CastSpell(unitTarget,29294,true);
                    return;
                }
                case 28098: // Stalagg Tesla Effect
                case 28110: // Feugen Tesla Effect
                {
                    if (unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (m_caster->getVictim() && !m_caster->IsWithinDistInMap(unitTarget, 60.0f))
                    {
                        // Cast Shock on nearby targets
                        if (Unit* pTarget = ((Creature*)m_caster)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            unitTarget->CastSpell(pTarget, 28099, false);
                    }
                    else
                    {
                        // "Evade"
                        unitTarget->RemoveAurasDueToSpell(m_spellInfo->Id == 28098 ? 28097 : 28109);
                        unitTarget->DeleteThreatList();
                        unitTarget->CombatStop(true);
                        // Recast chain (Stalagg Chain or Feugen Chain
                        unitTarget->CastSpell(m_caster, m_spellInfo->Id == 28098 ? 28096 : 28111, false);
                    }
                    return;
                }
            }

            //All IconID Check in there
            switch(m_spellInfo->SpellIconID)
            {
                // Berserking (troll racial traits)
                case 1661:
                {
                    uint32 healthPerc = uint32((float(m_caster->GetHealth())/m_caster->GetMaxHealth())*100);
                    int32 melee_mod = 10;
                    if (healthPerc <= 40)
                        melee_mod = 30;
                    if (healthPerc < 100 && healthPerc > 40)
                        melee_mod = 10+(100-healthPerc)/3;

                    int32 hasteModBasePoints0 = melee_mod;          // (EffectBasePoints[0]+1)-1+(5-melee_mod) = (melee_mod-1+1)-1+5-melee_mod = 5-1
                    int32 hasteModBasePoints1 = (5-melee_mod);
                    int32 hasteModBasePoints2 = 5;

                    // FIXME: custom spell required this aura state by some unknown reason, we not need remove it anyway
                    m_caster->ModifyAuraState(AURA_STATE_BERSERKING,true);
                    m_caster->CastCustomSpell(m_caster,26635,&hasteModBasePoints0,&hasteModBasePoints1,&hasteModBasePoints2,true,NULL);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
            switch(m_spellInfo->Id )
            {
                case 11189: // Frost Warding
                case 28332:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // increase reflaction chanced (effect 1) of Frost Ward, removed in aura boosts
                    SpellModifier *mod = new SpellModifier;
                        mod->op = SPELLMOD_EFFECT2;
                        mod->value = damage;
                        mod->type = SPELLMOD_FLAT;
                        mod->spellId = m_spellInfo->Id;
                        mod->effectId = i;
                        mod->lastAffected = NULL;
                        mod->mask = ACE_UINT64_LITERAL(0x0000000000000100);
                        mod->charges = 0;
                    ((Player*)unitTarget)->AddSpellMod(mod, true);
                    break;
                }
                case 11094: // Improved Fire Ward
                case 13043:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // increase reflaction chanced (effect 1) of Fire Ward, removed in aura boosts
                    SpellModifier *mod = new SpellModifier;
                        mod->op = SPELLMOD_EFFECT2;
                        mod->value = damage;
                        mod->type = SPELLMOD_FLAT;
                        mod->spellId = m_spellInfo->Id;
                        mod->effectId = i;
                        mod->lastAffected = NULL;
                        mod->mask = ACE_UINT64_LITERAL(0x0000000000000008);
                        mod->charges = 0;
                    ((Player*)unitTarget)->AddSpellMod(mod, true);
                    break;
                }
                case 12472: // Cold Snap
                {
                    if(m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    // immediately finishes the cooldown on Frost spells
                    const PlayerSpellMap& sp_list = ((Player *)m_caster)->GetSpellMap();
                    for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                    {
                        if (itr->second->state == PLAYERSPELL_REMOVED)
                            continue;

                        uint32 classspell = itr->first;
                        SpellEntry const *spellInfo = sSpellStore.LookupEntry(classspell);

                        if( spellInfo->SpellFamilyName == SPELLFAMILY_MAGE &&
                            (spellInfo->School == SPELL_SCHOOL_FROST) &&
                            spellInfo->Id != m_spellInfo->Id && GetSpellRecoveryTime(spellInfo) > 0 )
                        {
                            ((Player*)m_caster)->RemoveSpellCooldown(classspell);

                            WorldPacket data(SMSG_CLEAR_COOLDOWN, (4+8));
                            data << uint32(classspell);
                            data << uint64(m_caster->GetGUID());
                            ((Player*)m_caster)->GetSession()->SendPacket(&data);
                        }
                    }
                    return;
                }
            }
            break;
        case SPELLFAMILY_WARRIOR:
            // Charge
            if(m_spellInfo->SpellFamilyFlags & 0x1 && m_spellInfo->SpellVisual == 867)
            {
                int32 chargeBasePoints0 = damage;
                m_caster->CastCustomSpell(m_caster,34846,&chargeBasePoints0,NULL,NULL,true);
                return;
            }
            // Execute
            if(m_spellInfo->SpellFamilyFlags & 0x20000000)
            {
                if(!unitTarget)
                    return;

                spell_id = 20647;
                bp = damage+int32(m_caster->GetPower(POWER_RAGE) * m_spellInfo->DmgMultiplier[i]);
                m_caster->CastCustomSpell(unitTarget, spell_id, &bp, NULL, NULL, true, 0);
                m_caster->SetPower(POWER_RAGE,0);
                break;
            }
            // Warrior's Wrath
            if(m_spellInfo->Id==21977)
            {
                if(!unitTarget)
                    return;

                m_caster->CastSpell(unitTarget,21887,true); // spell mod
                return;
            }
             // Nefarian Class Call Chaman Corrupted Totems
            if (m_spellInfo->Id == 23424)
            {
                switch (urand(0, 3))
                {
                    case 0:        // Corrupted Fire Nova Totem
                        m_caster->CastSpell(m_caster, 23419, true);
                        break;
                    case 1:        // Corrupted Stoneskin Totem
                        m_caster->CastSpell(m_caster, 23420, true);
                        break;
                    case 2:        // Corrupted Windfury Totem
                        m_caster->CastSpell(m_caster, 23423, true);
                        break;
                    case 3:        // Corrupted Healing Stream Totem
                        m_caster->CastSpell(m_caster, 23422, true);
                        break;
                }
                return;
            }
            break;
        case SPELLFAMILY_WARLOCK:
            //Life Tap (only it have this with dummy effect)
            if (m_spellInfo->SpellFamilyFlags == 0x40000)
            {
                float cost = damage;

                if(Player* modOwner = m_caster->GetSpellModOwner())
                    modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, cost,this);

                int32 dmg = m_caster->SpellDamageBonus(m_caster, m_spellInfo,uint32(cost > 0 ? cost : 0), SPELL_DIRECT_DAMAGE);

                if(int32(m_caster->GetHealth()) > dmg)
                {
                    // Shouldn't Appear in Combat Log
                    m_caster->ModifyHealth(-dmg);

                    int32 mana = dmg;

                    Unit::AuraList const& auraDummy = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = auraDummy.begin(); itr != auraDummy.end(); ++itr)
                    {
                        // only Imp. Life Tap have this in combination with dummy aura
                        if((*itr)->GetSpellProto()->SpellFamilyName==SPELLFAMILY_WARLOCK && (*itr)->GetSpellProto()->SpellIconID == 208)
                            mana = ((*itr)->GetModifier()->m_amount + 100)* mana / 100;
                    }

                    m_caster->ModifyPower(POWER_MANA,dmg);

                    // Mana Feed
                    int32 manaFeedVal = m_caster->CalculateSpellDamage(m_spellInfo,1, m_spellInfo->EffectBasePoints[1],m_caster);
                    manaFeedVal = manaFeedVal * mana / 100;
                    if(manaFeedVal > 0)
                        m_caster->CastCustomSpell(m_caster,32553,&manaFeedVal,NULL,NULL,true,NULL);
                }
                else
                    SendCastResult(SPELL_FAILED_FIZZLE);
                return;
            }
            break;
        case SPELLFAMILY_PRIEST:
            switch(m_spellInfo->Id )
            {
                case 28598: // Touch of Weakness triggered spell
                {
                    if(!unitTarget || !m_triggeredByAuraSpell)
                        return;

                    uint32 spellid = 0;
                    switch(m_triggeredByAuraSpell->Id)
                    {
                        case 2652:  spellid =  2943; break; // Rank 1
                        case 19261: spellid = 19249; break; // Rank 2
                        case 19262: spellid = 19251; break; // Rank 3
                        case 19264: spellid = 19252; break; // Rank 4
                        case 19265: spellid = 19253; break; // Rank 5
                        case 19266: spellid = 19254; break; // Rank 6
                        case 25461: spellid = 25460; break; // Rank 7
                        default:
                            sLog.outError("Spell::EffectDummy: Spell 28598 triggered by unhandled spell %u",m_triggeredByAuraSpell->Id);
                            return;
                    }
                    m_caster->CastSpell(unitTarget, spellid, true, NULL);
                    return;
                }
            }
            break;
        case SPELLFAMILY_DRUID:
            switch(m_spellInfo->Id )
            {
                case 5420:  // Tree of Life passive
                {
                    // Tree of Life area effect
                    int32 health_mod = int32(m_caster->GetStat(STAT_SPIRIT)/4);
                    m_caster->CastCustomSpell(m_caster,34123,&health_mod,NULL,NULL,true,NULL);
                    return;
                }
            }
            break;
        case SPELLFAMILY_ROGUE:
            switch(m_spellInfo->Id )
            {
                case 5938:  // Shiv
                {
                    if(m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player *pCaster = ((Player*)m_caster);

                    Item *item = pCaster->GetWeaponForAttack(OFF_ATTACK);
                    if(!item)
                        return;

                    // all poison enchantments is temporary
                    uint32 enchant_id = item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT);
                    if(!enchant_id)
                        return;

                    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if(!pEnchant)
                        return;

                    for (int s=0;s<3;s++)
                    {
                        if(pEnchant->type[s]!=ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                            continue;

                        SpellEntry const* combatEntry = sSpellStore.LookupEntry(pEnchant->spellid[s]);
                        if(!combatEntry || combatEntry->Dispel != DISPEL_POISON)
                            continue;

                        m_caster->CastSpell(unitTarget, combatEntry, true, item);
                    }

                    m_caster->CastSpell(unitTarget, 5940, true);
                    return;
                }
            }
            break;
        case SPELLFAMILY_HUNTER:
            switch(m_spellInfo->Id)
            {
                case 23989: //Readiness talent
                {
                    if(m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    //immediately finishes the cooldown for hunter abilities
                    const PlayerSpellMap& sp_list = ((Player *)m_caster)->GetSpellMap();
                    for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                    {
                        uint32 classspell = itr->first;
                        SpellEntry const *spellInfo = sSpellStore.LookupEntry(classspell);

                        if (spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && spellInfo->Id != 23989 && GetSpellRecoveryTime(spellInfo) > 0 )
                        {
                            ((Player*)m_caster)->RemoveSpellCooldown(classspell);

                            WorldPacket data(SMSG_CLEAR_COOLDOWN, (4+8));
                            data << uint32(classspell);
                            data << uint64(m_caster->GetGUID());
                            ((Player*)m_caster)->GetSession()->SendPacket(&data);
                        }
                    }
                    return;
                }
            }
            break;
        case SPELLFAMILY_PALADIN:
            switch(m_spellInfo->SpellIconID)
            {
                case  156:  // Holy Shock
                {
                    if(!unitTarget)
                        return;

                    int hurt = 0;
                    int heal = 0;

                    switch(m_spellInfo->Id)
                    {
                        case 20473: hurt = 25912; heal = 25914; break;
                        case 20929: hurt = 25911; heal = 25913; break;
                        case 20930: hurt = 25902; heal = 25903; break;
                        default:
                            sLog.outError("Spell::EffectDummy: Spell %u not handled in HS",m_spellInfo->Id);
                            return;
                    }

                    if(m_caster->IsFriendlyTo(unitTarget))
                        m_caster->CastSpell(unitTarget, heal, true, 0);
                    else
                        m_caster->CastSpell(unitTarget, hurt, true, 0);

                    return;
                }
                case 561:   // Judgement of command
                {
                    if(!unitTarget)
                        return;

                    uint32 spell_id = m_spellInfo->EffectBasePoints[i]+1;//m_currentBasePoints[i]+1;
                    SpellEntry const* spell_proto = sSpellStore.LookupEntry(spell_id);
                    if(!spell_proto)
                        return;

                    if( !unitTarget->hasUnitState(UNIT_STAT_STUNNED) && m_caster->GetTypeId()==TYPEID_PLAYER)
                    {
                        // decreased damage (/2) for non-stunned target.
                        SpellModifier *mod = new SpellModifier;
                        mod->op = SPELLMOD_DAMAGE;
                        mod->value = -50;
                        mod->type = SPELLMOD_PCT;
                        mod->spellId = m_spellInfo->Id;
                        mod->effectId = i;
                        mod->lastAffected = NULL;
                        mod->mask = 0x0000020000000000LL;
                        mod->charges = 0;

                        ((Player*)m_caster)->AddSpellMod(mod, true);
                        m_caster->CastSpell(unitTarget,spell_proto,true,NULL);
                                                            // mod deleted
                        ((Player*)m_caster)->AddSpellMod(mod, false);
                    }
                    else
                        m_caster->CastSpell(unitTarget,spell_proto,true,NULL);

                    return;
                }
            }
            break;
        case SPELLFAMILY_SHAMAN:
            //Shaman Rockbiter Weapon
            if (m_spellInfo->SpellFamilyFlags == 0x400000)
            {
                uint32 spell_id = 0;
                switch(m_spellInfo->Id)
                {
                    case  8017: spell_id = 36494; break;    // Rank 1
                    case  8018: spell_id = 36750; break;    // Rank 2
                    case  8019: spell_id = 36755; break;    // Rank 3
                    case 10399: spell_id = 36759; break;    // Rank 4
                    case 16314: spell_id = 36763; break;    // Rank 5
                    case 16315: spell_id = 36766; break;    // Rank 6
                    case 16316: spell_id = 36771; break;    // Rank 7
                    default:
                        sLog.outError("Spell::EffectDummy: Spell %u not handled in RW",m_spellInfo->Id);
                        return;
                }

                SpellEntry const *spellInfo = sSpellStore.LookupEntry( spell_id );

                if(!spellInfo)
                {
                    sLog.outError("WORLD: unknown spell id %i\n", spell_id);
                    return;
                }

                if(m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                for(int i = BASE_ATTACK; i <= OFF_ATTACK; ++i)
                {
                    if(Item* item = ((Player*)m_caster)->GetWeaponForAttack(WeaponAttackType(i)))
                    {
                        if(item->IsFitToSpellRequirements(m_spellInfo))
                        {
                            Spell *spell = new Spell(m_caster, spellInfo, true);

                            // enchanting spell selected by calculated damage-per-sec in enchanting effect
                            // at calculation applied affect from Elemental Weapons talent
                            // real enchantment damage-1
                            spell->m_currentBasePoints[1] = damage-1;

                            SpellCastTargets targets;
                            targets.setItemTarget( item );
                            spell->prepare(&targets);
                        }
                    }
                }
                return;
            }

            // Flametongue Weapon Proc, Ranks
            if (m_spellInfo->SpellFamilyFlags & SPELLFAMILYFLAG_SHAMAN_FLAMETONGUE)
            {
                if (!m_CastItem)
                {
                    sLog.outError("Spell::EffectDummy: spell %i requires cast Item", m_spellInfo->Id);
                    return;
                }
                // found spelldamage coefficients of 0.381% per 0.1 speed and 15.244 per 4.0 speed
                // but own calculation say 0.385 gives at most one point difference to published values
                int32 spellDamage = m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo));
                float weaponSpeed = (1.0f / 1000) * m_CastItem->GetProto()->Delay;
                int32 totalDamage = int32((damage + 3.85f * spellDamage) * 0.01 * weaponSpeed);

                m_caster->CastCustomSpell(unitTarget, 10444, &totalDamage, NULL, NULL, true, m_CastItem);
                return;
            }

            break;
    }

    //spells triggered by dummy effect should not miss
    if(spell_id)
    {
        SpellEntry const *spellInfo = sSpellStore.LookupEntry( spell_id );

        if(!spellInfo)
        {
            sLog.outError("EffectDummy of spell %u: triggering unknown spell id %i\n", m_spellInfo->Id, spell_id);
            return;
        }

        Spell* spell = new Spell(m_caster, spellInfo, true, m_originalCasterGUID, NULL, true);
        if(bp) spell->m_currentBasePoints[0] = bp;
        SpellCastTargets targets;
        targets.setUnitTarget(unitTarget);
        spell->prepare(&targets);
    }

    // pet auras
    if(PetAura const* petSpell = spellmgr.GetPetAura(m_spellInfo->Id))
    {
        m_caster->AddPetAura(petSpell);
        return;
    }
}

void Spell::EffectTriggerSpellWithValue(uint32 i)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[i];

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        sLog.outError("EffectTriggerSpellWithValue of spell %u: triggering unknown spell id %i\n", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    int32 bp = damage;
    m_caster->CastCustomSpell(unitTarget,triggered_spell_id,&bp,&bp,&bp,true,NULL,NULL,m_originalCasterGUID);
}

void Spell::EffectTriggerRitualOfSummoning(uint32 i)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[i];
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        sLog.outError("EffectTriggerRitualOfSummoning of spell %u: triggering unknown spell id %i", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    finish();
    Spell *spell = new Spell(m_caster, spellInfo, true);

    SpellCastTargets targets;
    targets.setUnitTarget( unitTarget);
    spell->prepare(&targets);

    m_caster->SetCurrentCastedSpell(spell);
    spell->m_selfContainer = &(m_caster->m_currentSpells[spell->GetCurrentContainer()]);

}

void Spell::EffectForceCast(uint32 i)
{
    if( !unitTarget )
        return;

    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[i];

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        sLog.outError("EffectForceCast of spell %u: triggering unknown spell id %i", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    unitTarget->CastSpell(unitTarget,spellInfo,true,NULL,NULL,m_originalCasterGUID);
}

void Spell::EffectTriggerSpell(uint32 i)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[i];

    // special cases
    switch(triggered_spell_id)
    {
        // Vanish
        case 18461:
        {
            m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
            m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
            m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_STALKED);

            // if this spell is given to NPC it must handle rest by it's own AI
            if ( m_caster->GetTypeId() != TYPEID_PLAYER )
                return;

            // get highest rank of the Stealth spell
            uint32 spellId = 0;
            const PlayerSpellMap& sp_list = ((Player*)m_caster)->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
            {
                // only highest rank is shown in spell book, so simply check if shown in spell book
                if(!itr->second->active || itr->second->disabled || itr->second->state == PLAYERSPELL_REMOVED)
                    continue;

                SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
                if (!spellInfo)
                    continue;

                if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && spellInfo->SpellFamilyFlags & SPELLFAMILYFLAG_ROGUE_STEALTH)
                {
                    spellId = spellInfo->Id;
                    break;
                }
            }

            // no Stealth spell found
            if (!spellId)
                return;

            // reset cooldown on it if needed
            if(((Player*)m_caster)->HasSpellCooldown(spellId))
                ((Player*)m_caster)->RemoveSpellCooldown(spellId);

            m_caster->CastSpell(m_caster, spellId, true);
            return;
        }
        // just skip
        case 23770:                                         // Sayge's Dark Fortune of *
            // not exist, common cooldown can be implemented in scripts if need.
            return;
        // Brittle Armor - (need add max stack of 24575 Brittle Armor)
        case 29284:
        {
            const SpellEntry *spell = sSpellStore.LookupEntry(24575);
            if (!spell)
                return;

            for (int i=0; i < spell->StackAmount; ++i)
                m_caster->CastSpell(unitTarget,spell->Id, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        }
        // Mercurial Shield - (need add max stack of 26464 Mercurial Shield)
        case 29286:
        {
            const SpellEntry *spell = sSpellStore.LookupEntry(26464);
            if (!spell)
                return;

            for (int i=0; i < spell->StackAmount; ++i)
                m_caster->CastSpell(unitTarget,spell->Id, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        }
        // Righteous Defense
        case 31980:
        {
            m_caster->CastSpell(unitTarget, 31790, true,m_CastItem,NULL,m_originalCasterGUID);
            return;
        }
        // Cloak of Shadows
        case 35729 :
        {
            uint32 dispelMask = GetDispellMask(DISPEL_ALL);
            Unit::AuraMap& Auras = m_caster->GetAuras();
            for(Unit::AuraMap::iterator iter = Auras.begin(); iter != Auras.end(); ++iter)
            {
                // remove all harmful spells on you...
                SpellEntry const* spell = iter->second->GetSpellProto();
                if((spell->DmgClass == SPELL_DAMAGE_CLASS_MAGIC // only affect magic spells
                    || ((1<<spell->Dispel) & dispelMask))
                    // ignore positive and passive auras
                    && !iter->second->IsPositive() && !iter->second->IsPassive())
                {
                    m_caster->RemoveAurasDueToSpell(spell->Id);
                    iter = Auras.begin();
                }
            }
            return;
        }
        // Priest Shadowfiend (34433) need apply mana gain trigger aura on pet
        case 41967:
        {
            if (Unit *pet = m_caster->GetPet())
                pet->CastSpell(pet, 28305, true);
            return;
        }
    }

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        sLog.outError("EffectTriggerSpell of spell %u: triggering unknown spell id %i", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    // some triggered spells require specific equipment
    if(spellInfo->EquippedItemClass >=0 && m_caster->GetTypeId()==TYPEID_PLAYER)
    {
        // main hand weapon required
        if(spellInfo->AttributesEx3 & SPELL_ATTR_EX3_MAIN_HAND)
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(BASE_ATTACK);

            // skip spell if no weapon in slot or broken
            if(!item || item->IsBroken() )
                return;

            // skip spell if weapon not fit to triggered spell
            if(!item->IsFitToSpellRequirements(spellInfo))
                return;
        }

        // offhand hand weapon required
        if(spellInfo->AttributesEx3 & SPELL_ATTR_EX3_REQ_OFFHAND)
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(OFF_ATTACK);

            // skip spell if no weapon in slot or broken
            if(!item || item->IsBroken() )
                return;

            // skip spell if weapon not fit to triggered spell
            if(!item->IsFitToSpellRequirements(spellInfo))
                return;
        }
    }

    // some triggered spells must be casted instantly (for example, if next effect case instant kill caster)
    bool instant = false;
    for(uint32 j = i+1; j < 3; ++j)
    {
        if(m_spellInfo->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER
            && (m_spellInfo->Effect[j]==SPELL_EFFECT_INSTAKILL 
            || m_spellInfo->Effect[j]==SPELL_EFFECT_SANCTUARY))
        {
            instant = true;
            break;
        }
    }

    if(instant)
    {
        if (unitTarget)
            m_caster->CastSpell(unitTarget,spellInfo,true,m_CastItem,NULL,m_originalCasterGUID);
    }
    else
        m_TriggerSpells.push_back(spellInfo);
}

void Spell::EffectTriggerMissileSpell(uint32 effect_idx)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effect_idx];

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        sLog.outError("EffectTriggerMissileSpell of spell %u (eff: %u): triggering unknown spell id %u",
            m_spellInfo->Id,effect_idx,triggered_spell_id);
        return;
    }

    if (m_CastItem)
        DEBUG_LOG("WORLD: cast Item spellId - %i", spellInfo->Id);

    Spell *spell = new Spell(m_caster, spellInfo, true, m_originalCasterGUID );

    SpellCastTargets targets;
    targets.setDestination(m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ);
    spell->m_CastItem = m_CastItem;
    spell->prepare(&targets, NULL);
}

void Spell::EffectTeleportUnits(uint32 i)
{
    if(!unitTarget || unitTarget->isInFlight())
        return;

    // If not exist data for dest location - return
    if(!m_targets.HasDst())
    {
        sLog.outError( "Spell::EffectTeleportUnits - does not have destination for spell ID %u\n", m_spellInfo->Id );
        return;
    }
    // Init dest coordinates
    int32 mapid = m_targets.m_mapId;
    if(mapid < 0) mapid = (int32)unitTarget->GetMapId();
    float x = m_targets.m_destX;
    float y = m_targets.m_destY;
    float z = m_targets.m_destZ;
    float orientation = m_targets.getUnitTarget() ? m_targets.getUnitTarget()->GetOrientation() : unitTarget->GetOrientation();
    sLog.outDebug("Spell::EffectTeleportUnits - teleport unit to %u %f %f %f\n", mapid, x, y, z);
    // Teleport
    if(unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->TeleportTo(mapid, x, y, z, orientation, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget==m_caster ? TELE_TO_SPELL : 0));
    else
    {
        MapManager::Instance().GetMap(mapid, m_caster)->CreatureRelocation((Creature*)unitTarget, x, y, z, orientation);
        WorldPacket data;
        unitTarget->BuildTeleportAckMsg(&data, x, y, z, orientation);
        unitTarget->SendMessageToSet(&data, false);
    }

    // post effects for TARGET_DST_DB
    switch ( m_spellInfo->Id )
    {
        // Dimensional Ripper - Everlook
        case 23442:
        {
            int32 r = irand(0, 119);
            if ( r >= 70 )                                  // 7/12 success
            {
                if ( r < 100 )                              // 4/12 evil twin
                    m_caster->CastSpell(m_caster,23445,true);
                else                                        // 1/12 fire
                    m_caster->CastSpell(m_caster,23449,true);
            }
            return;
        }
    }
}

void Spell::EffectApplyAura(uint32 i)
{
    if(!unitTarget)
        return;

    SpellImmuneList const& list = unitTarget->m_spellImmune[IMMUNITY_STATE];
    for(SpellImmuneList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
        if(itr->type == m_spellInfo->EffectApplyAuraName[i])
            return;

    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    if( !unitTarget->isAlive() && m_spellInfo->Id != 20584 && m_spellInfo->Id != 8326 &&
        (unitTarget->GetTypeId()!=TYPEID_PLAYER || !((Player*)unitTarget)->GetSession()->PlayerLoading()) )
        return;

    Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;
    if(!caster)
        return;

    sLog.outDebug("Spell: Aura is: %u", m_spellInfo->EffectApplyAuraName[i]);

    // Sayge's Dark Fortune: +1-10% (random) (Youfie)
    if (m_spellInfo->SpellIconID == 1595 && m_spellInfo->SpellVisual == 7042)
        damage = m_spellInfo->Id == 23769 ? urand(1, 25) : urand(1, 10);
    // Gnomish Death Ray
    // rarely has a chance of dealing double damage, 14.29% chance (guess)
    // for now we use linear level scaling, but this is likely incorrect (hp pools don't scale exactly linearly)
    // there is some speculation that this should be tied to Engineering skill level, but since you don't need Engineering to use the item at all this seems doubtful
    else if (m_spellInfo->Id == 13278)
        damage = i == 0 ? int32(urand(600, 1200) * (caster->getLevel() / 60.0f)) * (!urand(0,6) ? 2 : 1)
                                                                 : m_currentBasePoints[0] * 0.1249f;

    Aura* Aur = CreateAura(m_spellInfo, i, &damage, unitTarget, caster, m_CastItem);

    // Now Reduce spell duration using data received at spell hit
    int32 duration = Aur->GetAuraMaxDuration();
    unitTarget->ApplyDiminishingToDuration(m_diminishGroup,duration,caster,m_diminishLevel);
    Aur->setDiminishGroup(m_diminishGroup);

    // if Aura removed and deleted, do not continue.
    if(duration== 0 && !(Aur->IsPermanent()))
    {
        delete Aur;
        return;
    }

    if(duration != Aur->GetAuraMaxDuration())
    {
        Aur->SetAuraMaxDuration(duration);
        Aur->SetAuraDuration(duration);
    }

    bool added = unitTarget->AddAura(Aur);

    // Aura not added and deleted in AddAura call;
    if (!added)
        return;

    // found crash at character loading, broken pointer to Aur...
    // Aur was deleted in AddAura()...
    if(!Aur)
        return;

    // TODO Make a way so it works for every related spell!
    if(unitTarget->GetTypeId()==TYPEID_PLAYER ||( unitTarget->GetTypeId()==TYPEID_UNIT && ((Creature*)unitTarget)->isPet() ) )              // Negative buff should only be applied on players
    {
        uint32 spellId = 0;
        AuraStates const *SpellCasterAuraState = spellmgr.GetCasterAuraStates(m_spellInfo->Id);
        AuraStates const *SpellTargetAuraState = spellmgr.GetTargetAuraStates(m_spellInfo->Id);
        uint32 casterAuraStateFlag = 0;
        uint32 targetAuraStateFlag = 0;

        if(SpellCasterAuraState)
            casterAuraStateFlag = SpellCasterAuraState->AuraStateNot;
        if(SpellTargetAuraState)
            targetAuraStateFlag = SpellTargetAuraState->AuraStateNot;

        if(casterAuraStateFlag == AURA_STATE_WEAKENED_SOUL || targetAuraStateFlag == AURA_STATE_WEAKENED_SOUL)
            spellId = 6788;                                 // Weakened Soul
        else if(casterAuraStateFlag == AURA_STATE_FORBEARANCE || targetAuraStateFlag == AURA_STATE_FORBEARANCE)
            spellId = 25771;                                // Forbearance

        if (m_spellInfo->Mechanic == MECHANIC_BANDAGE) // Bandages
            spellId = 11196;                                // Recently Bandaged
        else if( (m_spellInfo->AttributesEx & 0x20) && (m_spellInfo->AttributesEx2 & 0x20000) )
            spellId = 23230;                                // Blood Fury - Healing Reduction

        SpellEntry const *AdditionalSpellInfo = sSpellStore.LookupEntry(spellId);
        if (AdditionalSpellInfo)
        {
            // applied at target by target
            Aura* AdditionalAura = CreateAura(AdditionalSpellInfo, 0, NULL, unitTarget,unitTarget, 0);
            unitTarget->AddAura(AdditionalAura);
            sLog.outDebug("Spell: Additional Aura is: %u", AdditionalSpellInfo->EffectApplyAuraName[0]);
        }
    }
}

void Spell::EffectUnlearnSpecialization( uint32 i )
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)unitTarget;
    uint32 spellToUnlearn = m_spellInfo->EffectTriggerSpell[i];

    _player->removeSpell(spellToUnlearn);

    sLog.outDebug( "Spell: Player %u have unlearned spell %u from NpcGUID: %u", _player->GetGUIDLow(), spellToUnlearn, m_caster->GetGUIDLow() );
}

void Spell::EffectPowerDrain(uint32 i)
{
    if(m_spellInfo->EffectMiscValue[i] < 0 || m_spellInfo->EffectMiscValue[i] >= MAX_POWERS)
        return;

    Powers drain_power = Powers(m_spellInfo->EffectMiscValue[i]);

    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;
    if(unitTarget->getPowerType() != drain_power)
        return;
    if(damage < 0)
        return;

    uint32 curPower = unitTarget->GetPower(drain_power);

    //add spell damage bonus
    damage=m_caster->SpellDamageBonus(unitTarget,m_spellInfo,uint32(damage),SPELL_DIRECT_DAMAGE);

    int32 new_damage;
    if (curPower < damage)
        new_damage = curPower;
    else
        new_damage = damage;

    unitTarget->ModifyPower(drain_power,-(int32)new_damage);

    // Don`t restore from self drain
    if(drain_power == POWER_MANA && m_caster != unitTarget)
    {
        float manaMultiplier = m_spellInfo->EffectMultipleValue[i];
        if(manaMultiplier==0)
            manaMultiplier = 1;

        if(Player *modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, manaMultiplier);

        int32 gain = int32(new_damage*manaMultiplier);

        m_caster->ModifyPower(POWER_MANA,gain);
        //send log
        m_caster->SendEnergizeSpellLog(m_caster, m_spellInfo->Id,gain,POWER_MANA);
    }
}

void Spell::EffectSendEvent(uint32 EffectIndex)
{
    if (m_caster->GetTypeId() == TYPEID_PLAYER && ((Player*)m_caster)->InBattleGround())
    {
        BattleGround* bg = ((Player *)m_caster)->GetBattleGround();
        if(bg && bg->GetStatus() == STATUS_IN_PROGRESS)
        {
            switch(m_spellInfo->Id)
            {
                case 23333:                                 // Pickup Horde Flag
                    /*do not uncomment .
                    if(bg->GetTypeID()==BATTLEGROUND_WS)
                        bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                    sLog.outDebug("Send Event Horde Flag Picked Up");
                    break;
                    /* not used :
                    case 23334:                                 // Drop Horde Flag
                        if(bg->GetTypeID()==BATTLEGROUND_WS)
                            bg->EventPlayerDroppedFlag((Player*)m_caster);
                        sLog.outDebug("Drop Horde Flag");
                        break;
                    */
                case 23335:                                 // Pickup Alliance Flag
                    /*do not uncomment ... (it will cause crash, because of null targetobject!) anyway this is a bad way to call that event, because it would cause recursion
                    if(bg->GetTypeID()==BATTLEGROUND_WS)
                        bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                    sLog.outDebug("Send Event Alliance Flag Picked Up");
                    break;
                    /* not used :
                    case 23336:                                 // Drop Alliance Flag
                        if(bg->GetTypeID()==BATTLEGROUND_WS)
                            bg->EventPlayerDroppedFlag((Player*)m_caster);
                        sLog.outDebug("Drop Alliance Flag");
                        break;
                    case 23385:                                 // Alliance Flag Returns
                        if(bg->GetTypeID()==BATTLEGROUND_WS)
                            bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                        sLog.outDebug("Alliance Flag Returned");
                        break;
                    case 23386:                                   // Horde Flag Returns
                        if(bg->GetTypeID()==BATTLEGROUND_WS)
                            bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                        sLog.outDebug("Horde Flag Returned");
                        break;*/
                case 34976:
                    /*
                    if(bg->GetTypeID()==BATTLEGROUND_EY)
                        bg->EventPlayerClickedOnFlag((Player*)m_caster, gameObjTarget);
                    */
                    break;
                default:
                    sLog.outDebug("Unknown spellid %u in BG event", m_spellInfo->Id);
                    break;
            }
        }
    }
    sLog.outDebug("Spell ScriptStart %u for spellid %u in EffectSendEvent ", m_spellInfo->EffectMiscValue[EffectIndex], m_spellInfo->Id);
    sWorld.ScriptsStart(sEventScripts, m_spellInfo->EffectMiscValue[EffectIndex], m_caster, focusObject);
}

void Spell::EffectPowerBurn(uint32 i)
{
    if(m_spellInfo->EffectMiscValue[i] < 0 || m_spellInfo->EffectMiscValue[i] >= MAX_POWERS)
        return;

    Powers powertype = Powers(m_spellInfo->EffectMiscValue[i]);

    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;
    if(unitTarget->getPowerType()!=powertype)
        return;
    if(damage < 0)
        return;

    unitTarget->ModifyPower(powertype, -damage);

    float multiplier = m_spellInfo->EffectMultipleValue[i];
    if(Player *modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    int32 new_damage = int32(damage*multiplier);
    if(m_originalCaster)
        m_originalCaster->DealDamage(unitTarget, new_damage);
}

void Spell::EffectHeal( uint32 /*i*/ )
{
}

void Spell::SpellDamageHeal(uint32 /*i*/)
{
    if( unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        int32 addhealth = damage;

        // Holy Light and Flash of Light - Increase healing amount by Blessing of Light
        if ((m_spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN) && ((m_spellInfo->SpellIconID  == 242) || (m_spellInfo->SpellIconID  == 70)))
        {
            int damageAmount = 0;
            Unit::AuraList const& mDummyAuras = unitTarget->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator i = mDummyAuras.begin();i != mDummyAuras.end(); ++i)
            {
                SpellEntry const *spell = (*i)->GetSpellProto();
                if(((spell->SpellIconID == 540) || (spell->SpellIconID == 1801)) && (spell->SpellFamilyName == SPELLFAMILY_PALADIN))
                {
                    if ((m_spellInfo->SpellIconID  == 70) && ((*i)->GetEffIndex() == 0))
                    {
                        damageAmount+=(*i)->GetModifierValue();
                        break;
                    }
                    else if ((m_spellInfo->SpellIconID  == 242) && ((*i)->GetEffIndex() == 1))
                    {
                        damageAmount+=(*i)->GetModifierValue();
                        break;
                    }
                }
            }

            addhealth = caster->SpellHealingBonus(m_spellInfo, addhealth,HEAL, unitTarget);
            addhealth += damageAmount;
        }
        // Swiftmend - consumes Regrowth or Rejuvenation
        else if(AuraStates const *SpellTargetAuraStates = spellmgr.GetTargetAuraStates(m_spellInfo->Id))
        {
            if(SpellTargetAuraStates->AuraState == AURA_STATE_SWIFTMEND && unitTarget->HasAuraState(AURA_STATE_SWIFTMEND))
            {
                Unit::AuraList const& RejorRegr = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
                // find most short by duration
                Aura *targetAura = NULL;
                for(Unit::AuraList::const_iterator i = RejorRegr.begin(); i != RejorRegr.end(); ++i)
                {
                    if((*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID
                        && ((*i)->GetSpellProto()->SpellFamilyFlags == 0x40 || (*i)->GetSpellProto()->SpellFamilyFlags == 0x10) )
                    {
                        if(!targetAura || (*i)->GetAuraDuration() < targetAura->GetAuraDuration())
                            targetAura = *i;
                    }
                }

                if(!targetAura)
                {
                    sLog.outError("Target(GUID:" I64FMTD ") has aurastate AURA_STATE_SWIFTMEND but no matching aura.", unitTarget->GetGUID());
                    return;
                }

                int32 tickheal = targetAura->GetModifierValuePerStack();
                if(Unit* auraCaster = targetAura->GetCaster())
                    tickheal = auraCaster->SpellHealingBonus(targetAura->GetSpellProto(), tickheal, DOT, unitTarget);
                //int32 tickheal = targetAura->GetSpellProto()->EffectBasePoints[idx] + 1;
                //It is said that talent bonus should not be included
                //int32 tickheal = targetAura->GetModifierValue();
                int32 tickcount = 0;
                if(targetAura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID)
                {
                    switch(targetAura->GetSpellProto()->SpellFamilyFlags)//TODO: proper spellfamily for 3.0.x
                    {
                        case 0x10:  tickcount = 4;  break; // Rejuvenation
                        case 0x40:  tickcount = 6;  break; // Regrowth
                    }
                }
                addhealth += tickheal * tickcount;
                unitTarget->RemoveAurasByCasterSpell(targetAura->GetId(), targetAura->GetCasterGUID());

                //addhealth += tickheal * tickcount;
                //addhealth = caster->SpellHealingBonus(m_spellInfo, addhealth,HEAL, unitTarget);
            }
        }
        else
            addhealth = caster->SpellHealingBonus(m_spellInfo, addhealth,HEAL, unitTarget);

        m_damage -= addhealth;
    }
}

void Spell::EffectHealPct( uint32 /*i*/ )
{
    if( unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        uint32 addhealth = unitTarget->GetMaxHealth() * damage / 100;
        caster->SendHealSpellLog(unitTarget, m_spellInfo->Id, addhealth, false);

        int32 gain = unitTarget->ModifyHealth( int32(addhealth) );
        unitTarget->getHostilRefManager().threatAssist(m_caster, float(gain) * 0.5f, m_spellInfo);

        if(caster->GetTypeId()==TYPEID_PLAYER)
            if(BattleGround *bg = ((Player*)caster)->GetBattleGround())
                bg->UpdatePlayerScore(((Player*)caster), SCORE_HEALING_DONE, gain);
    }
}

void Spell::EffectHealMechanical( uint32 /*i*/ )
{
    // Mechanic creature type should be correctly checked by targetCreatureType field
    if( unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        uint32 addhealth = caster->SpellHealingBonus(m_spellInfo, uint32(damage), HEAL, unitTarget);
        caster->SendHealSpellLog(unitTarget, m_spellInfo->Id, addhealth, false);
        unitTarget->ModifyHealth( int32(damage) );
    }
}

void Spell::EffectHealthLeech(uint32 i)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    if(damage < 0)
        return;

    sLog.outDebug("HealthLeech :%i", damage);

    float multiplier = m_spellInfo->EffectMultipleValue[i];

    if(Player *modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    int32 new_damage = int32(damage*multiplier);
    uint32 curHealth = unitTarget->GetHealth();
    new_damage = m_caster->SpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, new_damage, m_IsTriggeredSpell, true);
    if(curHealth < new_damage)
        new_damage = curHealth;

    if(m_caster->isAlive())
    {
        new_damage = m_caster->SpellHealingBonus(m_spellInfo, new_damage, HEAL, m_caster);

        m_caster->ModifyHealth(new_damage);

        if(m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->SendHealSpellLog(m_caster, m_spellInfo->Id, uint32(new_damage));
    }
//    m_healthLeech+=tmpvalue;
//    m_damage+=new_damage;
}

void Spell::DoCreateItem(uint32 i, uint32 itemtype)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    uint32 newitemid = itemtype;
    ItemPrototype const *pProto = objmgr.GetItemPrototype( newitemid );
    if(!pProto)
    {
        player->SendEquipError( EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL );
        return;
    }

    uint32 num_to_add;

    // TODO: maybe all this can be replaced by using correct calculated `damage` value
    if(pProto->Class != ITEM_CLASS_CONSUMABLE || m_spellInfo->SpellFamilyName != SPELLFAMILY_MAGE)
    {
        num_to_add = damage;
        /*int32 basePoints = m_currentBasePoints[i];
        int32 randomPoints = m_spellInfo->EffectDieSides[i];
        if (randomPoints)
            num_to_add = basePoints + irand(1, randomPoints);
        else
            num_to_add = basePoints + 1;*/
    }
    else if (pProto->MaxCount == 1)
        num_to_add = 1;
    else if(player->getLevel() >= m_spellInfo->spellLevel)
    {
        num_to_add = damage;
        /*int32 basePoints = m_currentBasePoints[i];
        float pointPerLevel = m_spellInfo->EffectRealPointsPerLevel[i];
        num_to_add = basePoints + 1 + uint32((player->getLevel() - m_spellInfo->spellLevel)*pointPerLevel);*/
    }
    else
        num_to_add = 2;

    if (num_to_add < 1)
        num_to_add = 1;
    if (num_to_add > pProto->Stackable)
        num_to_add = pProto->Stackable;

    // init items_count to 1, since 1 item will be created regardless of specialization
    int items_count=1;
    // the chance to create additional items
    float additionalCreateChance=0.0f;
    // the maximum number of created additional items
    uint8 additionalMaxNum=0;

    // really will be created more items
    num_to_add *= items_count;

    // can the player store the new item?
    ItemPosCountVec dest;
    uint32 no_space = 0;
    uint8 msg = player->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, newitemid, num_to_add, &no_space );
    if( msg != EQUIP_ERR_OK )
    {
        // convert to possible store amount
        if( msg == EQUIP_ERR_INVENTORY_FULL || msg == EQUIP_ERR_CANT_CARRY_MORE_OF_THIS )
            num_to_add -= no_space;
        else
        {
            // if not created by another reason from full inventory or unique items amount limitation
            player->SendEquipError( msg, NULL, NULL );
            return;
        }
    }

    if(num_to_add)
    {
        // create the new item and store it
        Item* pItem = player->StoreNewItem( dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));

        // was it successful? return error if not
        if(!pItem)
        {
            player->SendEquipError( EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL );
            return;
        }

        // set the "Crafted by ..." property of the item
        if( pItem->GetProto()->Class != ITEM_CLASS_CONSUMABLE && pItem->GetProto()->Class != ITEM_CLASS_QUEST)
            pItem->SetUInt32Value(ITEM_FIELD_CREATOR,player->GetGUIDLow());

        // send info to the client
        if(pItem)
            player->SendNewItem(pItem, num_to_add, true, true);

        // we succeeded in creating at least one item, so a levelup is possible
        player->UpdateCraftSkill(m_spellInfo->Id);
    }       
}

void Spell::EffectCreateItem(uint32 i)
{
    DoCreateItem(i,m_spellInfo->EffectItemType[i]);
}

void Spell::EffectPersistentAA(uint32 i)
{
    float radius = GetSpellRadius(m_spellInfo,i,false);
    if(Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius);

    Unit *caster = m_caster->GetEntry() == WORLD_TRIGGER ? m_originalCaster : m_caster;
    int32 duration = GetSpellDuration(m_spellInfo);
    DynamicObject* dynObj = new DynamicObject;
    if(!dynObj->Create(objmgr.GenerateLowGuid(HIGHGUID_DYNAMICOBJECT), caster, m_spellInfo->Id, i, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, duration, radius))
    {
        delete dynObj;
        return;
    }
    dynObj->SetUInt32Value(OBJECT_FIELD_TYPE, 65);
    dynObj->SetUInt32Value(DYNAMICOBJECT_BYTES, 0x01eeeeee);
    caster->AddDynObject(dynObj);
    dynObj->GetMap()->Add(dynObj);
}

void Spell::EffectEnergize(uint32 i)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    if(m_spellInfo->EffectMiscValue[i] < 0 || m_spellInfo->EffectMiscValue[i] >= MAX_POWERS)
        return;

    // Some level depends spells
    int multiplier = 0;
    int level_diff = 0;
    switch (m_spellInfo->Id)
    {
        // Bloodrage
        case 2687:
            unitTarget->SetInCombatState(false, 10000);
            break;
        // Restore Energy
        case 9512:
            level_diff = m_caster->getLevel() - 40;
            multiplier = 2;
            break;
        // Blood Fury
        case 24571:
            level_diff = m_caster->getLevel() - 60;
            multiplier = 10;
            break;
        // Burst of Energy
        case 24532:
            level_diff = m_caster->getLevel() - 60;
            multiplier = 4;
            break;
        default:
            break;
    }

    if (level_diff > 0)
        damage -= multiplier * level_diff;

    if(damage < 0)
        return;

    Powers power = Powers(m_spellInfo->EffectMiscValue[i]);

    if(unitTarget->GetMaxPower(power) == 0)
        return;

    unitTarget->ModifyPower(power,damage);
    m_caster->SendEnergizeSpellLog(unitTarget, m_spellInfo->Id, damage, power);
}

void Spell::EffectEnergisePct(uint32 i)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    if(m_spellInfo->EffectMiscValue[i] < 0 || m_spellInfo->EffectMiscValue[i] >= MAX_POWERS)
        return;

    Powers power = Powers(m_spellInfo->EffectMiscValue[i]);

    uint32 maxPower = unitTarget->GetMaxPower(power);
    if(maxPower == 0)
        return;

    uint32 gain = damage * maxPower / 100;
    unitTarget->ModifyPower(power, gain);
    m_caster->SendEnergizeSpellLog(unitTarget, m_spellInfo->Id, damage, power);
}

void Spell::SendLoot(uint64 guid, LootType loottype)
{
    Player* player = (Player*)m_caster;
    if (!player)
        return;

    if (gameObjTarget)
    {
        if (Script->GOHello(player, gameObjTarget))
            return;

        switch (gameObjTarget->GetGoType())
        {
            case GAMEOBJECT_TYPE_DOOR:
            case GAMEOBJECT_TYPE_BUTTON:
                gameObjTarget->UseDoorOrButton();
                sWorld.ScriptsStart(sGameObjectScripts, gameObjTarget->GetDBTableGUIDLow(), player, gameObjTarget);
                return;

            case GAMEOBJECT_TYPE_QUESTGIVER:
                // start or end quest
                player->PrepareQuestMenu(guid);
                player->SendPreparedQuest(guid);
                return;

            case GAMEOBJECT_TYPE_SPELL_FOCUS:
                // triggering linked GO
                if(uint32 trapEntry = gameObjTarget->GetGOInfo()->spellFocus.linkedTrapId)
                    gameObjTarget->TriggeringLinkedGameObject(trapEntry,m_caster);
                return;

            case GAMEOBJECT_TYPE_GOOBER:
                // goober_scripts can be triggered if the player don't have the quest
                if (gameObjTarget->GetGOInfo()->goober.eventId)
                {
                    sLog.outDebug("Goober ScriptStart id %u for GO %u", gameObjTarget->GetGOInfo()->goober.eventId,gameObjTarget->GetDBTableGUIDLow());
                    sWorld.ScriptsStart(sEventScripts, gameObjTarget->GetGOInfo()->goober.eventId, player, gameObjTarget);
                }

                // cast goober spell
                if (gameObjTarget->GetGOInfo()->goober.questId)
                    ///Quest require to be active for GO using
                    if(player->GetQuestStatus(gameObjTarget->GetGOInfo()->goober.questId) != QUEST_STATUS_INCOMPLETE)
                        return;

                Script->GOHello(player, gameObjTarget);
                sWorld.ScriptsStart(sGameObjectScripts, gameObjTarget->GetDBTableGUIDLow(), player, gameObjTarget);

                gameObjTarget->AddUniqueUse(player);
                gameObjTarget->SetLootState(GO_JUST_DEACTIVATED);

                //TODO? Objective counting called without spell check but with quest objective check
                // if send spell id then this line will duplicate to spell casting call (double counting)
                // So we or have this line and not required in quest_template have reqSpellIdN
                // or must remove this line and required in DB have data in quest_template have reqSpellIdN for all quest using cases.
                player->CastedCreatureOrGO(gameObjTarget->GetEntry(), gameObjTarget->GetGUID(), 0);

                // triggering linked GO
                if(uint32 trapEntry = gameObjTarget->GetGOInfo()->goober.linkedTrapId)
                    gameObjTarget->TriggeringLinkedGameObject(trapEntry,m_caster);

                return;

            case GAMEOBJECT_TYPE_CHEST:
                // TODO: possible must be moved to loot release (in different from linked triggering)
                if (gameObjTarget->GetGOInfo()->chest.eventId)
                {
                    sLog.outDebug("Chest ScriptStart id %u for GO %u", gameObjTarget->GetGOInfo()->chest.eventId,gameObjTarget->GetDBTableGUIDLow());
                    sWorld.ScriptsStart(sEventScripts, gameObjTarget->GetGOInfo()->chest.eventId, player, gameObjTarget);
                }

                // triggering linked GO
                if(uint32 trapEntry = gameObjTarget->GetGOInfo()->chest.linkedTrapId)
                    gameObjTarget->TriggeringLinkedGameObject(trapEntry,m_caster);

                // Don't return, let loots been taken
        }
    }

    // Send loot
    player->SendLoot(guid, loottype);
}

void Spell::EffectOpenLock(uint32 /*i*/)
{
    if(!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
    {
        sLog.outDebug( "WORLD: Open Lock - No Player Caster!");
        return;
    }

    Player* player = (Player*)m_caster;

    LootType loottype = LOOT_CORPSE;
    uint32 lockId = 0;
    uint64 guid = 0;

    // Get lockId
    if(gameObjTarget)
    {
        GameObjectInfo const* goInfo = gameObjTarget->GetGOInfo();
        // Arathi Basin banner opening !
        if( goInfo->type == GAMEOBJECT_TYPE_BUTTON && goInfo->button.noDamageImmune ||
            goInfo->type == GAMEOBJECT_TYPE_GOOBER && goInfo->goober.losOK )
        {
            //isAllowUseBattleGroundObject() already called in CheckCast()
            // in battleground check
            if(BattleGround *bg = player->GetBattleGround())
            {
                // check if it's correct bg
                if(bg->GetTypeID() == BATTLEGROUND_AB || bg->GetTypeID() == BATTLEGROUND_AV)
                    bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        // handle outdoor pvp object opening, return true if go was registered for handling
        // these objects must have been spawned by outdoorpvp!
        else if(gameObjTarget->GetGOInfo()->type == GAMEOBJECT_TYPE_GOOBER && sOutdoorPvPMgr.HandleOpenGo(player, gameObjTarget->GetGUID()))
            return;
        lockId = gameObjTarget->GetLockId();
        guid = gameObjTarget->GetGUID();
    }
    else if(itemTarget)
    {
        lockId = itemTarget->GetProto()->LockID;
        guid = itemTarget->GetGUID();
    }
    else
    {
        sLog.outDebug( "WORLD: Open Lock - No GameObject/Item Target!");
        return;
    }

    if(!lockId)                                             // possible case for GO and maybe for items.
    {
        SendLoot(guid, loottype);
        return;
    }

    // Get LockInfo
    LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);

    if (!lockInfo)
    {
        sLog.outError( "Spell::EffectOpenLock: %s [guid = %u] has an unknown lockId: %u!",
            (gameObjTarget ? "gameobject" : "item"), GUID_LOPART(guid), lockId);
        SendCastResult(SPELL_FAILED_BAD_TARGETS);
        return;
    }

    // check key
    for(int i = 0; i < 5; ++i)
    {
        // type==1 This means lockInfo->key[i] is an item
        if(lockInfo->keytype[i]==LOCK_KEY_ITEM && lockInfo->key[i] && m_CastItem && m_CastItem->GetEntry()==lockInfo->key[i])
        {
            SendLoot(guid, loottype);
            return;
        }
    }

    uint32 SkillId = 0;
    // Check and skill-up skill
    if( m_spellInfo->Effect[1] == SPELL_EFFECT_SKILL )
        SkillId = m_spellInfo->EffectMiscValue[1];
                                                            // pickpocketing spells
    else if( m_spellInfo->EffectMiscValue[0] == LOCKTYPE_PICKLOCK )
        SkillId = SKILL_LOCKPICKING;

    // skill bonus provided by casting spell (mostly item spells)
    uint32 spellSkillBonus = uint32(damage/*m_currentBasePoints[0]+1*/);

    uint32 reqSkillValue = lockInfo->requiredminingskill;

    if(lockInfo->requiredlockskill)                         // required pick lock skill applying
    {
        if(SkillId != SKILL_LOCKPICKING)                    // wrong skill (cheating?)
        {
            SendCastResult(SPELL_FAILED_FIZZLE);
            return;
        }

        reqSkillValue = lockInfo->requiredlockskill;
    }
    else if(SkillId == SKILL_LOCKPICKING)                   // apply picklock skill to wrong target
    {
        SendCastResult(SPELL_FAILED_BAD_TARGETS);
        return;
    }

    if ( SkillId )
    {
        loottype = LOOT_SKINNING;
        if ( player->GetSkillValue(SkillId) + spellSkillBonus < reqSkillValue )
        {
            SendCastResult(SPELL_FAILED_LOW_CASTLEVEL);
            return;
        }

        // update skill if really known
        uint32 SkillValue = player->GetPureSkillValue(SkillId);
        if(SkillValue)                                      // non only item base skill
        {
            if(gameObjTarget)
            {
                // Allow one skill-up until respawned
                if ( !gameObjTarget->IsInSkillupList( player->GetGUIDLow() ) &&
                    player->UpdateGatherSkill(SkillId, SkillValue, reqSkillValue) )
                    gameObjTarget->AddToSkillupList( player->GetGUIDLow() );
            }
            else if(itemTarget)
            {
                // Do one skill-up
                uint32 SkillValue = player->GetPureSkillValue(SkillId);
                player->UpdateGatherSkill(SkillId, SkillValue, reqSkillValue);
            }
        }
    }

    SendLoot(guid, loottype);
}

void Spell::EffectSummonChangeItem(uint32 i)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)m_caster;

    // applied only to using item
    if(!m_CastItem)
        return;

    // ... only to item in own inventory/bank/equip_slot
    if(m_CastItem->GetOwnerGUID()!=player->GetGUID())
        return;

    uint32 newitemid = m_spellInfo->EffectItemType[i];
    if(!newitemid)
        return;

    uint16 pos = m_CastItem->GetPos();

    Item *pNewItem = Item::CreateItem( newitemid, 1, player);
    if( !pNewItem )
        return;

    for(uint8 i= PERM_ENCHANTMENT_SLOT; i<=TEMP_ENCHANTMENT_SLOT; ++i)
    {
        if(m_CastItem->GetEnchantmentId(EnchantmentSlot(i)))
            pNewItem->SetEnchantment(EnchantmentSlot(i), m_CastItem->GetEnchantmentId(EnchantmentSlot(i)), m_CastItem->GetEnchantmentDuration(EnchantmentSlot(i)), m_CastItem->GetEnchantmentCharges(EnchantmentSlot(i)));
    }

    if(m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) < m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY))
    {
        double loosePercent = 1 - m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) / double(m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY));
        player->DurabilityLoss(pNewItem, loosePercent);
    }

    if( player->IsInventoryPos( pos ) )
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreItem( m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true );
        if( msg == EQUIP_ERR_OK )
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(),true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if(m_CastItem==m_targets.getItemTarget())
                m_targets.setItemTarget(NULL);

            m_CastItem = NULL;

            player->StoreItem( dest, pNewItem, true);
            return;
        }
    }
    else if( player->IsBankPos ( pos ) )
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanBankItem( m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true );
        if( msg == EQUIP_ERR_OK )
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(),true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if(m_CastItem==m_targets.getItemTarget())
                m_targets.setItemTarget(NULL);

            m_CastItem = NULL;

            player->BankItem( dest, pNewItem, true);
            return;
        }
    }
    else if( player->IsEquipmentPos ( pos ) )
    {
        uint16 dest;
        uint8 msg = player->CanEquipItem( m_CastItem->GetSlot(), dest, pNewItem, true );
        if( msg == EQUIP_ERR_OK )
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(),true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if(m_CastItem==m_targets.getItemTarget())
                m_targets.setItemTarget(NULL);

            m_CastItem = NULL;

            player->EquipItem( dest, pNewItem, true);
            player->AutoUnequipOffhandIfNeed();
            return;
        }
    }

    // fail
    delete pNewItem;
}

void Spell::EffectOpenSecretSafe(uint32 i)
{
    EffectOpenLock(i);                                      //no difference for now
}

void Spell::EffectProficiency(uint32 /*i*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player *p_target = (Player*)unitTarget;

    uint32 subClassMask = m_spellInfo->EquippedItemSubClassMask;
    if(m_spellInfo->EquippedItemClass == 2 && !(p_target->GetWeaponProficiency() & subClassMask))
    {
        p_target->AddWeaponProficiency(subClassMask);
        p_target->SendProficiency(uint8(0x02),p_target->GetWeaponProficiency());
    }
    if(m_spellInfo->EquippedItemClass == 4 && !(p_target->GetArmorProficiency() & subClassMask))
    {
        p_target->AddArmorProficiency(subClassMask);
        p_target->SendProficiency(uint8(0x04),p_target->GetArmorProficiency());
    }
}

void Spell::EffectApplyAreaAura(uint32 i)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    AreaAura* Aur = new AreaAura(m_spellInfo, i, &damage, unitTarget, m_caster, m_CastItem);
    unitTarget->AddAura(Aur);
}

void Spell::EffectSummonType(uint32 i)
{
    /* no EffectMiscValueB for 1.12
    switch(m_spellInfo->EffectMiscValueB[i])
    {
        case SUMMON_TYPE_GUARDIAN:
            EffectSummonGuardian(i);
            break;
        case SUMMON_TYPE_POSESSED:
        case SUMMON_TYPE_POSESSED2:
        case SUMMON_TYPE_POSESSED3:
            EffectSummonPossessed(i);
            break;
        case SUMMON_TYPE_WILD:
            EffectSummonWild(i);
            break;
        case SUMMON_TYPE_DEMON:
            EffectSummonDemon(i);
            break;
        case SUMMON_TYPE_SUMMON:
            EffectSummon(i);
            break;
        case SUMMON_TYPE_CRITTER:
        case SUMMON_TYPE_CRITTER2:
        case SUMMON_TYPE_CRITTER3:
            EffectSummonCritter(i);
            break;
        case SUMMON_TYPE_TOTEM_SLOT1:
        case SUMMON_TYPE_TOTEM_SLOT2:
        case SUMMON_TYPE_TOTEM_SLOT3:
        case SUMMON_TYPE_TOTEM_SLOT4:
        case SUMMON_TYPE_TOTEM:
            EffectSummonTotem(i);
            break;
        case SUMMON_TYPE_UNKNOWN1:
        case SUMMON_TYPE_UNKNOWN3:
        case SUMMON_TYPE_UNKNOWN4:
        case SUMMON_TYPE_UNKNOWN5:
            break;
        default:
            sLog.outError("EffectSummonType: Unhandled summon type %u", m_spellInfo->EffectMiscValueB[i]);
            break;
    }*/
}

void Spell::EffectSummon(uint32 i)
{
    uint32 pet_entry = m_spellInfo->EffectMiscValue[i];
    if(!pet_entry)
        return;

    if(!m_originalCaster || m_originalCaster->GetTypeId() != TYPEID_PLAYER)
    {
        EffectSummonWild(i);
        return;
    }

    Player *owner = (Player*)m_originalCaster;

    if(owner->GetPetGUID())
        return;

    // Summon in dest location
    float x,y,z;
    if(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
    }
    else
        m_caster->GetClosePoint(x,y,z,owner->GetObjectSize());

    Pet *spawnCreature = owner->SummonPet(pet_entry, x, y, z, m_caster->GetOrientation(), SUMMON_PET, GetSpellDuration(m_spellInfo));
    if(!spawnCreature)
        return;

    spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, 0);
    spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
    spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    std::string name = owner->GetName();
    name.append(petTypeSuffix[spawnCreature->getPetType()]);
    spawnCreature->SetName( name );

    spawnCreature->SetReactState( REACT_DEFENSIVE );
}

void Spell::EffectLearnSpell(uint32 i)
{
    if(!unitTarget)
        return;

    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if(m_caster->GetTypeId() == TYPEID_PLAYER)
            EffectLearnPetSpell(i);

        return;
    }

    Player *player = (Player*)unitTarget;

    uint32 spellToLearn = (m_spellInfo->Id==SPELL_ID_GENERIC_LEARN) ? damage : m_spellInfo->EffectTriggerSpell[i];
    player->learnSpell(spellToLearn);

    sLog.outDebug( "Spell: Player %u have learned spell %u from NpcGUID=%u", player->GetGUIDLow(), spellToLearn, m_caster->GetGUIDLow() );
}

void Spell::EffectDispel(uint32 i)
{
    if(!unitTarget)
        return;

    // Fill possible dispel list
    std::vector <Aura *> dispel_list;

    // Create dispel mask by dispel type
    uint32 dispel_type = m_spellInfo->EffectMiscValue[i];
    uint32 dispelMask  = GetDispellMask( DispelType(dispel_type) );
    Unit::AuraMap const& auras = unitTarget->GetAuras();
    for(Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura *aur = (*itr).second;
        if (aur && (1<<aur->GetSpellProto()->Dispel) & dispelMask)
        {
            if(aur->GetSpellProto()->Dispel == DISPEL_MAGIC)
            {
                bool positive = true;
                if (!aur->IsPositive())
                    positive = false;
                else
                    positive = (aur->GetSpellProto()->Attributes & SPELL_ATTR_NEGATIVE)==0;

                // do not remove positive auras if friendly target
                //               negative auras if non-friendly target
                if(positive == unitTarget->IsFriendlyTo(m_caster))
                    continue;
            }
            // Add every aura stack to dispel list
            for(uint32 stack_amount = 0; stack_amount < aur->GetStackAmount(); ++stack_amount)
                dispel_list.push_back(aur);
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!dispel_list.empty())
    {
        std::list < std::pair<uint32,uint64> > success_list;// (spell_id,casterGuid)
        std::list < uint32 > fail_list;                     // spell_id
        int32 list_size = dispel_list.size();
        // dispel N = damage buffs (or while exist buffs for dispel)
        for (int32 count=0; count < damage && list_size > 0; ++count)
        {
            // Random select buff for dispel
            Aura *aur = dispel_list[urand(0, list_size-1)];

            SpellEntry const* spellInfo = aur->GetSpellProto();
            // Base dispel chance
            // TODO: possible chance depend from spell level??
            int32 miss_chance = 0;
            // Apply dispel mod from aura caster
            if (Unit *caster = aur->GetCaster())
            {
                if ( Player* modOwner = caster->GetSpellModOwner() )
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_RESIST_DISPEL_CHANCE, miss_chance, this);
            }
            // Try dispel
            if (roll_chance_i(miss_chance))
                fail_list.push_back(aur->GetId());
            else
                success_list.push_back(std::pair<uint32,uint64>(aur->GetId(),aur->GetCasterGUID()));
            // Remove buff from list for prevent doubles
            for (std::vector<Aura *>::iterator j = dispel_list.begin(); j != dispel_list.end(); )
            {
                Aura *dispelled = *j;
                if (dispelled->GetId() == aur->GetId() && dispelled->GetCasterGUID() == aur->GetCasterGUID())
                {
                    j = dispel_list.erase(j);
                    --list_size;
                    break;
                }
                else
                    ++j;
            }
        }
        // Send success log and really remove auras
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLDISPELLOG, 8+8+4+1+4+count*5);
            data.append(unitTarget->GetPackGUID());         // Victim GUID
            data.append(m_caster->GetPackGUID());           // Caster GUID
            data << uint32(m_spellInfo->Id);                // dispel spell id
            data << uint8(0);                               // not used
            data << uint32(count);                          // count
            for (std::list<std::pair<uint32,uint64> >::iterator j = success_list.begin(); j != success_list.end(); ++j)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(j->first);
                data << uint32(spellInfo->Id);              // Spell Id
                data << uint8(0);                           // 0 - dispelled !=0 cleansed
                if(spellInfo->StackAmount!= 0)
                {
                    //Why are Aura's Removed by EffIndex? Auras should be removed as a whole.....
                    unitTarget->RemoveSingleAuraFromStackByDispel(spellInfo->Id);
                }
                else
                unitTarget->RemoveAurasDueToSpellByDispel(spellInfo->Id, j->second, m_caster);
             }
            m_caster->SendMessageToSet(&data, true);

            // On succes dispel
            // Devour Magic
            if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->Category == 12)
            {
                uint32 heal_spell = 0;
                switch (m_spellInfo->Id)
                {
                    case 19505: heal_spell = 19658; break;
                    case 19731: heal_spell = 19732; break;
                    case 19734: heal_spell = 19733; break;
                    case 19736: heal_spell = 19735; break;
                    default:
                        sLog.outDebug("Spell for Devour Magic %d not handled in Spell::EffectDispel", m_spellInfo->Id);
                        break;
                }
                if (heal_spell)
                    m_caster->CastSpell(m_caster, heal_spell, true);
            }
             // Paladin Tier 3 Set 8 Piece Bonus - Heal on Cleanse
            else if ((m_spellInfo->Id == 4987) && m_caster->HasAura(28787,0))
                 m_caster->CastSpell(unitTarget, 28788, true);
        }
        // Send fail log to client
        if (!fail_list.empty())
        {
            // Failed to dispell
            WorldPacket data(SMSG_DISPEL_FAILED, 8+8+4+4*fail_list.size());
            data << uint64(m_caster->GetGUID());            // Caster GUID
            data << uint64(unitTarget->GetGUID());          // Victim GUID
            data << uint32(m_spellInfo->Id);                // dispel spell id
            for (std::list< uint32 >::iterator j = fail_list.begin(); j != fail_list.end(); ++j)
                data << uint32(*j);                         // Spell Id
            m_caster->SendMessageToSet(&data, true);
        }
    }
}

void Spell::EffectDualWield(uint32 /*i*/)
{
    unitTarget->SetCanDualWield(true);
    if(unitTarget->GetTypeId() == TYPEID_UNIT)
        ((Creature*)unitTarget)->UpdateDamagePhysical(OFF_ATTACK);
}

void Spell::EffectPull(uint32 /*i*/)
{
    // TODO: create a proper pull towards distract spell center for distract
    sLog.outDebug("WORLD: Spell Effect DUMMY");
}

void Spell::EffectDistract(uint32 /*i*/)
{
    // Check for possible target
    if (!unitTarget || unitTarget->isInCombat())
        return;

    // target must be OK to do this
    if( unitTarget->hasUnitState(UNIT_STAT_CONFUSED | UNIT_STAT_STUNNED | UNIT_STAT_FLEEING ) )
        return;

    float angle = unitTarget->GetAngle(m_targets.m_destX, m_targets.m_destY);

    if ( unitTarget->GetTypeId() == TYPEID_PLAYER )
    {
        // For players just turn them
        WorldPacket data;
        ((Player*)unitTarget)->BuildTeleportAckMsg(&data, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), angle);
        ((Player*)unitTarget)->GetSession()->SendPacket( &data );
        ((Player*)unitTarget)->SetPosition(unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), angle, false);
    }
    else
    {
        // Set creature Distracted, Stop it, And turn it
        unitTarget->SetOrientation(angle);
        unitTarget->StopMoving();
        unitTarget->GetMotionMaster()->MoveDistract(damage*1000);
    }
}

void Spell::EffectPickPocket(uint32 /*i*/)
{
    if( m_caster->GetTypeId() != TYPEID_PLAYER )
        return;

    // victim must be creature and attackable
    if( !unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->IsFriendlyTo(unitTarget) )
        return;

    // victim have to be alive and humanoid or undead
    if( unitTarget->isAlive() && (unitTarget->GetCreatureTypeMask() &CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) != 0)
    {
        int32 chance = 10 + int32(m_caster->getLevel()) - int32(unitTarget->getLevel());

        if (chance > irand(0, 19))
        {
            // Stealing successful
            //sLog.outDebug("Sending loot from pickpocket");
            ((Player*)m_caster)->SendLoot(unitTarget->GetGUID(),LOOT_PICKPOCKETING);
        }
        else
        {
            // Reveal action + get attack
            m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TALK);
            if (((Creature*)unitTarget)->IsAIEnabled)
                ((Creature*)unitTarget)->AI()->AttackStart(m_caster);
        }
    }
}

void Spell::EffectAddFarsight(uint32 i)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    float radius = GetSpellRadius(m_spellInfo,i,false);
    int32 duration = GetSpellDuration(m_spellInfo);
    DynamicObject* dynObj = new DynamicObject;
    if(!dynObj->Create(objmgr.GenerateLowGuid(HIGHGUID_DYNAMICOBJECT), m_caster, m_spellInfo->Id, 4, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, duration, radius))
    {
        delete dynObj;
        return;
    }
    dynObj->SetUInt32Value(OBJECT_FIELD_TYPE, 65);
    dynObj->SetUInt32Value(DYNAMICOBJECT_BYTES, 0x80000002);
    m_caster->AddDynObject(dynObj);

    dynObj->setActive(true);    //must before add to map to be put in world container
    dynObj->GetMap()->Add(dynObj); //grid will also be loaded

    // Need to update visibility of object for client to accept farsight guid
    ((Player*)m_caster)->UpdateVisibilityOf(dynObj);
    ((Player*)m_caster)->SetFarsightTarget(dynObj);
}

void Spell::EffectSummonWild(uint32 i)
{
    uint32 creature_entry = m_spellInfo->EffectMiscValue[i];
    if(!creature_entry)
        return;

    uint32 level = m_caster->getLevel();

    // level of creature summoned using engineering item based at engineering skill level
    if(m_caster->GetTypeId()==TYPEID_PLAYER && m_CastItem)
    {
        ItemPrototype const *proto = m_CastItem->GetProto();
        if(proto && proto->RequiredSkill == SKILL_ENGINERING)
        {
            uint16 skill202 = ((Player*)m_caster)->GetSkillValue(SKILL_ENGINERING);
            if(skill202)
            {
                level = skill202/5;
            }
        }
    }

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(m_spellInfo,i,false);

    int32 amount = damage > 0 ? damage : 1;

    for(int32 count = 0; count < amount; ++count)
    {
        float px, py, pz;
        // If dest location if present
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon 1 unit in dest location
            if (count == 0)
            {
                px = m_targets.m_destX;
                py = m_targets.m_destY;
                pz = m_targets.m_destZ;
            }
            // Summon in random point all other units if location present
            else
                m_caster->GetRandomPoint(center_x,center_y,center_z,radius,px,py,pz);
        }
        // Summon if dest location not present near caster
        else
        {
            if (radius > 0.0f)
            {
                // not using bounding radius of caster here
                m_caster->GetClosePoint(px, py, pz, 0.0f, radius);
            }
            else
            {
                // EffectRadiusIndex 0 or 36
                px = m_caster->GetPositionX();
                py = m_caster->GetPositionY();
                pz = m_caster->GetPositionZ();
            }
        }

        int32 duration = GetSpellDuration(m_spellInfo);

        TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;

        if (Creature *summon = m_caster->SummonCreature(creature_entry, px, py, pz, m_caster->GetOrientation(), summonType, duration))
        {
            summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
            // Exception for Alterac Shredder. The second effect of the spell (possess) can't target the shredder
            // because it is not summoned at target selection phase.
            switch (m_spellInfo->Id)
            {
                // Both sides
                case 21544:
                case 21565:
                    summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->EffectTriggerSpell[1]);
                    summon->SetUInt64Value(UNIT_FIELD_CREATEDBY, m_caster->GetGUID());
                    *m_selfContainer = NULL;
                    m_caster->CastSpell(summon, m_spellInfo->EffectTriggerSpell[1], true);
                    break;
                // Target Dummy
                case 4071:
                case 4072:
                case 19805:
                    summon->SetUInt64Value(UNIT_FIELD_CREATEDBY, m_caster->GetGUID());
                    summon->SetLootRecipient(m_caster);
                    break;
            }
        }
    }
}

void Spell::EffectSummonGuardian(uint32 i)
{
    uint32 pet_entry = m_spellInfo->EffectMiscValue[i];
    if(!pet_entry)
        return;

    // Jewelery statue case (totem like)
    if(m_spellInfo->SpellIconID==2056)
    {
        EffectSummonTotem(i);
        return;
    }

    Player *caster = NULL;
    if(m_originalCaster)
    {
        if(m_originalCaster->GetTypeId() == TYPEID_PLAYER)
            caster = (Player*)m_originalCaster;
        else if(((Creature*)m_originalCaster)->isTotem())
            caster = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    }

    if(!caster)
    {
        EffectSummonWild(i);
        return;
    }

    // set timer for unsummon
    int32 duration = GetSpellDuration(m_spellInfo);

    // Search old Guardian only for players (if casted spell not have duration or cooldown)
    // FIXME: some guardians have control spell applied and controlled by player and anyway player can't summon in this time
    //        so this code hack in fact
    if(duration <= 0 || GetSpellRecoveryTime(m_spellInfo)==0)
        if(caster->HasGuardianWithEntry(pet_entry))
            return;                                         // find old guardian, ignore summon

    // in another case summon new
    uint32 level = caster->getLevel();

    // level of pet summoned using engineering item based at engineering skill level
    if(m_CastItem)
    {
        ItemPrototype const *proto = m_CastItem->GetProto();
        if(proto && proto->RequiredSkill == SKILL_ENGINERING)
        {
            uint16 skill202 = caster->GetSkillValue(SKILL_ENGINERING);
            if(skill202)
            {
                level = skill202/5;
            }
        }
    }

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(m_spellInfo,i,false);

    int32 amount = damage > 0 ? damage : 1;

    for(int32 count = 0; count < amount; ++count)
    {
        float px, py, pz;
        // If dest location if present
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon 1 unit in dest location
            if (count == 0)
            {
                px = m_targets.m_destX;
                py = m_targets.m_destY;
                pz = m_targets.m_destZ;
            }
            // Summon in random point all other units if location present
            else
                m_caster->GetRandomPoint(center_x,center_y,center_z,radius,px,py,pz);
        }
        // Summon if dest location not present near caster
        else
            m_caster->GetClosePoint(px,py,pz,m_caster->GetObjectSize());

        Pet *spawnCreature = caster->SummonPet(m_spellInfo->EffectMiscValue[i], px, py, pz, m_caster->GetOrientation(), GUARDIAN_PET, duration);
        if(!spawnCreature)
            return;

        spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP,0);
        spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
    }
}

void Spell::EffectSummonPossessed(uint32 i)
{
    uint32 entry = m_spellInfo->EffectMiscValue[i];
    if(!entry)
        return;

    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 level = m_caster->getLevel();

    float x, y, z;
    m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    int32 duration = GetSpellDuration(m_spellInfo);

    Pet* pet = ((Player*)m_caster)->SummonPet(entry, x, y, z, m_caster->GetOrientation(), POSSESSED_PET, duration);
    if(!pet)
        return;

    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
    pet->SetCharmedOrPossessedBy(m_caster, true);
}

void Spell::EffectTeleUnitsFaceCaster(uint32 i)
{
    if(!unitTarget)
        return;

    if(unitTarget->isInFlight())
        return;

    uint32 mapid = m_caster->GetMapId();
    float dis = GetSpellRadius(m_spellInfo,i,false);

    float fx,fy,fz;
    m_caster->GetClosePoint(fx,fy,fz,unitTarget->GetObjectSize(),dis);

    if(unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->TeleportTo(mapid, fx, fy, fz, -m_caster->GetOrientation(), TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget==m_caster ? TELE_TO_SPELL : 0));
    else
        m_caster->GetMap()->CreatureRelocation((Creature*)m_caster, fx, fy, fz, -m_caster->GetOrientation());
}

void Spell::EffectLearnSkill(uint32 i)
{
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(damage < 0)
        return;

    uint32 skillid =  m_spellInfo->EffectMiscValue[i];
    uint16 skillval = ((Player*)unitTarget)->GetPureSkillValue(skillid);
    ((Player*)unitTarget)->SetSkill(skillid, skillval?skillval:1, damage*75);
}

void Spell::EffectAddHonor(uint32 /*i*/)
{
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    sLog.outDebug("SpellEffect::AddHonor called for spell_id %u , that rewards %d honor points to player: %u", m_spellInfo->Id, damage, ((Player*)unitTarget)->GetGUIDLow());

    // TODO: find formula for honor reward based on player's level!

    // now fixed only for level 70 players:
   /*[TZERO] to replace ? 
    if (((Player*)unitTarget)->getLevel() == 70)
        ((Player*)unitTarget)->RewardHonor(NULL, 1, damage); */
}

void Spell::EffectTradeSkill(uint32 /*i*/)
{
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    // uint32 skillid =  m_spellInfo->EffectMiscValue[i];
    // uint16 skillmax = ((Player*)unitTarget)->(skillid);
    // ((Player*)unitTarget)->SetSkill(skillid,skillval?skillval:1,skillmax+75);
}

void Spell::EffectEnchantItemPerm(uint32 i)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    if (m_spellInfo->EffectMiscValue[i])
    {
        uint32 enchant_id = m_spellInfo->EffectMiscValue[i];

        SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if(!pEnchant)
            return;

        // item can be in trade slot and have owner diff. from caster
        Player* item_owner = itemTarget->GetOwner();
        if(!item_owner)
            return;

        if(item_owner!=p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_GM_LOG_TRADE) )
        {
            sLog.outCommand(p_caster->GetSession()->GetAccountId(),"GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
                p_caster->GetName(),p_caster->GetSession()->GetAccountId(),
                itemTarget->GetProto()->Name1,itemTarget->GetEntry(),
                item_owner->GetName(),item_owner->GetSession()->GetAccountId());
        }

        // remove old enchanting before applying new if equipped
        item_owner->ApplyEnchantment(itemTarget,PERM_ENCHANTMENT_SLOT,false);

        itemTarget->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0);

        // add new enchanting if equipped
        item_owner->ApplyEnchantment(itemTarget,PERM_ENCHANTMENT_SLOT,true);

       // update trade window for show enchantment for caster in trade window
       if (m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
            p_caster->GetSession()->SendUpdateTrade();
    }
}

uint32 GetPoisonCharges(uint32 spellId)
{
    switch (spellId)
    {
        // Instant Poison
        case 8679:
            return 40;
        case 8686:
            return 55;
        case 8688:
            return 70;
        case 11338:
            return 85;
        case 11339:
            return 100;
        case 11340:
            return 115;
        // Mind-numbing Poison
        case 5761:
            return 50;
        case 8693:
            return 75;
        case 11399:
            return 100;
        // Deadly Poison
        case 2823:
            return 60;
        case 2824:
            return 75;
        case 11355:
            return 90;
        case 11356:
            return 105;
        case 25351:
            return 120;
        // Wound Poison
        case 13219:
            return 60;
        case 13225:
            return 75;
        case 13226:
            return 90;
        case 13227:
            return 105;
            // Crippling poison
            //case 3408:  return 0;
            //case 11202: return 0;
    }
    return 0;
}

void Spell::EffectEnchantItemTmp(uint32 i)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;

    if(!itemTarget)
        return;

    uint32 enchant_id = m_spellInfo->EffectMiscValue[i];

    // Shaman Rockbiter Weapon
    if(i==0 && m_spellInfo->Effect[1]==SPELL_EFFECT_DUMMY)
    {
        int32 enchnting_damage = CalculateDamage(1, NULL);//+1;

        // enchanting id selected by calculated damage-per-sec stored in Effect[1] base value
        // with already applied percent bonus from Elemental Weapons talent
        // Note: damage calculated (correctly) with rounding int32(float(v)) but
        // RW enchantments applied damage int32(float(v)+0.5), this create  0..1 difference sometime
        switch(enchnting_damage)
        {
            // Rank 1
            case  2: enchant_id =   29; break;              //  0% [ 7% ==  2, 14% == 2, 20% == 2]
            // Rank 2
            case  4: enchant_id =    6; break;              //  0% [ 7% ==  4, 14% == 4]
            case  5: enchant_id = 3025; break;              // 20%
            // Rank 3
            case  6: enchant_id =    1; break;              //  0% [ 7% ==  6, 14% == 6]
            case  7: enchant_id = 3027; break;              // 20%
            // Rank 4
            case  9: enchant_id = 3032; break;              //  0% [ 7% ==  6]
            case 10: enchant_id =  503; break;              // 14%
            case 11: enchant_id = 3031; break;              // 20%
            // Rank 5
            case 15: enchant_id = 3035; break;              // 0%
            case 16: enchant_id = 1663; break;              // 7%
            case 17: enchant_id = 3033; break;              // 14%
            case 18: enchant_id = 3034; break;              // 20%
            // Rank 6
            case 28: enchant_id = 3038; break;              // 0%
            case 29: enchant_id =  683; break;              // 7%
            case 31: enchant_id = 3036; break;              // 14%
            case 33: enchant_id = 3037; break;              // 20%
            // Rank 7
            case 40: enchant_id = 3041; break;              // 0%
            case 42: enchant_id = 1664; break;              // 7%
            case 45: enchant_id = 3039; break;              // 14%
            case 48: enchant_id = 3040; break;              // 20%
            // Rank 8
            case 49: enchant_id = 3044; break;              // 0%
            case 52: enchant_id = 2632; break;              // 7%
            case 55: enchant_id = 3042; break;              // 14%
            case 58: enchant_id = 3043; break;              // 20%
            // Rank 9
            case 62: enchant_id = 2633; break;              // 0%
            case 66: enchant_id = 3018; break;              // 7%
            case 70: enchant_id = 3019; break;              // 14%
            case 74: enchant_id = 3020; break;              // 20%
            default:
                sLog.outError("Spell::EffectEnchantItemTmp: Damage %u not handled in S'RW",enchnting_damage);
                return;
        }
    }

    if (!enchant_id)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have 0 as enchanting id",m_spellInfo->Id,i);
        return;
    }

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if(!pEnchant)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have not existed enchanting id %u ",m_spellInfo->Id,i,enchant_id);
        return;
    }

    // select enchantment duration
    uint32 duration;

    // other rogue family enchantments always 1 hour (some have spell damage=0, but some have wrong data in EffBasePoints)
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE)
        duration = 1800;                                    // 30 mins (Ustaag <Nostalrius> : 1h post 2.3)
    // shaman family enchantments
    else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN)
        duration = 300;                                     // 5 mins (Ustaag <Nostalrius> : 30 mn post 1.12)
    // other cases with this SpellVisual already selected
    else if (m_spellInfo->SpellVisual == 215)
        duration = 1800;                                    // 30 mins
    // some fishing pole bonuses
    else if (m_spellInfo->SpellVisual == 563)
        duration = 600;                                     // 10 mins
    // shaman Rockbiter enchantments
    else if (m_spellInfo->SpellVisual == 58)
        duration = 300;                                     // 5 mins (Ustaag <Nostalrius> : 30 mn post 1.12)
    // shaman Flametongue enchantments
    else if (m_spellInfo->SpellVisual == 290)
        duration = 300;                                     // 5 mins (Ustaag <Nostalrius> : 30 mn post 1.12)
    // shaman Frostbrand enchantments
    else if (m_spellInfo->SpellVisual == 291)
        duration = 300;                                     // 5 mins (Ustaag <Nostalrius> : 30 mn post 1.12)
    // Sharpen Blade enchantments
    else if (m_spellInfo->SpellVisual == 3324)
        duration = 1800;                                    // 30 mins (Ustaag <Nostalrius> : Sharpening Stone)
    // Enhance Blunt enchantments
    else if (m_spellInfo->SpellVisual == 3325)
        duration = 1800;                                    // 30 mins (Ustaag <Nostalrius> : Weightstone)
    // Oil enchantments
    else if (m_spellInfo->SpellVisual == 3182)
        duration = 1800;                                    // 30 mins (Ustaag <Nostalrius> : Oil)
    // default case
    else
        duration = 3600;                                    // 1 hour

    // Calcul du nombre de charges
    // Poisons :
    uint32 charges    = 0;
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE)
        charges = GetPoisonCharges(m_spellInfo->Id);

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if(!item_owner)
        return;

    if(item_owner!=p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_GM_LOG_TRADE) )
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(),"GM %s (Account: %u) enchanting(temp): %s (Entry: %d) for player: %s (Account: %u)",
            p_caster->GetName(),p_caster->GetSession()->GetAccountId(),
            itemTarget->GetProto()->Name1,itemTarget->GetEntry(),
            item_owner->GetName(),item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget,TEMP_ENCHANTMENT_SLOT,false);

    itemTarget->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, duration*1000, charges);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget,TEMP_ENCHANTMENT_SLOT,true);

    // update trade window for show enchantment for caster in trade window
    if (m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
      p_caster->GetSession()->SendUpdateTrade();
}

void Spell::EffectTameCreature(uint32 /*i*/)
{
    if(m_caster->GetPetGUID())
        return;

    if(!unitTarget)
        return;

    if(unitTarget->GetTypeId() == TYPEID_PLAYER)
        return;

    Creature* creatureTarget = (Creature*)unitTarget;

    if(creatureTarget->isPet())
        return;

    if(m_caster->getClass() != CLASS_HUNTER)
        return;

    // cast finish successfully
    //SendChannelUpdate(0);
    finish();

    Pet* pet = m_caster->CreateTamedPetFrom(creatureTarget,m_spellInfo->Id);
    if(!pet) return;

    // Nostalrius: defensive as default behaviour
    pet->SetReactState(REACT_DEFENSIVE);

    // kill original creature
    creatureTarget->ForcedDespawn();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL,creatureTarget->getLevel()-1);

    // add to world
    pet->GetMap()->Add((Creature*)pet);

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL,creatureTarget->getLevel());

    // caster have pet now
    m_caster->SetPet(pet);

    if(m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        ((Player*)m_caster)->PetSpellInitialize();
    }
}

void Spell::EffectSummonPet(uint32 i)
{
    Player *owner = NULL;
    if(m_originalCaster)
    {
        if(m_originalCaster->GetTypeId() == TYPEID_PLAYER)
            owner = (Player*)m_originalCaster;
        else if(((Creature*)m_originalCaster)->isTotem())
            owner = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    }

    if(!owner)
    {
        EffectSummonWild(i);
        return;
    }

    uint32 petentry = m_spellInfo->EffectMiscValue[i];

    Pet *OldSummon = owner->GetPet();

    // if pet requested type already exist
    if( OldSummon )
    {
        if(petentry == 0 || OldSummon->GetEntry() == petentry)
        {
            // pet in corpse state can't be summoned
            if( OldSummon->isDead() )
                return;

            OldSummon->GetMap()->Remove((Creature*)OldSummon,false);
            OldSummon->SetMapId(owner->GetMapId());

            float px, py, pz;
            owner->GetClosePoint(px, py, pz, OldSummon->GetObjectSize());

            OldSummon->Relocate(px, py, pz, OldSummon->GetOrientation());
            owner->GetMap()->Add((Creature*)OldSummon);

            if(owner->GetTypeId() == TYPEID_PLAYER && OldSummon->isControlled() )
            {
                ((Player*)owner)->PetSpellInitialize();
            }
            return;
        }

        if(owner->GetTypeId() == TYPEID_PLAYER)
            ((Player*)owner)->RemovePet(OldSummon,(OldSummon->getPetType()==HUNTER_PET ? PET_SAVE_AS_DELETED : PET_SAVE_NOT_IN_SLOT),false);
        else
            return;
    }

    float x, y, z;
    owner->GetClosePoint(x, y, z, owner->GetObjectSize());
    Pet* pet = owner->SummonPet(petentry, x, y, z, owner->GetOrientation(), SUMMON_PET, 0);
    if(!pet)
        return;
        
    if(m_caster->GetTypeId() == TYPEID_UNIT)
    {
        if ( ((Creature*)m_caster)->isTotem() )
            pet->SetReactState(REACT_AGGRESSIVE);
        else
            pet->SetReactState(REACT_DEFENSIVE);
    }
    else
        pet->SetReactState(REACT_DEFENSIVE);

    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    // this enables popup window (pet dismiss, cancel), hunter pet additional flags set later
    pet->SetUInt32Value(UNIT_FIELD_FLAGS,UNIT_FLAG_PVP_ATTACKABLE);
    pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, time(NULL));

    // generate new name for summon pet
    std::string new_name=objmgr.GeneratePetName(petentry);
    if(!new_name.empty())
        pet->SetName(new_name);
}

void Spell::EffectLearnPetSpell(uint32 i)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    Pet *pet = _player->GetPet();
    if(!pet)
        return;
    if(!pet->isAlive())
        return;

    SpellEntry const *learn_spellproto = sSpellStore.LookupEntry(m_spellInfo->EffectTriggerSpell[i]);
    if(!learn_spellproto)
        return;

    pet->SetTP(pet->m_TrainingPoints - pet->GetTPForSpell(learn_spellproto->Id));
    pet->learnSpell(learn_spellproto->Id);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    _player->PetSpellInitialize();
}

void Spell::EffectTaunt(uint32 /*i*/)
{
    // this effect use before aura Taunt apply for prevent taunt already attacking target
    // for spell as marked "non effective at already attacking target"
    if(!unitTarget || !unitTarget->CanHaveThreatList()
        || unitTarget->getVictim() == m_caster)
    {
        SendCastResult(SPELL_FAILED_DONT_REPORT);
        return;
    }

    // Also use this effect to set the taunter's threat to the taunted creature's highest value
    if(unitTarget->getThreatManager().getCurrentVictim())
    {
        float myThreat = unitTarget->getThreatManager().getThreat(m_caster);
        float itsThreat = unitTarget->getThreatManager().getCurrentVictim()->getThreat();
        if(itsThreat > myThreat)
            unitTarget->getThreatManager().addThreat(m_caster, itsThreat - myThreat);
    }

    if(((Creature*)unitTarget)->IsAIEnabled)
        ((Creature*)unitTarget)->AI()->AttackStart(m_caster);
}

void Spell::EffectWeaponDmg(uint32 i)
{
}

void Spell::SpellDamageWeaponDmg(uint32 i)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    // multiple weapon dmg effect workaround
    // execute only the last weapon damage
    // and handle all effects at once
    for (int j = 0; j < 3; j++)
    {
        switch(m_spellInfo->Effect[j])
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                if (j < i)                                  // we must calculate only at last weapon effect
                    return;
            break;
        }
    }

    // some spell specific modifiers
    //float weaponDamagePercentMod = 1.0f;                    // applied to weapon damage (and to fixed effect damage bonus if customBonusDamagePercentMod not set
    float totalDamagePercentMod  = 1.0f;                    // applied to final bonus+weapon damage
    int32 fixed_bonus = 0;
    int32 spell_bonus = 0;                                  // bonus specific for spell

    switch(m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_PALADIN:
        {
            // Seal of Command - receive benefit from Spell Damage and Healing
            if(m_spellInfo->SpellFamilyFlags & 0x00000002000000LL)
            {
                spell_bonus += int32(0.20f*m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo)));
                spell_bonus += int32(0.29f*m_caster->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(m_spellInfo), unitTarget));
            }
            break;
        }
    }

    bool normalized = false;
    float weaponDamagePercentMod = 1.0;
    for (int j = 0; j < 3; ++j)
    {
        switch(m_spellInfo->Effect[j])
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                fixed_bonus += CalculateDamage(j,unitTarget);
                break;
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                fixed_bonus += CalculateDamage(j,unitTarget);
                normalized = true;
                break;
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                weaponDamagePercentMod *= float(CalculateDamage(j,unitTarget)) / 100.0f;
            default:
                break;                                      // not weapon damage effect, just skip
        }
    }

    // apply to non-weapon bonus weapon total pct effect, weapon total flat effect included in weapon damage
    if(fixed_bonus || spell_bonus)
    {
        UnitMods unitMod;
        switch(m_attackType)
        {
            default:
            case BASE_ATTACK:   unitMod = UNIT_MOD_DAMAGE_MAINHAND; break;
            case OFF_ATTACK:    unitMod = UNIT_MOD_DAMAGE_OFFHAND;  break;
            case RANGED_ATTACK: unitMod = UNIT_MOD_DAMAGE_RANGED;   break;
        }

        float weapon_total_pct  = m_caster->GetModifierValue(unitMod, TOTAL_PCT);

        if(fixed_bonus)
            fixed_bonus = int32(fixed_bonus * weapon_total_pct);
        if(spell_bonus)
            spell_bonus = int32(spell_bonus * weapon_total_pct);
    }

    int32 weaponDamage = m_caster->CalculateDamage(m_attackType, normalized);

    // Sequence is important
    for (int j = 0; j < 3; ++j)
    {
        // We assume that a spell have at most one fixed_bonus
        // and at most one weaponDamagePercentMod
        switch(m_spellInfo->Effect[j])
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                weaponDamage += fixed_bonus;
                break;
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                weaponDamage = int32(weaponDamage * weaponDamagePercentMod);
            default:
                break;                                      // not weapon damage effect, just skip
        }
    }

    // only for Seal of Command
    if(spell_bonus)
        weaponDamage += spell_bonus;

    // only for Mutilate
    if(totalDamagePercentMod != 1.0f)
        weaponDamage = int32(weaponDamage * totalDamagePercentMod);

    // prevent negative damage
    uint32 eff_damage = uint32(weaponDamage > 0 ? weaponDamage : 0);

    // Add melee damage bonuses (also check for negative)
    m_caster->MeleeDamageBonus(unitTarget, &eff_damage, m_attackType, m_spellInfo);
    m_damage+= eff_damage;

    // take ammo
    if(m_attackType == RANGED_ATTACK && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Item *pItem = ((Player*)m_caster)->GetWeaponForAttack( RANGED_ATTACK );

        // wands don't have ammo
        if(!pItem  || pItem->IsBroken() || pItem->GetProto()->SubClass==ITEM_SUBCLASS_WEAPON_WAND)
            return;

        if( pItem->GetProto()->InventoryType == INVTYPE_THROWN )
        {
            if(pItem->GetMaxStackCount()==1)
            {
                // decrease durability for non-stackable throw weapon
                ((Player*)m_caster)->DurabilityPointLossForEquipSlot(EQUIPMENT_SLOT_RANGED);
            }
            else
            {
                // decrease items amount for stackable throw weapon
                uint32 count = 1;
                ((Player*)m_caster)->DestroyItemCount( pItem, count, true);
            }
        }
        else if(uint32 ammo = ((Player*)m_caster)->GetUInt32Value(PLAYER_AMMO_ID))
            ((Player*)m_caster)->DestroyItemCount(ammo, 1, true);
    }
}

void Spell::EffectThreat(uint32 /*i*/)
{
    if(!unitTarget || !unitTarget->isAlive() || !m_caster->isAlive())
        return;

    if(!unitTarget->CanHaveThreatList())
        return;

    unitTarget->AddThreat(m_caster, float(damage));
}

void Spell::EffectHealMaxHealth(uint32 /*i*/)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;

    uint32 casterhealth = caster->GetMaxHealth();
    uint32 missinghealth = unitTarget->GetMaxHealth() - unitTarget->GetHealth();
    if(casterhealth > missinghealth)
    {
        unitTarget->SetHealth(unitTarget->GetMaxHealth());
        caster->SendHealSpellLog(unitTarget, m_spellInfo->Id, missinghealth, false);
    }
    else
    {
        unitTarget->SetHealth(unitTarget->GetHealth() + casterhealth);
        caster->SendHealSpellLog(unitTarget, m_spellInfo->Id, casterhealth, false);
    }
}

void Spell::EffectInterruptCast(uint32 i)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    // TODO: not all spells that used this effect apply cooldown at school spells
    // also exist case: apply cooldown to interrupted cast only and to all spells
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
    {
        if (unitTarget->m_currentSpells[i])
        {
            // Nostalrius: fix CS of instant spells (with CC Delay)
            if (i != CURRENT_CHANNELED_SPELL && !unitTarget->m_currentSpells[i]->GetCastTime())
                continue;

            // check if we can interrupt spell
            if ( (unitTarget->m_currentSpells[i]->getState() == SPELL_STATE_CASTING || (unitTarget->m_currentSpells[i]->getState() == SPELL_STATE_PREPARING && unitTarget->m_currentSpells[i]->GetCastTime() > 0.0f)) && unitTarget->m_currentSpells[i]->m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT && unitTarget->m_currentSpells[i]->m_spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE )
            {
                if(m_originalCaster)
                {
                    int32 duration = m_originalCaster->CalculateSpellDuration(m_spellInfo, i, unitTarget);
                    unitTarget->ProhibitSpellScholl(SpellSchools(unitTarget->m_currentSpells[i]->m_spellInfo->School), duration);
                }
                unitTarget->InterruptSpell(i, false);
            }
        }
    }
}

void Spell::EffectSummonObjectWild(uint32 i)
{
    uint32 gameobject_id = m_spellInfo->EffectMiscValue[i];

    GameObject* pGameObj = new GameObject;

    WorldObject* target = focusObject;
    if( !target )
        target = m_caster;

    float x,y,z;
    if(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
    }
    else
        m_caster->GetClosePoint(x,y,z,DEFAULT_WORLD_OBJECT_SIZE);

    Map *map = target->GetMap();

    if(!pGameObj->Create(objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
        x, y, z, target->GetOrientation(), 0, 0, 0, 0, 100, 1))
    {
        delete pGameObj;
        return;
    }

    int32 duration = GetSpellDuration(m_spellInfo);
    pGameObj->SetRespawnTime(duration > 0 ? duration/1000 : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    if(pGameObj->GetGoType() != GAMEOBJECT_TYPE_FLAGDROP)   // make dropped flag clickable for other players (not set owner guid (created by) for this)...
        m_caster->AddGameObject(pGameObj);
    map->Add(pGameObj);

    if(pGameObj->GetMapId() == 489 && pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP)  //WS
    {
        if(m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            Player *pl = (Player*)m_caster;
            BattleGround* bg = ((Player *)m_caster)->GetBattleGround();
            if(bg && bg->GetTypeID()==BATTLEGROUND_WS && bg->GetStatus() == STATUS_IN_PROGRESS)
            {
                 uint32 team = ALLIANCE;

                 if(pl->GetTeam() == team)
                     team = HORDE;

                ((BattleGroundWS*)bg)->SetDroppedFlagGUID(pGameObj->GetGUID(),team);
            }
        }
    }

  /*[TZERO]  if(pGameObj->GetMapId() == 566 && pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP)  //EY
    {
        if(m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            BattleGround* bg = ((Player *)m_caster)->GetBattleGround();
            if(bg && bg->GetTypeID()==BATTLEGROUND_EY && bg->GetStatus() == STATUS_IN_PROGRESS)
            {
                ((BattleGroundEY*)bg)->SetDroppedFlagGUID(pGameObj->GetGUID());
            }
        }
    } */

    if(uint32 linkedEntry = pGameObj->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if(linkedGO->Create(objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), linkedEntry, map,
            x, y, z, target->GetOrientation(), 0, 0, 0, 0, 100, 1))
        {
            linkedGO->SetRespawnTime(duration > 0 ? duration/1000 : 0);
            linkedGO->SetSpellId(m_spellInfo->Id);

            m_caster->AddGameObject(linkedGO);
            map->Add(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectScriptEffect(uint32 effIndex)
{
    // TODO: we must implement hunter pet summon at login there (spell 6962)

    // by spell id
    switch(m_spellInfo->Id)
    {
        // Bending Shinbone
        case 8856:
        {
            if(!itemTarget && m_caster->GetTypeId()!=TYPEID_PLAYER)
                return;

            uint32 spell_id = 0;
            switch(urand(1,5))
            {
                case 1:  spell_id = 8854; break;
                default: spell_id = 8855; break;
            }

            m_caster->CastSpell(m_caster,spell_id,true,NULL);
            return;
        }

        // Healthstone creating spells
        case  6201:
        case  6202:
        case  5699:
        case 11729:
        case 11730:
        {
            uint32 itemtype;
            uint32 rank = 0;
            Unit::AuraList const& mDummyAuras = unitTarget->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator i = mDummyAuras.begin();i != mDummyAuras.end(); ++i)
            {
                if((*i)->GetId() == 18692)
                {
                    rank = 1;
                    break;
                }
                else if((*i)->GetId() == 18693)
                {
                    rank = 2;
                    break;
                }
            }

            static uint32 const itypes[5][3] = {
                { 5512,19004,19005},                        // Minor Healthstone
                { 5511,19006,19007},                        // Lesser Healthstone
                { 5509,19008,19009},                        // Healthstone
                { 5510,19010,19011},                        // Greater Healthstone
                { 9421,19012,19013}                         // Major Healthstone
            };

            switch(m_spellInfo->Id)
            {
                case  6201: itemtype=itypes[0][rank];break; // Minor Healthstone
                case  6202: itemtype=itypes[1][rank];break; // Lesser Healthstone
                case  5699: itemtype=itypes[2][rank];break; // Healthstone
                case 11729: itemtype=itypes[3][rank];break; // Greater Healthstone
                case 11730: itemtype=itypes[4][rank];break; // Major Healthstone
                default:
                    return;
            }
            DoCreateItem( effIndex, itemtype );
            return;
        }

        // Piccolo of the Flaming Fire
        case 17512:
        {
            if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;
            unitTarget->HandleEmoteCommand(EMOTE_STATE_DANCE);
            break;
        }

        // Rockbiter Weapon Proc
        case  20865:
        case  20866:
        case  20867:
        case  20868:
        case  20870:
        case  20871:
        {
            if (!unitTarget || !unitTarget->CanHaveThreatList())
                return;

            if (unitTarget->getThreatManager().getThreat(m_caster))
                unitTarget->getThreatManager().addThreat(m_caster, damage * m_caster->GetAttackTime(BASE_ATTACK) / 1000);

            break;
        }

        // Shadow Flame (All script effects, not just end ones to prevent player from dodging the last triggered spell)
        case 22539:
        case 22972:
        case 22975:
        case 22976:
        case 22977:
        case 22978:
        case 22979:
        case 22980:
        case 22981:
        case 22982:
        case 22983:
        case 22984:
        case 22985:
        {
            if(!unitTarget || !unitTarget->isAlive())
                return;

            // Onyxia Scale Cloak
            if(unitTarget->GetDummyAura(22683))
                return;

            // Shadow Flame
            m_caster->CastSpell(unitTarget, 22682, true);
            return;
        }
        break;

        // Uther's Tribute
        case 24194:
        // Grom's Tribute
        case 24195:
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            uint8 race = m_caster->getRace();
            uint32 spellId = 0;

            switch (m_spellInfo->Id)
            {
                case 24194:
                    switch (race)
                    {
                        case RACE_HUMAN:
                            spellId = 24105;
                            break;
                        case RACE_DWARF:
                            spellId = 24107;
                            break;
                        case RACE_NIGHTELF:
                            spellId = 24108;
                            break;
                        case RACE_GNOME:
                            spellId = 24106;
                            break;
                    }
                    break;
                case 24195:
                    switch (race)
                    {
                        case RACE_ORC:
                            spellId = 24104;
                            break;
                        case RACE_UNDEAD_PLAYER:
                            spellId = 24103;
                            break;
                        case RACE_TAUREN:
                            spellId = 24102;
                            break;
                        case RACE_TROLL:
                            spellId = 24101;
                            break;
                    }
                    break;
            }

            if (spellId)
                m_caster->CastSpell(m_caster, spellId, true);

            return;
        }

        // Poisonous Blood
        case 24320:
        {
            unitTarget->CastSpell(unitTarget, 24321, true, NULL, NULL, m_caster->GetGUID());
            return;
        }

        case 24324: // Nostalrius : Ivina (Hakkar)
        {
            enum
            {
                SPELL_BLOODSIPHON_STUN       = 24324,
                SPELL_BLOODSIPHON_DAMAGE     = 24323,
                SPELL_BLOODSIPHON_HEAL       = 24322,
                SPELL_POISONOUS_BLOOD        = 24321
            };
            if (!unitTarget || !unitTarget->isAlive())
                return;

            if (unitTarget->HasAura(SPELL_POISONOUS_BLOOD,0))
                unitTarget->CastSpell(m_caster, SPELL_BLOODSIPHON_DAMAGE, true);
            else
                unitTarget->CastSpell(m_caster, SPELL_BLOODSIPHON_HEAL, true);
            return;
        }

        // Brittle Armor - need remove one 24575 Brittle Armor aura
        case 24590:
        {
            unitTarget->RemoveSingleAuraFromStack(24575, 0);
            unitTarget->RemoveSingleAuraFromStack(24575, 1);
            return;
        }

        // Trick
        case 24714:
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                return;

            if (roll_chance_i(14))                  // Trick (can be different critter models). 14% since below can have 1 of 6
                m_caster->CastSpell(m_caster, 24753, true);
            else                                    // Random Costume, 6 different (plus add. for gender)
                m_caster->CastSpell(m_caster, 24720, true);

            return;
        }

        // Pirate Costume
        case 24717:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Pirate Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24708 : 24709, true);
            return;
        }

        // Ninja Costume
        case 24718:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Ninja Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24711 : 24710, true);
            return;
        }

        // Leper Gnome Costume
        case 24719:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Leper Gnome Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24712 : 24713, true);
            return;
        }

        // Random Costume
        case 24720:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            uint32 spellId = 0;

            switch (urand(0, 6))
            {
                case 0:
                    spellId = unitTarget->getGender() == GENDER_MALE ? 24708 : 24709;
                    break;
                case 1:
                    spellId = unitTarget->getGender() == GENDER_MALE ? 24711 : 24710;
                    break;
                case 2:
                    spellId = unitTarget->getGender() == GENDER_MALE ? 24712 : 24713;
                    break;
                case 3:
                    spellId = 24723;
                    break;
                case 4:
                    spellId = 24732;
                    break;
                case 5:
                    spellId = unitTarget->getGender() == GENDER_MALE ? 24735 : 24736;
                    break;
                case 6:
                    spellId = 24740;
                    break;
            }

            m_caster->CastSpell(unitTarget, spellId, true);
            return;
        }

        // Ghost Costume
        case 24737:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Ghost Costume (male or female)
            m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24735 : 24736, true);
            return;
        }

        // Trick or Treat
        case 24751:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            // Tricked or Treated
            unitTarget->CastSpell(unitTarget, 24755, true);

            // Treat / Trick
            unitTarget->CastSpell(unitTarget, roll_chance_i(50) ? 24714 : 24715, true);
            return;
        }

        // Mistletoe
        case 26004:
        {
            if (!unitTarget)
                return;

            unitTarget->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
            return;
        }

        // Rotate Trigger
        case 26137:
        {
            if (!unitTarget)
                return;

            unitTarget->CastSpell(unitTarget, urand(0, 1) ? 26009 : 26136, true);
            return;
        }

        // Mistletoe
        case 26218:
        {
            if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            uint32 spells[2] = {26206, 26207};

            m_caster->CastSpell(unitTarget, spells[urand(0, 1)], true);
            return;
        }

        // PX-238 Winter Wondervolt TRAP
        case 26275:
        {
            if( unitTarget->HasAura(26272,0)
             || unitTarget->HasAura(26157,0)
             || unitTarget->HasAura(26273,0)
             || unitTarget->HasAura(26274,0))
                return;

            uint32 iTmpSpellId;

            switch(urand(0,3))
            {
                case 0:
                    iTmpSpellId = 26272;
                    break;
                case 1:
                    iTmpSpellId = 26157;
                    break;
                case 2:
                    iTmpSpellId = 26273;
                    break;
                case 3:
                    iTmpSpellId = 26274;
                    break;
            }

            unitTarget->CastSpell(unitTarget, iTmpSpellId, true);

            return;
        }

        // Mercurial Shield - need remove one 26464 Mercurial Shield aura
        case 26465:
        {
            unitTarget->RemoveSingleAuraFromStack(26464, 0);
            return;
        }

        // Summon Black Qiraji Battle Tank
        case 26656:
        {
            if(!unitTarget)
                return;

            // Prevent stacking of mounts
            unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

            // Two separate mounts depending on area id (allows use both in and out of specific instance)
            if (unitTarget->GetAreaId() == 3428)
                unitTarget->CastSpell(unitTarget, 25863, true, m_CastItem);
            else
                unitTarget->CastSpell(unitTarget, 26655, true, m_CastItem);
            break;
        }

        // Summon Bone Minions
        case 27687:
        {
            if (!unitTarget)
                return;

            unitTarget->CastSpell(unitTarget, 27690, true);
            unitTarget->CastSpell(unitTarget, 27691, true);
            unitTarget->CastSpell(unitTarget, 27692, true);
            unitTarget->CastSpell(unitTarget, 27693, true);
            return;
        }

        // Summon Bone Mages
        case 27695:
        {
            if (!unitTarget)
                return;

            unitTarget->CastSpell(unitTarget, 27696, true);
            unitTarget->CastSpell(unitTarget, 27697, true);
            unitTarget->CastSpell(unitTarget, 27698, true);
            unitTarget->CastSpell(unitTarget, 27699, true);
            return;
        }

        // Decimate (Naxxramas: Gluth)
        case 28374:
        {
            if (!unitTarget)
                return;

            int32 damage = unitTarget->GetHealth() - unitTarget->GetMaxHealth() * 0.05f;
            if (damage > 0)
                m_caster->CastCustomSpell(unitTarget, 28375, &damage, NULL, NULL, true);
            return;
        }

        // Summon Blizzard
        case 28560:
        {
            if (!unitTarget)
                return;

            unitTarget->CastSpell(unitTarget, 28561, true, NULL, NULL, m_caster->GetGUID());
            return;
        }

        // Teleport to Twin Emperors
        case 29182:
        {
            if (unitTarget && (unitTarget->GetTypeId() == TYPEID_PLAYER))
                unitTarget->CastSpell(unitTarget, 29181, true);
            return;
        }

        // Teleport to Final Chamber
        case 29188:
        {
            if (unitTarget && (unitTarget->GetTypeId() == TYPEID_PLAYER))
                unitTarget->CastSpell(unitTarget, 29190, true);
            return;
        }

        // Improved Sprint
        case 30918:
        {
            // Removes snares and roots.
            uint32 mechanic_mask = (1<<MECHANIC_ROOT) | (1<<MECHANIC_SNARE);
            Unit::AuraMap& Auras = unitTarget->GetAuras();
            for(Unit::AuraMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
            {
                next = iter;
                ++next;
                Aura *aur = iter->second;
                if (!aur->IsPositive())             //only remove negative spells
                {
                    // check for mechanic mask
                    if(GetSpellMechanicMask(aur->GetSpellProto(), aur->GetEffIndex()) & mechanic_mask)
                    {
                        unitTarget->RemoveAurasDueToSpell(aur->GetId());
                        if(Auras.empty())
                            break;
                        else
                            next = Auras.begin();
                    }
                }
            }
            break;
        }
    }

    if(!unitTarget || !unitTarget->isAlive()) // can we remove this check?
    {
        sLog.outError("Spell %u in EffectScriptEffect does not have unitTarget", m_spellInfo->Id);
        return;
    }

    if( m_spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN )
    {
        // Flash of Light
        if (m_spellInfo->SpellIconID  == 242)
        {
            if (!unitTarget || !unitTarget->isAlive())
                return;
            int32 heal = damage;
            
            int32 spellid = m_spellInfo->Id;            // send main spell id as basepoints for not used effect
            m_caster->CastCustomSpell(unitTarget, 19993, &heal, &spellid, NULL, true);
        }
        else if (m_spellInfo->SpellFamilyFlags == 0x800000) // Judgement
        {
            uint32 spellId2 = 0;

            // all seals have aura dummy
            Unit::AuraList const& m_dummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator itr = m_dummyAuras.begin(); itr != m_dummyAuras.end(); ++itr)
            {
                SpellEntry const *spellInfo = (*itr)->GetSpellProto();

                // search seal (all seals have judgement's aura dummy spell id in 2 effect
                if ( !spellInfo || !IsSealSpell((*itr)->GetSpellProto()) || (*itr)->GetEffIndex() != 2 )
                    continue;

                // must be calculated base at raw base points in spell proto, GetModifier()->m_value for S.Righteousness modified by SPELLMOD_DAMAGE
                spellId2 = (*itr)->GetSpellProto()->EffectBasePoints[2]+1;

                if(spellId2 <= 1)
                    continue;

                // found, remove seal
                m_caster->RemoveAurasDueToSpell((*itr)->GetId());

                break;
            }

            m_caster->CastSpell(unitTarget,spellId2,true);

            return;
        }
    }

    // normal DB scripted effect
    sLog.outDebug("Spell ScriptStart spellid %u in EffectScriptEffect ", m_spellInfo->Id);
    sWorld.ScriptsStart(sSpellScripts, m_spellInfo->Id, m_caster, unitTarget);
}

void Spell::EffectSanctuary(uint32 /*i*/)
{
    if(!unitTarget)
        return;

    std::list<Unit*> targets;
    Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(unitTarget, unitTarget, World::GetMaxVisibleDistance());
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
    unitTarget->VisitNearbyObject(World::GetMaxVisibleDistance(), searcher);
    for(std::list<Unit*>::iterator iter = targets.begin(); iter != targets.end(); ++iter)
    {
        if(!(*iter)->hasUnitState(UNIT_STAT_CASTING))
            continue;

        for(uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
        {
            if((*iter)->m_currentSpells[i]
            && (*iter)->m_currentSpells[i]->m_targets.getUnitTargetGUID() == unitTarget->GetGUID())
            {
                (*iter)->InterruptSpell(i, false);
            }
        }
    }

    unitTarget->CombatStop();
    unitTarget->getHostilRefManager().deleteReferences();   // stop all fighting
    // Vanish allows to remove all threat and cast regular stealth so other spells can be used
    if(m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && (m_spellInfo->SpellFamilyFlags & SPELLFAMILYFLAG_ROGUE_VANISH))
    {
        ((Player *)m_caster)->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
    }
}

void Spell::EffectAddComboPoints(uint32 /*i*/)
{
    if(!unitTarget)
        return;

    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if(damage <= 0)
        return;

    ((Player*)m_caster)->AddComboPoints(unitTarget, damage);
    m_caster->SetUInt64Value(PLAYER_FIELD_COMBO_TARGET,unitTarget->GetGUID());
}

void Spell::EffectDuel(uint32 i)
{
    if(!m_caster || !unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *caster = (Player*)m_caster;
    Player *target = (Player*)unitTarget;

    // caster or target already have requested duel
    if( caster->duel || target->duel || !target->GetSocial() || target->GetSocial()->HasIgnore(caster->GetGUIDLow()) )
        return;

    // Players can only fight a duel with each other outside (=not inside dungeons and not in capital cities)
    // Don't have to check the target's map since you cannot challenge someone across maps
    if(caster->GetMap()->Instanceable())
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    AreaTableEntry const* casterAreaEntry = GetAreaEntryByAreaID(caster->GetZoneId());
    if(casterAreaEntry && (casterAreaEntry->flags & AREA_FLAG_CAPITAL) )
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    AreaTableEntry const* targetAreaEntry = GetAreaEntryByAreaID(target->GetZoneId());
    if(targetAreaEntry && (targetAreaEntry->flags & AREA_FLAG_CAPITAL) )
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    //CREATE DUEL FLAG OBJECT
    GameObject* pGameObj = new GameObject;

    uint32 gameobject_id = m_spellInfo->EffectMiscValue[i];

    Map *map = m_caster->GetMap();
    if(!pGameObj->Create(objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
        m_caster->GetPositionX()+(unitTarget->GetPositionX()-m_caster->GetPositionX())/2 ,
        m_caster->GetPositionY()+(unitTarget->GetPositionY()-m_caster->GetPositionY())/2 ,
        m_caster->GetPositionZ(),
        m_caster->GetOrientation(), 0, 0, 0, 0, 0, 1))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_FACTION, m_caster->getFaction() );
    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel()+1 );
    int32 duration = GetSpellDuration(m_spellInfo);
    pGameObj->SetRespawnTime(duration > 0 ? duration/1000 : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    m_caster->AddGameObject(pGameObj);
    map->Add(pGameObj);
    //END

    // Send request
    WorldPacket data(SMSG_DUEL_REQUESTED, 16);
    data << pGameObj->GetGUID();
    data << caster->GetGUID();
    caster->GetSession()->SendPacket(&data);
    target->GetSession()->SendPacket(&data);

    // create duel-info
    DuelInfo *duel   = new DuelInfo;
    duel->initiator  = caster;
    duel->opponent   = target;
    duel->startTime  = 0;
    duel->startTimer = 0;
    caster->duel     = duel;

    DuelInfo *duel2   = new DuelInfo;
    duel2->initiator  = caster;
    duel2->opponent   = caster;
    duel2->startTime  = 0;
    duel2->startTimer = 0;
    target->duel      = duel2;

    caster->SetUInt64Value(PLAYER_DUEL_ARBITER,pGameObj->GetGUID());
    target->SetUInt64Value(PLAYER_DUEL_ARBITER,pGameObj->GetGUID());
}

void Spell::EffectStuck(uint32 /*i*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(!sWorld.getConfig(CONFIG_CAST_UNSTUCK))
        return;

    Player* pTarget = (Player*)unitTarget;

    sLog.outDebug("Spell Effect: Stuck");
    sLog.outDetail("Player %s (guid %u) used auto-unstuck future at map %u (%f, %f, %f)", pTarget->GetName(), pTarget->GetGUIDLow(), m_caster->GetMapId(), m_caster->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());

    if(pTarget->isInFlight())
        return;

    // homebind location is loaded always
    pTarget->TeleportTo(pTarget->m_homebindMapId,pTarget->m_homebindX,pTarget->m_homebindY,pTarget->m_homebindZ,pTarget->GetOrientation(), (unitTarget==m_caster ? TELE_TO_SPELL : 0));

    // Stuck spell trigger Hearthstone cooldown
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(8690);
    if(!spellInfo)
        return;
    Spell spell(pTarget,spellInfo,true,0);
    spell.SendSpellCooldown();
}

void Spell::EffectSummonPlayer(uint32 /*i*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Evil Twin (ignore player summon, but hide this for summoner)
    if(unitTarget->GetDummyAura(23445))
        return;

    float x,y,z;
    m_caster->GetClosePoint(x,y,z,unitTarget->GetObjectSize());

    ((Player*)unitTarget)->SetSummonPoint(m_caster->GetMapId(),x,y,z);

    WorldPacket data(SMSG_SUMMON_REQUEST, 8+4+4);
    data << uint64(m_caster->GetGUID());                    // summoner guid
    data << uint32(m_caster->GetZoneId());                  // summoner zone
    data << uint32(MAX_PLAYER_SUMMON_DELAY*1000);           // auto decline after msecs
    ((Player*)unitTarget)->GetSession()->SendPacket(&data);
}

static ScriptInfo generateActivateCommand()
{
    ScriptInfo si;
    si.command = SCRIPT_COMMAND_ACTIVATE_OBJECT;
    return si;
}

void Spell::EffectActivateObject(uint32 effect_idx)
{
    if(!gameObjTarget)
        return;

    static ScriptInfo activateCommand = generateActivateCommand();

    int32 delay_secs = m_spellInfo->EffectMiscValue[effect_idx];

    sWorld.ScriptCommandStart(activateCommand, delay_secs, m_caster, gameObjTarget);
}

void Spell::EffectSummonTotem(uint32 i)
{
    uint8 slot = 0;
    switch(m_spellInfo->Effect[i])
    {
        case SUMMON_TYPE_TOTEM_SLOT1: slot = 0; break;
        case SUMMON_TYPE_TOTEM_SLOT2: slot = 1; break;
        case SUMMON_TYPE_TOTEM_SLOT3: slot = 2; break;
        case SUMMON_TYPE_TOTEM_SLOT4: slot = 3; break;
        // Battle standard case
        case SUMMON_TYPE_TOTEM:       slot = 254; break;
        // jewelery statue case, like totem without slot
        case SUMMON_TYPE_GUARDIAN:    slot = 255; break;
        default: return;
    }

    if(slot < MAX_TOTEM)
    {
        uint64 guid = m_caster->m_TotemSlot[slot];
        if(guid != 0)
        {
            Creature *OldTotem = ObjectAccessor::GetCreature(*m_caster, guid);
            if(OldTotem && OldTotem->isTotem())
                ((Totem*)OldTotem)->UnSummon();
        }
    }

    uint32 team = 0;
    if (m_caster->GetTypeId()==TYPEID_PLAYER)
        team = ((Player*)m_caster)->GetTeam();

    Totem* pTotem = new Totem;

    if(!pTotem->Create(objmgr.GenerateLowGuid(HIGHGUID_UNIT), m_caster->GetMap(), m_spellInfo->EffectMiscValue[i], team ))
    {
        delete pTotem;
        return;
    }

    float angle = slot < MAX_TOTEM ? M_PI/MAX_TOTEM - (slot*2*M_PI/MAX_TOTEM) : 0;

    float x,y,z;
    m_caster->GetClosePoint(x,y,z,pTotem->GetObjectSize(),2.0f,angle);

    // totem must be at same Z in case swimming caster and etc.
    if( fabs( z - m_caster->GetPositionZ() ) > 5 )
        z = m_caster->GetPositionZ();

    pTotem->Relocate(x, y, z, m_caster->GetOrientation());

    if(slot < MAX_TOTEM)
        m_caster->m_TotemSlot[slot] = pTotem->GetGUID();

    pTotem->SetOwner(m_caster->GetGUID());
    pTotem->SetTypeBySummonSpell(m_spellInfo);              // must be after Create call where m_spells initilized

    int32 duration=GetSpellDuration(m_spellInfo);
    if(Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id,SPELLMOD_DURATION, duration);
    pTotem->SetDuration(duration);

    if (damage)                                             // if not spell info, DB values used
    {
        pTotem->SetMaxHealth(damage);
        pTotem->SetHealth(damage);
    }

    pTotem->SetUInt32Value(UNIT_CREATED_BY_SPELL,m_spellInfo->Id);
    pTotem->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_PVP_ATTACKABLE);

    pTotem->ApplySpellImmune(m_spellInfo->Id,IMMUNITY_STATE,SPELL_AURA_MOD_FEAR,true);
    pTotem->ApplySpellImmune(m_spellInfo->Id,IMMUNITY_STATE,SPELL_AURA_TRANSFORM,true);

    pTotem->Summon(m_caster);

    if(slot < MAX_TOTEM && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(SMSG_TOTEM_CREATED, 1+8+4+4);
        data << uint8(slot);
        data << uint64(pTotem->GetGUID());
        data << uint32(duration);
        data << uint32(m_spellInfo->Id);
        ((Player*)m_caster)->SendDirectMessage(&data);
    }
}

void Spell::EffectEnchantHeldItem(uint32 i)
{
    // this is only item spell effect applied to main-hand weapon of target player (players in area)
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* item_owner = (Player*)unitTarget;
    Item* item = item_owner->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if(!item )
        return;

    // must be equipped
    if(!item ->IsEquipped())
        return;

    if (m_spellInfo->EffectMiscValue[i])
    {
        uint32 enchant_id = m_spellInfo->EffectMiscValue[i];
        int32 duration = GetSpellDuration(m_spellInfo);          //Try duration index first ..
        if(!duration)
            duration = damage;//+1;            //Base points after ..
        if(!duration)
            duration = 10;                                  //10 seconds for enchants which don't have listed duration

        SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if(!pEnchant)
            return;

        // Always go to temp enchantment slot
        EnchantmentSlot slot = TEMP_ENCHANTMENT_SLOT;

        // Enchantment will not be applied if a different one already exists
        if(item->GetEnchantmentId(slot) && item->GetEnchantmentId(slot) != enchant_id)
            return;

        // Apply the temporary enchantment
        item->SetEnchantment(slot, enchant_id, duration*1000, 0);
        item_owner->ApplyEnchantment(item,slot,true);
    }
}

void Spell::EffectDisEnchant(uint32 /*i*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;
    if(!itemTarget || !itemTarget->GetProto()->DisenchantID)
        return;

    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    ((Player*)m_caster)->SendLoot(itemTarget->GetGUID(),LOOT_DISENCHANTING);

    // item will be removed at disenchanting end
}

void Spell::EffectInebriate(uint32 /*i*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)unitTarget;
    uint16 currentDrunk = player->GetDrunkValue();
    uint16 drunkMod = damage * 256;
    if (currentDrunk + drunkMod > 0xFFFF)
        currentDrunk = 0xFFFF;
    else
        currentDrunk += drunkMod;
    player->SetDrunkValue(currentDrunk, m_CastItem?m_CastItem->GetEntry():0);
}

void Spell::EffectFeedPet(uint32 i)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    Item* foodItem = m_targets.getItemTarget();
    if(!foodItem)
        return;

    Pet *pet = _player->GetPet();
    if(!pet)
        return;

    if(!pet->isAlive())
        return;

    int32 benefit = pet->GetCurrentFoodBenefitLevel(itemTarget->GetProto()->ItemLevel);
    if(benefit <= 0)
        return;

    uint32 count = 1;
    _player->DestroyItemCount(itemTarget,count,true);
    // TODO: fix crash when a spell has two effects, both pointed at the same item target

    m_caster->CastCustomSpell(m_caster,m_spellInfo->EffectTriggerSpell[i], &benefit , NULL , NULL , true);
}

void Spell::EffectDismissPet(uint32 /*i*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Pet* pet = m_caster->GetPet();

    // not let dismiss dead pet
    if(!pet||!pet->isAlive())
        return;

    ((Player*)m_caster)->RemovePet(pet,PET_SAVE_NOT_IN_SLOT);
}

void Spell::EffectSummonObject(uint32 i)
{
    uint32 go_id = m_spellInfo->EffectMiscValue[i];

    uint8 slot = 0;
    switch(m_spellInfo->Effect[i])
    {
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT1: slot = 0; break;
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT2: slot = 1; break;
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT3: slot = 2; break;
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT4: slot = 3; break;
        default: return;
    }

    uint64 guid = m_caster->m_ObjectSlot[slot];
    if(guid != 0)
    {
        GameObject* obj = NULL;
        if( m_caster )
            obj = ObjectAccessor::GetGameObject(*m_caster, guid);

        if(obj) obj->Delete();
        m_caster->m_ObjectSlot[slot] = 0;
    }

    GameObject* pGameObj = new GameObject;

    float rot2 = sin(m_caster->GetOrientation()/2);
    float rot3 = cos(m_caster->GetOrientation()/2);

    float x,y,z;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
    }
    // Summon in random point all other units if location present
    else
        m_caster->GetClosePoint(x,y,z,DEFAULT_WORLD_OBJECT_SIZE);

    Map *map = m_caster->GetMap();
    if(!pGameObj->Create(objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), go_id, map, x, y, z, m_caster->GetOrientation(), 0, 0, rot2, rot3, 0, 1))
    {
        delete pGameObj;
        return;
    }

    //pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL,m_caster->getLevel());
    int32 duration = GetSpellDuration(m_spellInfo);
    pGameObj->SetRespawnTime(duration > 0 ? duration/1000 : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);
    m_caster->AddGameObject(pGameObj);

    map->Add(pGameObj);
    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
    data << pGameObj->GetGUID();
    m_caster->SendMessageToSet(&data,true);

    m_caster->m_ObjectSlot[slot] = pGameObj->GetGUID();
}

void Spell::EffectResurrect(uint32 /*effIndex*/)
{
    if(!unitTarget)
        return;
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(unitTarget->isAlive())
        return;
    if(!unitTarget->IsInWorld())
        return;

    switch (m_spellInfo->Id)
    {
        // Defibrillate (Goblin Jumper Cables) have 33% chance on success
        case 8342:
            if (roll_chance_i(67))
            {
                m_caster->CastSpell(m_caster, 8338, true, m_CastItem);
                return;
            }
            break;
        // Defibrillate (Goblin Jumper Cables XL) have 50% chance on success
        case 22999:
            if (roll_chance_i(50))
            {
                m_caster->CastSpell(m_caster, 23055, true, m_CastItem);
                return;
            }
            break;
        default:
            break;
    }

    Player* pTarget = ((Player*)unitTarget);

    if(pTarget->isRessurectRequested())       // already have one active request
        return;

    uint32 health = pTarget->GetMaxHealth() * damage / 100;
    uint32 mana   = pTarget->GetMaxPower(POWER_MANA) * damage / 100;

    pTarget->setResurrectRequestData(m_caster->GetGUID(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana);
    SendResurrectRequest(pTarget);
}

void Spell::EffectAddExtraAttacks(uint32 /*i*/)
{
    if(!unitTarget || !unitTarget->isAlive())
        return;

    if (m_spellInfo->Id == 20178) // Reckoning
    {
        if (unitTarget->m_extraAttacks < 4)
            unitTarget->m_extraAttacks++;
        return;
    }

    if( unitTarget->m_extraAttacks )
        return;

    unitTarget->m_extraAttacks = damage;
}

void Spell::EffectParry(uint32 /*i*/)
{
    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)unitTarget)->SetCanParry(true);
    }
}

void Spell::EffectBlock(uint32 /*i*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->SetCanBlock(true);
}

void Spell::EffectMomentMove(uint32 i)
{
    if(unitTarget->isInFlight())
        return;

    if(!m_targets.HasDst())
        return;

    uint32 mapid = m_caster->GetMapId();
    float dis = GetSpellRadius(m_spellInfo,i,false);

    // src point
    float *fx = new float[11], *fy = new float[11], *fz = new float[11];
    unitTarget->GetPosition(fx[0], fy[0], fz[0]);

    float orientation = unitTarget->GetOrientation(), itr_i, step = dis / 10.0, fx2, fy2, fz2, ground, floor;
    int itr_j = 1, last_valid = 0;
    bool hit = false;

    for (itr_i = step; itr_i <= dis; itr_i += step)
    {
        fx[itr_j] = fx[0] + itr_i * cos(orientation);
        fy[itr_j] = fy[0] + itr_i * sin(orientation);
        ground = MapManager::Instance().GetMap(mapid, unitTarget)->GetHeight(fx[itr_j], fy[itr_j], MAX_HEIGHT, true);
        floor = MapManager::Instance().GetMap(mapid, unitTarget)->GetHeight(fx[itr_j], fy[itr_j], fz[last_valid], true);
        fz[itr_j] = fabs(ground - fz[last_valid]) <= fabs(floor - fz[last_valid]) ? ground : floor;
        if (fabs(fz[itr_j] - fz[0]) <= 6.0)
        {
            if (VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(mapid, fx[last_valid], fy[last_valid], fz[last_valid] + 0.5, fx[itr_j], fy[itr_j], fz[itr_j] + 0.5, fx2, fy2, fz2, -0.5))
            {
                hit = true;
                fx[itr_j] = fx2 - 0.6 * cos(orientation);
                fy[itr_j] = fy2 - 0.6 * sin(orientation);
                ground = MapManager::Instance().GetMap(mapid, unitTarget)->GetHeight(fx[itr_j], fy[itr_j], MAX_HEIGHT, true);
                floor = MapManager::Instance().GetMap(mapid, unitTarget)->GetHeight(fx[itr_j], fy[itr_j], fz[last_valid], true);
                float tempz = fabs(ground - fz[last_valid]) <= fabs(floor - fz[last_valid]) ? ground : floor;
                fz[itr_j] = fabs(tempz - fz[last_valid]) <= fabs(fz2 - fz[last_valid]) ? tempz : fz2;
                break;
            }
            else
                last_valid = itr_j;
        }
        itr_j++;
    }
    if (hit == false)
        itr_j = last_valid;


    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->TeleportTo(mapid, fx[itr_j], fy[itr_j], fz[itr_j] + 0.07531, orientation, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget == m_caster ? TELE_TO_SPELL : 0));
    else
        MapManager::Instance().GetMap(mapid, unitTarget)->CreatureRelocation((Creature*)unitTarget, fx[itr_j], fy[itr_j], fz[itr_j] + 0.07531, orientation);

    delete [] fx; delete [] fy; delete [] fz;

/*    uint32 mapid = unitTarget->GetMapId();
    float ox,oy,oz;
    unitTarget->GetPosition(ox,oy,oz);

    float fx,fy,fz;                                  // getObjectHitPos overwrite last args in any result case
    if(VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(mapid, ox,oy,oz+0.5, m_targets.m_destX,m_targets.m_destY,oz+0.5,fx,fy,fz, -0.5))
        unitTarget->UpdateGroundPositionZ(fx,fy,fz);

    if(unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->TeleportTo(mapid, fx, fy, fz, unitTarget->GetOrientation(), TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget==m_caster ? TELE_TO_SPELL : 0));
    else
        MapManager::Instance().GetMap(mapid, unitTarget)->CreatureRelocation((Creature*)unitTarget, fx, fy, fz, unitTarget->GetOrientation());*/
}

void Spell::EffectReputation(uint32 i)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)unitTarget;

    int32  rep_change = damage;//+1;           // field store reputation change -1

    uint32 faction_id = m_spellInfo->EffectMiscValue[i];

    FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

    if(!factionEntry)
        return;

    _player->ModifyFactionReputation(factionEntry,rep_change);
}

void Spell::EffectQuestComplete(uint32 i)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    uint32 quest_id = m_spellInfo->EffectMiscValue[i];
    _player->AreaExploredOrEventHappens(quest_id);
}

void Spell::EffectSelfResurrect(uint32 i)
{
    if(!unitTarget || unitTarget->isAlive())
        return;
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if(!unitTarget->IsInWorld())
        return;

    uint32 health = 0;
    uint32 mana = 0;

    // flat case
    if(damage < 0)
    {
        health = uint32(-damage);
        mana = m_spellInfo->EffectMiscValue[i];
    }
    // percent case
    else
    {
        health = uint32(damage/100.0f*unitTarget->GetMaxHealth());
        if(unitTarget->GetMaxPower(POWER_MANA) > 0)
            mana = uint32(damage/100.0f*unitTarget->GetMaxPower(POWER_MANA));
    }

    Player *plr = ((Player*)unitTarget);
    plr->ResurrectPlayer(0.0f);

    plr->SetHealth( health );
    plr->SetPower(POWER_MANA, mana );
    plr->SetPower(POWER_RAGE, 0 );
    plr->SetPower(POWER_ENERGY, plr->GetMaxPower(POWER_ENERGY) );

    plr->SpawnCorpseBones();

    plr->SaveToDB();
}

void Spell::EffectSkinning(uint32 /*i*/)
{
    if(unitTarget->GetTypeId() != TYPEID_UNIT )
        return;
    if(!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Creature* creature = (Creature*) unitTarget;
    int32 targetLevel = creature->getLevel();

    uint32 skill = creature->GetCreatureInfo()->GetRequiredLootSkill();

    ((Player*)m_caster)->SendLoot(creature->GetGUID(),LOOT_SKINNING);
    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

    int32 reqValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel-10)*10 : targetLevel*5;

    int32 skillValue = ((Player*)m_caster)->GetPureSkillValue(skill);

    // Double chances for elites
    ((Player*)m_caster)->UpdateGatherSkill(skill, skillValue, reqValue, creature->isElite() ? 2 : 1 );
}

void Spell::EffectCharge(uint32 /*i*/)
{
    if(!m_caster)
        return;

    Unit *target = m_targets.getUnitTarget();
    if(!target)
        return;

    float x, y, z;
    target->GetContactPoint(m_caster, x, y, z);
    m_caster->GetMotionMaster()->MoveCharge(x, y, z);

    // not all charge effects used in negative spells
    if ( !IsPositiveSpell(m_spellInfo->Id) && m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->Attack(target, true);
}

void Spell::EffectSummonCritter(uint32 i)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = (Player*)m_caster;

    uint32 pet_entry = m_spellInfo->EffectMiscValue[i];
    if(!pet_entry)
        return;

    Pet* old_critter = player->GetMiniPet();

    // for same pet just despawn
    if(old_critter && old_critter->GetEntry() == pet_entry)
    {
        player->RemoveMiniPet();
        return;
    }

    // despawn old pet before summon new
    if(old_critter)
        player->RemoveMiniPet();

    // summon new pet
    Pet* critter = new Pet(MINI_PET);

    Map *map = m_caster->GetMap();
    uint32 pet_number = objmgr.GeneratePetNumber();
    if(!critter->Create(objmgr.GenerateLowGuid(HIGHGUID_PET),
        map, pet_entry, pet_number))
    {
        sLog.outError("Spell::EffectSummonCritter, spellid %u: no such creature entry %u", m_spellInfo->Id, pet_entry);
        delete critter;
        return;
    }

    float x,y,z;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
         x = m_targets.m_destX;
         y = m_targets.m_destY;
         z = m_targets.m_destZ;
     }
     // Summon if dest location not present near caster
     else
         m_caster->GetClosePoint(x,y,z,critter->GetObjectSize());

    critter->Relocate(x,y,z,m_caster->GetOrientation());

    if(!critter->IsPositionValid())
    {
        sLog.outError("ERROR: Pet (guidlow %d, entry %d) not summoned. Suggested coordinates isn't valid (X: %f Y: %f)",
            critter->GetGUIDLow(), critter->GetEntry(), critter->GetPositionX(), critter->GetPositionY());
        delete critter;
        return;
    }

    critter->SetOwnerGUID(m_caster->GetGUID());
    critter->SetCreatorGUID(m_caster->GetGUID());
    critter->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE,m_caster->getFaction());
    critter->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    critter->AIM_Initialize();
    critter->InitPetCreateSpells();                         // e.g. disgusting oozeling has a create spell as critter...
    critter->SetMaxHealth(1);
    critter->SetHealth(1);
    critter->SetLevel(1);

    // set timer for unsummon
    int32 duration = GetSpellDuration(m_spellInfo);
    if(duration > 0)
        critter->SetDuration(duration);

    std::string name = player->GetName();
    name.append(petTypeSuffix[critter->getPetType()]);
    critter->SetName( name );
    player->SetMiniPet(critter);

    map->Add((Creature*)critter);
}

void Spell::EffectKnockBack(uint32 i)
{
    if(!unitTarget)
        return;

    // Effect only works on players
    if(unitTarget->GetTypeId()!=TYPEID_PLAYER)
        return;

    float x, y;
    if(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
    }
    else
    {
        x = m_caster->GetPositionX();
        y = m_caster->GetPositionY();
    }

    float dx = unitTarget->GetPositionX() - x;
    float dy = unitTarget->GetPositionY() - y;
    float vcos, vsin;
    if(dx < 0.001f && dy < 0.001f)
    {
        float angle = rand_norm()*2*M_PI;
        vcos = cos(angle);
        vsin = sin(angle);
    }
    else
    {
        float dist = sqrt((dx*dx) + (dy*dy));
        vcos = dx / dist;
        vsin = dy / dist;
    }

    WorldPacket data(SMSG_MOVE_KNOCK_BACK, (8+4+4+4+4+4));
    data.append(unitTarget->GetPackGUID());
    data << uint32(0);                                      // Sequence
    data << float(vcos);                                    // x direction
    data << float(vsin);                                    // y direction
    data << float(m_spellInfo->EffectMiscValue[i])/10;      // Horizontal speed
    data << float(damage/-10);                              // Z Movement speed (vertical)

    ((Player*)unitTarget)->GetSession()->SendPacket(&data);
}

void Spell::EffectSendTaxi(uint32 i)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(m_spellInfo->EffectMiscValue[i]);
    if(!entry)
        return;

    std::vector<uint32> nodes;

    nodes.resize(2);
    nodes[0] = entry->from;
    nodes[1] = entry->to;

    uint32 mountid = 0;
    switch(m_spellInfo->Id)
    {
        case 31606:       //Stormcrow Amulet
            mountid = 17447;
            break;
        case 45071:      //Quest - Sunwell Daily - Dead Scar Bombing Run
        case 45113:      //Quest - Sunwell Daily - Ship Bombing Run
        case 45353:      //Quest - Sunwell Daily - Ship Bombing Run Return
            mountid = 22840;
            break;
        case 34905:      //Stealth Flight
            mountid = 6851;
            break;
        case 41533:      //Fly of the Netherwing
        case 41540:      //Fly of the Netherwing
            mountid = 23468;
            break;
    }

    ((Player*)unitTarget)->ActivateTaxiPathTo(nodes,mountid);

}

void Spell::EffectPlayerPull(uint32 i)
{
    if(!unitTarget || !m_caster)
        return;

    // Effect only works on players
    if(unitTarget->GetTypeId()!=TYPEID_PLAYER)
        return;

    float vsin = sin(unitTarget->GetAngle(m_caster));
    float vcos = cos(unitTarget->GetAngle(m_caster));

    WorldPacket data(SMSG_MOVE_KNOCK_BACK, (8+4+4+4+4+4));
    data.append(unitTarget->GetPackGUID());
    data << uint32(0);                                      // Sequence
    data << float(vcos);                                    // x direction
    data << float(vsin);                                    // y direction
                                                            // Horizontal speed
    data << float(damage ? damage : unitTarget->GetDistance2d(m_caster));
    data << float(m_spellInfo->EffectMiscValue[i])/-10;     // Z Movement speed

    ((Player*)unitTarget)->GetSession()->SendPacket(&data);
}

void Spell::EffectDispelMechanic(uint32 i)
{
    if(!unitTarget)
        return;

    uint32 mechanic = m_spellInfo->EffectMiscValue[i];

    Unit::AuraMap& Auras = unitTarget->GetAuras();
    for(Unit::AuraMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
    {
        next = iter;
        ++next;
        SpellEntry const *spell = sSpellStore.LookupEntry(iter->second->GetSpellProto()->Id);
        if(spell->Mechanic == mechanic || spell->EffectMechanic[iter->second->GetEffIndex()] == mechanic)
        {
            unitTarget->RemoveAurasDueToSpell(spell->Id);
            if(Auras.empty())
                break;
            else
                next = Auras.begin();
        }
    }
    return;
}

void Spell::EffectSummonDeadPet(uint32 /*i*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player *_player = (Player*)m_caster;
    Pet *pet = _player->GetPet();
    if(!pet)
        return;
    if(pet->isAlive())
        return;
    if(damage < 0)
        return;
    // Chakor : Teleport the pet to the player's location
    pet->NearTeleportTo(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetOrientation(), false);
    pet->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
    pet->RemoveFlag (UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    pet->setDeathState( ALIVE );
    pet->clearUnitState(UNIT_STAT_ALL_STATE);
    pet->SetHealth( uint32(pet->GetMaxHealth()*(float(damage)/100)));

    pet->AIM_Initialize();
    pet->GetMotionMaster()->MoveFollow(_player,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
    pet->GetCharmInfo()->SetCommandState( COMMAND_FOLLOW );

    pet->GetCharmInfo()->SetIsCommandAttack(false);
    pet->GetCharmInfo()->SetIsAtStay(false);
    pet->GetCharmInfo()->SetIsReturning(true);
    pet->GetCharmInfo()->SetIsFollowing(false);

    _player->PetSpellInitialize();
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
}

void Spell::EffectDestroyAllTotems(uint32 /*i*/)
{
    float mana = 0;
    for(int slot = 0;  slot < MAX_TOTEM; ++slot)
    {
        if(!m_caster->m_TotemSlot[slot])
            continue;

        Creature* totem = ObjectAccessor::GetCreature(*m_caster,m_caster->m_TotemSlot[slot]);
        if(totem && totem->isTotem())
        {
            uint32 spell_id = totem->GetUInt32Value(UNIT_CREATED_BY_SPELL);
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell_id);
            if(spellInfo)
                mana += spellInfo->manaCost * damage / 100;
            ((Totem*)totem)->UnSummon();
        }
    }

    int32 gain = m_caster->ModifyPower(POWER_MANA,int32(mana));
    m_caster->SendEnergizeSpellLog(m_caster, m_spellInfo->Id, gain, POWER_MANA);
}

void Spell::EffectDurabilityDamage(uint32 i)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[i];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if(slot < 0)
    {
        ((Player*)unitTarget)->DurabilityPointsLossAll(damage,(slot < -1));
        return;
    }

    // invalid slot value
    if(slot >= INVENTORY_SLOT_BAG_END)
        return;

    if(Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0,slot))
        ((Player*)unitTarget)->DurabilityPointsLoss(item,damage);
}

void Spell::EffectDurabilityDamagePCT(uint32 i)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[i];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if(slot < 0)
    {
        ((Player*)unitTarget)->DurabilityLossAll(double(damage)/100.0f,(slot < -1));
        return;
    }

    // invalid slot value
    if(slot >= INVENTORY_SLOT_BAG_END)
        return;

    if(damage <= 0)
        return;

    if(Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0,slot))
        ((Player*)unitTarget)->DurabilityLoss(item,double(damage)/100.0f);
}

void Spell::EffectModifyThreatPercent(uint32 /*effIndex*/)
{
    if(!unitTarget)
        return;

    unitTarget->getThreatManager().modifyThreatPercent(m_caster, damage);
}

void Spell::EffectTransmitted(uint32 effIndex)
{
    uint32 name_id = m_spellInfo->EffectMiscValue[effIndex];

    GameObjectInfo const* goinfo = objmgr.GetGameObjectInfo(name_id);

    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject (Entry: %u) not exist and not created at spell (ID: %u) cast",name_id, m_spellInfo->Id);
        return;
    }

    float fx,fy,fz;

    if(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        fx = m_targets.m_destX;
        fy = m_targets.m_destY;
        fz = m_targets.m_destZ;
    }
    //FIXME: this can be better check for most objects but still hack
    else if(m_spellInfo->EffectRadiusIndex[effIndex] && m_spellInfo->speed==0)
    {
        float dis = GetSpellRadius(m_spellInfo,effIndex,false);
        m_caster->GetClosePoint(fx,fy,fz,DEFAULT_WORLD_OBJECT_SIZE, dis);
    }
    else
    {
        float min_dis = GetSpellMinRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float max_dis = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float dis = rand_norm() * (max_dis - min_dis) + min_dis;

        m_caster->GetClosePoint(fx,fy,fz,DEFAULT_WORLD_OBJECT_SIZE, dis);
    }

    Map *cMap = m_caster->GetMap();

    if(goinfo->type==GAMEOBJECT_TYPE_FISHINGNODE)
    {
        if ( !cMap->IsInWater(fx,fy,fz-0.5f)) // Hack to prevent fishing bobber from failing to land on fishing hole
        { // but this is not proper, we really need to ignore not materialized objects
            SendCastResult(SPELL_FAILED_NOT_HERE);
            SendChannelUpdate(0);
            return;
        }

        // replace by water level in this case
        fz = cMap->GetWaterLevel(fx,fy);
    }
    // if gameobject is summoning object, it should be spawned right on caster's position
    else if(goinfo->type==GAMEOBJECT_TYPE_SUMMONING_RITUAL)
    {
        m_caster->GetPosition(fx,fy,fz);
    }

    GameObject* pGameObj = new GameObject;

    if(!pGameObj->Create(objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), name_id, cMap,
        fx, fy, fz, m_caster->GetOrientation(), 0, 0, 0, 0, 100, 1))
    {
        delete pGameObj;
        return;
    }

    int32 duration = GetSpellDuration(m_spellInfo);

    switch(goinfo->type)
    {
        case GAMEOBJECT_TYPE_FISHINGNODE:
        {
            m_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT,pGameObj->GetGUID());
                                                            // Orientation3
            pGameObj->SetFloatValue(GAMEOBJECT_ROTATION + 2, 0.88431775569915771 );
                                                            // Orientation4
            pGameObj->SetFloatValue(GAMEOBJECT_ROTATION + 3, -0.4668855369091033 );
            m_caster->AddGameObject(pGameObj);              // will removed at spell cancel

            // end time of range when possible catch fish (FISHING_BOBBER_READY_TIME..GetDuration(m_spellInfo))
            // start time == fish-FISHING_BOBBER_READY_TIME (0..GetDuration(m_spellInfo)-FISHING_BOBBER_READY_TIME)
            int32 lastSec;
            switch(urand(0, 3))
            {
                case 0: lastSec =  3; break;
                case 1: lastSec =  7; break;
                case 2: lastSec = 13; break;
                case 3: lastSec = 17; break;
            }

            duration = duration - lastSec*1000 + FISHING_BOBBER_READY_TIME*1000;
            break;
        }
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
        {
            if(m_caster->GetTypeId()==TYPEID_PLAYER)
            {
                pGameObj->AddUniqueUse((Player*)m_caster);
                m_caster->AddGameObject(pGameObj);          // will removed at spell cancel
            }
            break;
        }
        case GAMEOBJECT_TYPE_FISHINGHOLE:
        case GAMEOBJECT_TYPE_CHEST:
        default:
        {
            break;
        }
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration/1000 : 0);

    pGameObj->SetOwnerGUID(m_caster->GetGUID() );

    //pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel() );
    pGameObj->SetSpellId(m_spellInfo->Id);

    DEBUG_LOG("AddObject at SpellEfects.cpp EffectTransmitted\n");
    //m_caster->AddGameObject(pGameObj);
    //m_ObjToDel.push_back(pGameObj);

    cMap->Add(pGameObj);

    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
    data << uint64(pGameObj->GetGUID());
    m_caster->SendMessageToSet(&data,true);

    if(uint32 linkedEntry = pGameObj->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if(linkedGO->Create(objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), linkedEntry, cMap,
            fx, fy, fz, m_caster->GetOrientation(), 0, 0, 0, 0, 100, 1))
        {
            linkedGO->SetRespawnTime(duration > 0 ? duration/1000 : 0);
            //linkedGO->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel() );
            linkedGO->SetSpellId(m_spellInfo->Id);
            linkedGO->SetOwnerGUID(m_caster->GetGUID() );

            linkedGO->GetMap()->Add(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectSkill(uint32 /*i*/)
{
    sLog.outDebug("WORLD: SkillEFFECT");
}

void Spell::EffectSummonDemon(uint32 i)
{
    float px = m_targets.m_destX;
    float py = m_targets.m_destY;
    float pz = m_targets.m_destZ;

    Creature* Charmed = m_caster->SummonCreature(m_spellInfo->EffectMiscValue[i], px, py, pz, m_caster->GetOrientation(),TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,3600000);
    if (!Charmed)
        return;

    // might not always work correctly, maybe the creature that dies from CoD casts the effect on itself and is therefore the caster?
    Charmed->SetLevel(m_caster->getLevel());

    // TODO: Add damage/mana/hp according to level

    if (m_spellInfo->EffectMiscValue[i] == 89)              // Inferno summon
    {
        // Enslave demon effect, without mana cost and cooldown
        m_caster->CastSpell(Charmed, 20882, true);          // FIXME: enslave does not scale with level, level 62+ minions cannot be enslaved

        // Inferno effect
        Charmed->CastSpell(Charmed, 22703, true, 0);
    }
}

/* There is currently no need for this effect. We handle it in BattleGround.cpp
   If we would handle the resurrection here, the spiritguide would instantly disappear as the
   player revives, and so we wouldn't see the spirit heal visual effect on the npc.
   This is why we use a half sec delay between the visual effect and the resurrection itself */
void Spell::EffectSpiritHeal(uint32 /*i*/)
{
    /*
    if(!unitTarget || unitTarget->isAlive())
        return;
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if(!unitTarget->IsInWorld())
        return;

    //m_spellInfo->EffectBasePoints[i]; == 99 (percent?)
    //((Player*)unitTarget)->setResurrect(m_caster->GetGUID(), unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), unitTarget->GetMaxHealth(), unitTarget->GetMaxPower(POWER_MANA));
    ((Player*)unitTarget)->ResurrectPlayer(1.0f);
    ((Player*)unitTarget)->SpawnCorpseBones();
    */
}

// remove insignia spell effect
void Spell::EffectSkinPlayerCorpse(uint32 /*i*/)
{
    sLog.outDebug("Effect: SkinPlayerCorpse");
    if ( (m_caster->GetTypeId() != TYPEID_PLAYER) || (unitTarget->GetTypeId() != TYPEID_PLAYER) || (unitTarget->isAlive()) )
        return;

    ((Player*)unitTarget)->RemovedInsignia( (Player*)m_caster );
}

void Spell::EffectStealBeneficialBuff(uint32 i)
{
    sLog.outDebug("Effect: StealBeneficialBuff");

    if(!unitTarget || unitTarget==m_caster)                 // can't steal from self
        return;

    std::vector <Aura *> steal_list;
    // Create dispel mask by dispel type
    uint32 dispelMask  = GetDispellMask( DispelType(m_spellInfo->EffectMiscValue[i]) );
    Unit::AuraMap const& auras = unitTarget->GetAuras();
    for(Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura *aur = (*itr).second;
        if (aur && (1<<aur->GetSpellProto()->Dispel) & dispelMask)
        {
            // Need check for passive? this
            if (aur->IsPositive() && !aur->IsPassive())
                steal_list.push_back(aur);
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!steal_list.empty())
    {
        std::list < std::pair<uint32,uint64> > success_list;
        int32 list_size = steal_list.size();
        // dispel N = damage buffs (or while exist buffs for dispel)
        for (int32 count=0; count < damage && list_size > 0; ++count)
        {
            // Random select buff for dispel
            Aura *aur = steal_list[urand(0, list_size-1)];
            // Not use chance for steal
            // TODO possible need do it
            success_list.push_back( std::pair<uint32,uint64>(aur->GetId(),aur->GetCasterGUID()));

            // Remove buff from list for prevent doubles
            for (std::vector<Aura *>::iterator j = steal_list.begin(); j != steal_list.end(); )
            {
                Aura *stealed = *j;
                if (stealed->GetId() == aur->GetId() && stealed->GetCasterGUID() == aur->GetCasterGUID())
                {
                    j = steal_list.erase(j);
                    --list_size;
                }
                else
                    ++j;
            }
        }
        // Really try steal and send log
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLSTEALLOG, 8+8+4+1+4+count*5);
            data.append(unitTarget->GetPackGUID());  // Victim GUID
            data.append(m_caster->GetPackGUID());    // Caster GUID
            data << uint32(m_spellInfo->Id);         // dispel spell id
            data << uint8(0);                        // not used
            data << uint32(count);                   // count
            for (std::list<std::pair<uint32,uint64> >::iterator j = success_list.begin(); j != success_list.end(); ++j)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(j->first);
                data << uint32(spellInfo->Id);       // Spell Id
                data << uint8(0);                    // 0 - steals !=0 transfers
                unitTarget->RemoveAurasDueToSpellBySteal(spellInfo->Id, j->second, m_caster);
            }
            m_caster->SendMessageToSet(&data, true);
        }
    }
}

void Spell::EffectKillCredit(uint32 i)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->KilledMonster(m_spellInfo->EffectMiscValue[i], 0);
}

void Spell::EffectQuestFail(uint32 i)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->FailQuest(m_spellInfo->EffectMiscValue[i]);
}

void Spell::EffectRedirectThreat(uint32 /*i*/)
{
    if(unitTarget)
        m_caster->SetReducedThreatPercent((uint32)damage, unitTarget->GetGUID());
}

