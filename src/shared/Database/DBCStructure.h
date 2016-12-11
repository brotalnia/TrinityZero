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

#ifndef DBCSTRUCTURE_H
#define DBCSTRUCTURE_H

#include "DBCEnums.h"
#include "Platform/Define.h"

#include <map>
#include <set>
#include <vector>

// Structures using to access raw DBC data and required packing to portability

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push,N), also any gcc version not support it at some platform
#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct AreaTableEntry
{
    uint32    ID;                                           // 0
    uint32    mapid;                                        // 1
    uint32    zone;                                         // 2 if 0 then it's zone, else it's zone id of this area
    uint32    exploreFlag;                                  // 3, main index
    uint32    flags;                                        // 4, unknown value but 312 for all cities
                                                            // 5-9 unused
    uint32    area_level;                                   // 10
    char*     area_name[8];                                 // 11-18 
    uint32    team;                                         // 20
};

struct AreaTriggerEntry
{
    uint32    id;                                           // 0
    uint32    mapid;                                        // 1
    float     x;                                            // 2
    float     y;                                            // 3
    float     z;                                            // 4
    float     radius;                                       // 5
    float     box_x;                                        // 6 extent x edge
    float     box_y;                                        // 7 extent y edge
    float     box_z;                                        // 8 extent z edge
    float     box_orientation;                              // 9 extent rotation by about z axis
};

struct AuctionHouseEntry
{
    uint32 houseId; // 0 index
    uint32 faction; // 1 id of faction.dbc for player factions associated with city
    uint32 depositPercent; // 2 1/3 from real
    uint32 cutPercent; // 3
    //char* name[16]; // 4-19
                                                            // 20 string flag, unused
};

struct BankBagSlotPricesEntry
{
    uint32      ID;
    uint32      price;
};


#define MAX_OUTFIT_ITEMS 12

struct CharStartOutfitEntry
{
    //uint32 Id;                                            // 0
    uint32 RaceClassGender;                                 // 1 (UNIT_FIELD_BYTES_0 & 0x00FFFFFF) comparable (0 byte = race, 1 byte = class, 2 byte = gender)
    int32 ItemId[MAX_OUTFIT_ITEMS];                         // 2-13
    //int32 ItemDisplayId[MAX_OUTFIT_ITEMS];                // 14-25 not required at server side
    //int32 ItemInventorySlot[MAX_OUTFIT_ITEMS];            // 26-37 not required at server side
    //uint32 Unknown1;                                      // 38, unique values (index-like with gaps ordered in other way as ids)
    //uint32 Unknown2;                                      // 39
    //uint32 Unknown3;                                      // 40
};


struct ChatChannelsEntry
{
    uint32      ChannelID;                                  // 0
    uint32      flags;                                      // 1
                                                            // 2 unused
    char*       pattern[8];                                 // 3-10
                                                            // 11 string flags, unused
    //char*       name[8];                                  // 12-19 unused
                                                            // 20 string flag, unused
};

struct ChrClassesEntry
 {
    uint32      ClassID;                                    // 0
                                                            // 1-2, unused
    uint32      powerType;                                  // 3
                                                            // 4
    char*       name[8];                                    // 5-12 used to detect DBC locale
    uint32      spellfamily;                                // 15
                                                            // 16 unused
};


struct ChrRacesEntry
{
    uint32      RaceID;                                     // 0
                                                            // 1 unused
    uint32      FactionID;                                  // 2 facton template id
                                                            // 3 unused
    uint32      model_m;                                    // 4
    uint32      model_f;                                    // 5 
                                                            // 6-7 unused
    uint32      TeamID;                                     // 8 (7-Alliance 1-Horde)
    uint32      startingTaxiMask;                           
    uint32      startmovie;                                 
    char*       name[8];                                    // 17-25 used for DBC language detection/selection
};

struct CreatureDisplayInfoEntry
{
    uint32      Displayid;                                  // 0
                                                            // 1    iRefID_CreatureModelData Points to the model filename.
                                                            // 3    iRefID_CreatureDisplayInfoExtra    
    float       scale;                                      // 4    Default model scale if not set by server for that spawn
                                                            // 6        Opacity     Integer     0 - 255. 0 is totally transparent, 255 is solid    
};

struct CreatureFamilyEntry
{
    uint32    ID;                                           // 0
    float     minScale;                                     // 1
    uint32    minScaleLevel;                                // 2 0/1
    float     maxScale;                                     // 3
    uint32    maxScaleLevel;                                // 4 0/60
    uint32    skillLine[2];                                 // 5-6
    uint32    petFoodMask;                                  // 7
    char*     Name[8];                                      // 8-23
                                                            // 24 string flags, unused
                                                            // 25 icon, unused
};

struct CreatureSpellDataEntry
{
    uint32    ID;                                           // 0
    //uint32    spellId[4];                                 // 1-4 hunter pet learned spell (for later use)
};



struct DurabilityCostsEntry
{
    uint32    Itemlvl;                                      // 0
    uint32    multiplier[29];                               // 1-29
};

struct DurabilityQualityEntry
{
    uint32    Id;                                           // 0
    float     quality_mod;                                  // 1
};

struct EmotesEntry
{
    uint32  Id;                                             // 0
    //char*   Name;                                         // 1, internal name
    //uint32  AnimationId;                                  // 2, ref to animationData
    uint32  Flags;                                          // 3, bitmask, may be unit_flags
    uint32  EmoteType;                                      // 4, Can be 0, 1 or 2 (determine how emote are shown)
    uint32  UnitStandState;                                 // 5, uncomfirmed, may be enum UnitStandStateType
    //uint32  SoundId;                                      // 6, ref to soundEntries
};

struct EmotesTextEntry
{
    uint32    Id;
    uint32    textid;
};

struct FactionEntry
{
    uint32      ID;                                         // 0
    int32       reputationListID;                           // 1
    uint32      BaseRepRaceMask[4];                         // 2-5 Base reputation race masks (see enum Races)
    uint32      BaseRepClassMask[4];                        // 6-9 Base reputation class masks (see enum Classes)
    int32       BaseRepValue[4];                            // 10-13 Base reputation values
    uint32      ReputationFlags[4];                         // 14-17 Default flags to apply
    uint32      team;                                       // 18 enum Team
    char*       name[8];                                    // 19-26
                                                            // 27 string flags, unused
};

struct FactionTemplateEntry
{
    uint32      ID;                                         // 0
    uint32      faction;                                    // 1
    uint32      factionFlags;                               // 2 specific flags for that faction
    uint32      ourMask;                                    // 3 if mask set (see FactionMasks) then faction included in masked team
    uint32      friendlyMask;                               // 4 if mask set (see FactionMasks) then faction friendly to masked team
    uint32      hostileMask;                                // 5 if mask set (see FactionMasks) then faction hostile to masked team
    uint32      enemyFaction1;                              // 6
    uint32      enemyFaction2;                              // 7
    uint32      enemyFaction3;                              // 8
    uint32      enemyFaction4;                              // 9
    uint32      friendFaction1;                             // 10
    uint32      friendFaction2;                             // 11
    uint32      friendFaction3;                             // 12
    uint32      friendFaction4;                             // 13
    //-------------------------------------------------------  end structure

    // helpers
    bool IsFriendlyTo(FactionTemplateEntry const& entry) const
    {
        if(ID == entry.ID)
            return true;
        if(enemyFaction1  == entry.faction || enemyFaction2  == entry.faction || enemyFaction3 == entry.faction || enemyFaction4 == entry.faction )
            return false;
        if(friendFaction1 == entry.faction || friendFaction2 == entry.faction || friendFaction3 == entry.faction || friendFaction4 == entry.faction )
            return true;
        return (friendlyMask & entry.ourMask) || (ourMask & entry.friendlyMask);
    }
    bool IsHostileTo(FactionTemplateEntry const& entry) const
    {
        if(ID == entry.ID)
            return false;
        if(enemyFaction1  == entry.faction || enemyFaction2  == entry.faction || enemyFaction3 == entry.faction || enemyFaction4 == entry.faction )
            return true;
        if(friendFaction1 == entry.faction || friendFaction2 == entry.faction || friendFaction3 == entry.faction || friendFaction4 == entry.faction )
            return false;
        return (hostileMask & entry.ourMask) != 0;
    }
    bool IsHostileToPlayers() const { return (hostileMask & FACTION_MASK_PLAYER) !=0; }
    bool IsNeutralToAll() const { return hostileMask == 0 && friendlyMask == 0 && enemyFaction1==0 && enemyFaction2==0 && enemyFaction3==0 && enemyFaction4==0; }
    bool IsContestedGuardFaction() const { return (factionFlags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD)!=0; }
};

#define GT_MAX_LEVEL    100

struct ItemDisplayInfoEntry
{
    uint32      ID;
    uint32      randomPropertyChance;
};

struct ItemRandomPropertiesEntry
{
    uint32    ID;                                           // 0
    //char*     internalName                                // 1   unused
    uint32    enchant_id[3];                                // 2-4
                                                            // 5-6 unused, 0 only values, reserved for additional enchantments?
    //char*     nameSuffix[8]                               // 7-15, unused
};

struct ItemSetEntry
{
    //uint32    id                                          // 0 item set ID
    char*     name[8];                                      // 1-8
                                                            // 9 string flags, unused
                                                            // 10-20 items from set, but not have all items listed, use ItemPrototype::ItemSet instead
                                                            // 21-26 unused
    uint32    spells[8];                                    // 37-34
    uint32    items_to_triggerspell[8];                     // 35-42
    uint32    required_skill_id;                            // 43
    uint32    required_skill_value;                         // 44
};

struct LockEntry
{
    uint32      ID;                                         // 0
    uint32      keytype[5];                                 // 1-5
                                                            // 6-8, not used
    uint32      key[5];                                     // 9-13
                                                            // 14-16, not used
    uint32      requiredminingskill;                        // 17
    uint32      requiredlockskill;                          // 18
                                                            // 19-32, not used
};

struct MailTemplateEntry
{
    uint32      ID;                                         // 0
    //char*       subject[8];                               // 1-8
                                                            // 9 name flags, unused
};

struct MapEntry
{
    uint32      MapID;                                      // 0
    //char*       internalname;                             // 1 unused
    uint32      map_type;                                   // 2
                                                            // 3 unused
    char*       name[8];                                    // 4-11
                                                            // 12 name flags, unused
                                                            // 13-15 unused (something PvPZone related - levels?)
                                                            // 16-18
    uint32      linked_zone;                                // 19 common zone for instance and continent map
    //char*     hordeIntro                                  // 20-27 text for PvP Zones
                                                            // 28 intro text flags
    //char*     allianceIntro                               // 29-36 text for PvP Zones
                                                            // 37 intro text flags
    uint32      multimap_id;                                // 38
                                                            // 39-42 unused
    // Helpers


    bool IsDungeon() const { return map_type == MAP_INSTANCE || map_type == MAP_RAID; }
    bool Instanceable() const { return map_type == MAP_INSTANCE || map_type == MAP_RAID || map_type == MAP_BATTLEGROUND; }
    bool IsRaid() const { return map_type == MAP_RAID; }
    bool IsBattleGround() const { return map_type == MAP_BATTLEGROUND; }
    bool HasResetTime() const { return false; /* resetTimeRaid; */}

    bool IsMountAllowed() const
    {
        return !IsDungeon() ||
            MapID==568 || MapID==309 || MapID==209 || MapID==534 ||
            MapID==560 || MapID==509 || MapID==269;
    }
};

struct QuestSortEntry
{
    uint32      id;                                         // 0, sort id
    //char*       name[16];                                 // 1-16, unused
                                                            // 17 name flags, unused
};

//struct SkillLineCategoryEntry{
//    uint32    id;                                           // 0 hidden key
//    char*     name[16];                                     // 1 - 17 Category name
//                                                                  // 18 string flag
//    uint32    displayOrder;                                 // Display order in character tab
//};

//struct SkillRaceClassInfoEntry{
//    uint32    id;                                           // 0
//    uint32    skillId;                                      // 1 present some refrences to unknown skill
//    uint32    raceMask;                                     // 2
//    uint32    classMask;                                    // 3
//    uint32    flags;                                        // 4 mask for some thing
//    uint32    reqLevel;                                     // 5
//    uint32    skillTierId;                                  // 6
//    uint32    skillCostID;                                  // 7
//};

//struct SkillTiersEntry{
//    uint32    id;                                           // 0
//    uint32    skillValue[16];                               // 1-17 unknown possibly add value on learn?
//    uint32    maxSkillValue[16];                            // Max value for rank
//};

struct SkillLineEntry
{
    uint32    id;                                           // 0
    uint32    categoryId;                                   // 1 (index from SkillLineCategory.dbc)
    //uint32    skillCostID;                                // 2 not used
    char*     name[8];                                      // 3-10
                                                            // 11 string flags, not used
    //char*     description[8];                             // 12-19, not used
                                                            // 20 string flags, not used
    uint32    spellIcon;                                    // 21
};

struct SkillLineAbilityEntry
{
    uint32    id;                                           // 0, INDEX
    uint32    skillId;                                      // 1
    uint32    spellId;                                      // 2
    uint32    racemask;                                     // 3
    uint32    classmask;                                    // 4
    //uint32    racemaskNot;                                // 5 always 0 in 2.4.2
    //uint32    classmaskNot;                               // 6 always 0 in 2.4.2
    uint32    req_skill_value;                              // 7 for trade skill.not for training.
    uint32    forward_spellid;                              // 8
    uint32    learnOnGetSkill;                              // 9 can be 1 or 2 for spells learned on get skill
    uint32    max_value;                                    // 10
    uint32    min_value;                                    // 11
                                                            // 12-13, unknown, always 0
    uint32    reqtrainpoints;                               // 14
};

struct SoundEntriesEntry
{
    uint32    Id;                                           // 0, sound id
    //uint32    Type;                                       // 1, sound type (10 generally for creature, etc)
    //char*     InternalName;                               // 2, internal name, for use in lookup command for example
    //char*     FileName[10];                               // 3-12, file names
    //uint32    Unk13[10];                                  // 13-22, linked with file names?
    //char*     Path;                                       // 23
                                                            // 24-28, unknown
};

struct _SpellEntry
{
    uint32    Id;                                           // 0 normally counted from 0 field (but some tools start counting from 1, check this before tool use for data view!)
    uint32    School;                                       // 1 not schoolMask from 2.x - just school type so everything linked with SpellEntry::SchoolMask must be rewrited
    uint32    Category;                                     // 2
    // uint32 castUI;                                       // 3 not used
    uint32    Dispel;                                       // 4
    uint32    Mechanic;                                     // 5
    uint32    Attributes;                                   // 6
    uint32    AttributesEx;                                 // 7
    uint32    AttributesEx2;                                // 8
    uint32    AttributesEx3;                                // 9
    uint32    AttributesEx4;                                // 10
    uint32    Stances;                                      // 11
    uint32    StancesNot;                                   // 12
    uint32    Targets;                                      // 13
    uint32    TargetCreatureType;                           // 14
    uint32    RequiresSpellFocus;                           // 15
    uint32    CasterAuraState;                              // 16
    uint32    TargetAuraState;                              // 17
    uint32    CastingTimeIndex;                             // 18
    uint32    RecoveryTime;                                 // 19
    uint32    CategoryRecoveryTime;                         // 20
    uint32    InterruptFlags;                               // 21
    uint32    AuraInterruptFlags;                           // 22
    uint32    ChannelInterruptFlags;                        // 23
    uint32    procFlags;                                    // 24
    uint32    procChance;                                   // 25
    uint32    procCharges;                                  // 26
    uint32    maxLevel;                                     // 27
    uint32    baseLevel;                                    // 28
    uint32    spellLevel;                                   // 29
    uint32    DurationIndex;                                // 30
    uint32    powerType;                                    // 31
    uint32    manaCost;                                     // 32
    uint32    manaCostPerlevel;                             // 33
    uint32    manaPerSecond;                                // 34
    uint32    manaPerSecondPerLevel;                        // 35
    uint32    rangeIndex;                                   // 36
    float     speed;                                        // 37
    uint32    modalNextSpell;                             // 38 not used
    uint32    StackAmount;                                  // 39
    uint32    Totem[2];                                     // 40-41
    int32     Reagent[8];                                   // 42-49
    uint32    ReagentCount[8];                              // 50-57
    int32     EquippedItemClass;                            // 58 (value)
    int32     EquippedItemSubClassMask;                     // 59 (mask)
    int32     EquippedItemInventoryTypeMask;                // 60 (mask)
    uint32    Effect[3];                                    // 61-63
    int32     EffectDieSides[3];                            // 64-66
    uint32    EffectBaseDice[3];                            // 67-69
    float     EffectDicePerLevel[3];                        // 70-72
    float     EffectRealPointsPerLevel[3];                  // 73-75
    int32     EffectBasePoints[3];                          // 76-78 (don't must be used in spell/auras explicitly, must be used cached Spell::m_currentBasePoints)
    uint32    EffectMechanic[3];                            // 79-81
    uint32    EffectImplicitTargetA[3];                     // 82-84
    uint32    EffectImplicitTargetB[3];                     // 85-87
    uint32    EffectRadiusIndex[3];                         // 88-90 - spellradius.dbc
    uint32    EffectApplyAuraName[3];                       // 91-93
    uint32    EffectAmplitude[3];                           // 94-96
    float     EffectMultipleValue[3];                       // 97-99
    uint32    EffectChainTarget[3];                         // 100-102
    uint32    EffectItemType[3];                            // 103-105
    int32     EffectMiscValue[3];                           // 106-108
//    int32     EffectMiscValueB[3];                          // missing in 1.12?
    uint32    EffectTriggerSpell[3];                        // 109-111
    float     EffectPointsPerComboPoint[3];                 // 112-114
    uint32    SpellVisual;                                  // 115
                                                            // 116 not used
    uint32    SpellIconID;                                  // 117
    uint32    activeIconID;                                 // 118
    //uint32    spellPriority;                              // 119
    char*     SpellName[8];                                 // 120-127
    //uint32    SpellNameFlag;                              // 128
    char*     Rank[8];                                      // 129-136
    //uint32    RankFlags;                                  // 137
    //char*     Description[8];                             // 138-145 not used
    //uint32    DescriptionFlags;                           // 146     not used
    //char*     ToolTip[8];                                 // 147-154 not used
    //uint32    ToolTipFlags;                               // 155     not used
    uint32    ManaCostPercentage;                           // 156
    uint32    StartRecoveryCategory;                        // 157
    uint32    StartRecoveryTime;                            // 158
    uint32    MaxTargetLevel;                               // 159
    uint32    SpellFamilyName;                              // 160
    uint64    SpellFamilyFlags;                             // 161+162
    uint32    MaxAffectedTargets;                           // 163
    uint32    DmgClass;                                     // 164 defenseType
    uint32    PreventionType;                               // 165
    //uint32    StanceBarOrder;                             // 166 not used
    float     DmgMultiplier[3];                             // 167-169
    //uint32    MinFactionId;                               // 170 not used, and 0 in 2.4.2
    //uint32    MinReputation;                              // 171 not used, and 0 in 2.4.2
    //uint32    RequiredAuraVision;                         // 172 not used

    private:
        // prevent creating custom entries (copy data from original in fact)
        _SpellEntry(_SpellEntry const&);                      // DON'T must have implementation
};


struct SpellEntry
{
    uint32    Id;                                           // 0 normally counted from 0 field (but some tools start counting from 1, check this before tool use for data view!)
    uint32    School;                                       // 1 not schoolMask from 2.x - just school type so everything linked with SpellEntry::SchoolMask must be rewrited
    uint32    Category;                                     // 2
    // uint32 castUI;                                       // 3 not used
    uint32    Dispel;                                       // 4
    uint32    Mechanic;                                     // 5
    uint32    Attributes;                                   // 6
    uint32    AttributesEx;                                 // 7
    uint32    AttributesEx2;                                // 8
    uint32    AttributesEx3;                                // 9
    uint32    AttributesEx4;                                // 10
    uint32    Stances;                                      // 11
    uint32    StancesNot;                                   // 12
    uint32    Targets;                                      // 13
    uint32    TargetCreatureType;                           // 14
    uint32    RequiresSpellFocus;                           // 15
    //uint32    CasterAuraState;                            // 16 not used in SpellEntry (we are using info from sql imported from 2.4.3 spell.dbc)
    //uint32    TargetAuraState;                            // 17 not used in SpellEntry (we are using info from sql imported from 2.4.3 spell.dbc)
    uint32    CastingTimeIndex;                             // 18
    uint32    RecoveryTime;                                 // 19
    uint32    CategoryRecoveryTime;                         // 20
    uint32    InterruptFlags;                               // 21
    uint32    AuraInterruptFlags;                           // 22
    uint32    ChannelInterruptFlags;                        // 23
    uint32    procFlags;                                    // 24
    uint32    procChance;                                   // 25
    uint32    procCharges;                                  // 26
    uint32    maxLevel;                                     // 27
    uint32    baseLevel;                                    // 28
    uint32    spellLevel;                                   // 29
    uint32    DurationIndex;                                // 30
    uint32    powerType;                                    // 31
    uint32    manaCost;                                     // 32
    uint32    manaCostPerlevel;                             // 33
    uint32    manaPerSecond;                                // 34
    uint32    manaPerSecondPerLevel;                        // 35
    uint32    rangeIndex;                                   // 36
    float     speed;                                        // 37
    uint32    modalNextSpell;                             // 38 not used
    uint32    StackAmount;                                  // 39
    uint32    Totem[2];                                     // 40-41
    int32     Reagent[8];                                   // 42-49
    uint32    ReagentCount[8];                              // 50-57
    int32     EquippedItemClass;                            // 58 (value)
    int32     EquippedItemSubClassMask;                     // 59 (mask)
    int32     EquippedItemInventoryTypeMask;                // 60 (mask)
    uint32    Effect[3];                                    // 61-63
    int32     EffectDieSides[3];                            // 64-66
    uint32    EffectBaseDice[3];                            // 67-69
    float     EffectDicePerLevel[3];                        // 70-72
    float     EffectRealPointsPerLevel[3];                  // 73-75
    int32     EffectBasePoints[3];                          // 76-78 (don't must be used in spell/auras explicitly, must be used cached Spell::m_currentBasePoints)
    uint32    EffectMechanic[3];                            // 79-81
    uint32    EffectImplicitTargetA[3];                     // 82-84
    uint32    EffectImplicitTargetB[3];                     // 85-87
    uint32    EffectRadiusIndex[3];                         // 88-90 - spellradius.dbc
    uint32    EffectApplyAuraName[3];                       // 91-93
    uint32    EffectAmplitude[3];                           // 94-96
    float     EffectMultipleValue[3];                       // 97-99
    uint32    EffectChainTarget[3];                         // 100-102
    uint32    EffectItemType[3];                            // 103-105
    int32     EffectMiscValue[3];                           // 106-108
//    int32     EffectMiscValueB[3];                          // missing in 1.12?
    uint32    EffectTriggerSpell[3];                        // 109-111
    float     EffectPointsPerComboPoint[3];                 // 112-114
    uint32    SpellVisual;                                  // 115
                                                            // 116 not used
    uint32    SpellIconID;                                  // 117
    uint32    activeIconID;                                 // 118
    //uint32    spellPriority;                              // 119
    char*     SpellName[8];                                 // 120-127
    //uint32    SpellNameFlag;                              // 128
    char*     Rank[8];                                      // 129-136
    //uint32    RankFlags;                                  // 137
    //char*     Description[8];                             // 138-145 not used
    //uint32    DescriptionFlags;                           // 146     not used
    //char*     ToolTip[8];                                 // 147-154 not used
    //uint32    ToolTipFlags;                               // 155     not used
    uint32    ManaCostPercentage;                           // 156
    uint32    StartRecoveryCategory;                        // 157
    uint32    StartRecoveryTime;                            // 158
    uint32    MaxTargetLevel;                               // 159
    uint32    SpellFamilyName;                              // 160
    uint64    SpellFamilyFlags;                             // 161+162
    uint32    MaxAffectedTargets;                           // 163
    uint32    DmgClass;                                     // 164 defenseType
    uint32    PreventionType;                               // 165
    //uint32    StanceBarOrder;                             // 166 not used
    float     DmgMultiplier[3];                             // 167-169
    //uint32    MinFactionId;                               // 170 not used, and 0 in 2.4.2
    //uint32    MinReputation;                              // 171 not used, and 0 in 2.4.2
    //uint32    RequiredAuraVision;                         // 172 not used

    private:
        // prevent creating custom entries (copy data from original in fact)
        SpellEntry(SpellEntry const&);                      // DON'T must have implementation
};

typedef std::set<uint32> SpellCategorySet;
typedef std::map<uint32,SpellCategorySet > SpellCategoryStore;
typedef std::set<uint32> PetFamilySpellsSet;
typedef std::map<uint32,PetFamilySpellsSet > PetFamilySpellsStore;

struct SpellCastTimesEntry
{
    uint32    ID;                                           // 0
    int32     CastTime;                                     // 1
    //float     CastTimePerLevel;                           // 2 unsure / per skill?
    //int32     MinCastTime;                                // 3 unsure
};

struct SpellFocusObjectEntry
{
    uint32    ID;                                           // 0
    //char*     Name[8];                                    // 1-8 unused
                                                            // 9 string flags, unused
};

// stored in SQL table
struct SpellThreatEntry
{
    uint32      spellId;
    int32       threat;
};

struct SpellRadiusEntry
{
    uint32    ID;
    float     radiusHostile;
    float     radiusFriend;
};

struct SpellRangeEntry
{
    uint32    ID;
    float     minRange;
    float     maxRange;
    uint32    type;
};

struct SpellShapeshiftEntry
{
    uint32 ID;                                              // 0
    //uint32 buttonPosition;                                // 1 unused
    //char*  Name[8];                                       // 2-9 unused
    //uint32 NameFlags;                                     // 10 unused
    uint32 flags1;                                          // 11
                                                            // 12 unused
    //uint32 unk1;                                          // 13 unused

    //int32  creatureType;                                    // TODO: remove this for 1.12
    //uint32 attackSpeed;                                     // TODO: remove this for 1.12
};

struct SpellDurationEntry
{
    uint32    ID;
    int32     Duration[3];
};

struct SpellItemEnchantmentEntry
{
    uint32      ID;                                         // 0
    uint32      type[3];                                    // 1-3
    uint32      amount[3];                                  // 4-6
    //uint32    amount2[3]                                  // 7-9 always same as similar `amount` value
    uint32      spellid[3];                                 // 10-12
    char*       description[8];                             // 13-20
                                                            // 21 description flags
    uint32      aura_id;                                    // 22
    uint32      slot;                                       // 23
    //uint32      GemID;                                      // not exists in 1.12
    //uint32      EnchantmentCondition;                       // not exists in 1.12 ?
};

struct StableSlotPricesEntry
{
    uint32 Slot;
    uint32 Price;
};

struct TalentEntry
{
    uint32    TalentID;                                     // 0
    uint32    TalentTab;                                    // 1 index in TalentTab.dbc (TalentTabEntry)
    uint32    Row;                                          // 2
    uint32    Col;                                          // 3
    uint32    RankID[5];                                    // 4-8
                                                            // 9-12 not used, always 0, maybe not used high ranks
    uint32    DependsOn;                                    // 13 index in Talent.dbc (TalentEntry)
                                                            // 14-15 not used
    uint32    DependsOnRank;                                // 16
                                                            // 17-19 not used
    uint32    DependsOnSpell;                               // 20 req.spell
};

struct TalentTabEntry
{
    uint32    TalentTabID;                                  // 0
    //char*   name[8];                                      // 1-8, unused
    //uint32  nameFlags;                                    // 9, unused
    //unit32  spellicon;                                    // 10
                                                            // 11 not used
    uint32    ClassMask;                                    // 12
    uint32    tabpage;                                      // 13
    //char*   internalname;                                 // 14
};

struct TaxiNodesEntry
{
    uint32    ID;                                           // 0
    uint32    map_id;                                       // 1
    float     x;                                            // 2
    float     y;                                            // 3
    float     z;                                            // 4
    char*     name[8];                                      // 5-12
                                                            // 13 string flags, unused
    uint32    horde_mount_type;                             // 14
    uint32    alliance_mount_type;                          // 15
};

struct TaxiPathEntry
{
    uint32    ID;
    uint32    from;
    uint32    to;
    uint32    price;
};

struct TaxiPathNodeEntry
{
    uint32    path;
    uint32    index;
    uint32    mapid;
    float     x;
    float     y;
    float     z;
    uint32    actionFlag;
    uint32    delay;
};

struct WorldMapAreaEntry
{
    //uint32    ID;                                         // 0
    uint32    map_id;                                       // 1
    uint32    area_id;                                      // 2 index (continent 0 areas ignored)
    //char*   internal_name                                 // 3
    float     y1;                                           // 4
    float     y2;                                           // 5
    float     x1;                                           // 6
    float     x2;                                           // 7
    //not exists in 1.12 int32   virtual_map_id;            // 8 -1 (map_id have correct map) other: virtual map where zone show (map_id - where zone in fact internally)
};

struct WorldSafeLocsEntry
{
    uint32    ID;                                           // 0
    uint32    map_id;                                       // 1
    float     x;                                            // 2
    float     y;                                            // 3
    float     z;                                            // 4
    //char*   name[8]                                       // 5-12 name, unused
                                                            // 13 name flags, unused
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

// Structures not used for casting to loaded DBC data and not required then packing
struct TalentSpellPos
{
    TalentSpellPos() : talent_id(0), rank(0) {}
    TalentSpellPos(uint16 _talent_id, uint8 _rank) : talent_id(_talent_id), rank(_rank) {}

    uint16 talent_id;
    uint8  rank;
};

typedef std::map<uint32,TalentSpellPos> TalentSpellPosMap;

struct TaxiPathBySourceAndDestination
{
    TaxiPathBySourceAndDestination() : ID(0),price(0) {}
    TaxiPathBySourceAndDestination(uint32 _id,uint32 _price) : ID(_id),price(_price) {}

    uint32    ID;
    uint32    price;
};
typedef std::map<uint32,TaxiPathBySourceAndDestination> TaxiPathSetForSource;
typedef std::map<uint32,TaxiPathSetForSource> TaxiPathSetBySource;

struct TaxiPathNode
{
    TaxiPathNode() : mapid(0), x(0),y(0),z(0),actionFlag(0),delay(0) {}
    TaxiPathNode(uint32 _mapid, float _x, float _y, float _z, uint32 _actionFlag, uint32 _delay) : mapid(_mapid), x(_x),y(_y),z(_z),actionFlag(_actionFlag),delay(_delay) {}

    uint32    mapid;
    float     x;
    float     y;
    float     z;
    uint32    actionFlag;
    uint32    delay;
};
typedef std::vector<TaxiPathNode> TaxiPathNodeList;
typedef std::vector<TaxiPathNodeList> TaxiPathNodesByPath;

#define TaxiMaskSize 8
typedef uint32 TaxiMask[TaxiMaskSize];
#endif

