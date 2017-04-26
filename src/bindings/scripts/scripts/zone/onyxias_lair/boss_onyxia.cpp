/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Boss_Onyxia
SD%Complete: 90
SDComment: Spell Heated Ground is wrong, flying animation, visual for area effect
SDCategory: Onyxia's Lair
EndScriptData */

#include "precompiled.h"
#include "instance_onyxia_lair.h"

enum
{
    SAY_AGGRO                   = -1249000,
    SAY_KILL                    = -1249001,
    SAY_PHASE_2_TRANS           = -1249002,
    SAY_PHASE_3_TRANS           = -1249003,
    EMOTE_BREATH                = -1249004,
    EMOTE_ROAR                  = -1249005,

    SPELL_WINGBUFFET            = 18500,
    SPELL_FLAMEBREATH           = 18435,
    SPELL_CLEAVE                = 19983,
    SPELL_TAILSWEEP             = 15847,
    SPELL_KNOCK_AWAY            = 19633,
    SPELL_BELLOWINGROAR         = 18431,
    SPELL_ERUPTION              = 17731,

    SPELL_FIREBALL              = 18392,
  //SPELL_ENGULFINGFLAMES       = 20019,
    SPELL_DEEPBREATH            = 23461,
    SPELL_HEATED_GROUND_EAST    = 22191,
    SPELL_HEATED_GROUND_WEST    = 22197,
    SPELL_HOVER                 = 17131,

    SPELL_BREATH_NORTH_TO_SOUTH = 17086,                    // 20x in "array"
    SPELL_BREATH_SOUTH_TO_NORTH = 18351,                    // 11x in "array"

    SPELL_BREATH_EAST_TO_WEST   = 18576,                    // 7x in "array"
    SPELL_BREATH_WEST_TO_EAST   = 18609,                    // 7x in "array"

    SPELL_BREATH_SE_TO_NW       = 18564,                    // 12x in "array"
    SPELL_BREATH_NW_TO_SE       = 18584,                    // 12x in "array"

    SPELL_BREATH_SW_TO_NE       = 18596,                    // 12x in "array"
    SPELL_BREATH_NE_TO_SW       = 18617,                    // 12x in "array"

  //SPELL_BREATH                = 21131,                    // 8x in "array", different initial cast than the other arrays
  //SPELL_SUMMONWHELP           = 17646,
        
    NPC_ONYXIAN_WHELP           = 11262,
    NPC_ONYXIAN_WARDER          = 12129,
    NPC_ERUPTION_TRIGGER        = 20009,

    GO_LAVATRAP_1               = 177984,
    GO_LAVATRAP_2               = 177985,
    GO_ONYXIA_DOOR                = 9091,

    DEPART_FLIGHT               = 20,
    LANDING_FLIGHT              = 21,

    PHASE_ONE                   = 1,
    PHASE_TWO                   = 2,
    PHASE_THREE                 = 3
};

static const float ONYXIA_AGGRO_RANGE  = 58.0f;

static const float ONYXIA_NORMAL_SPEED = 1.28571f;
static const float ONYXIA_BREATH_SPEED = 3.0f;

struct sOnyxMove
{
    uint32 uiLocId;
    uint32 uiLocIdEnd;
    uint32 uiSpellId;
    float fX, fY, fZ, fZGround;
};

static sOnyxMove aMoveData[] =
{
    {0, 4, SPELL_BREATH_NE_TO_SW,        10.2191f, -247.912f, -65.896f,  -85.84668f},  // north-east
    {1, 5, SPELL_BREATH_EAST_TO_WEST,   -31.4963f, -250.123f, -65.1278f, -89.127853f}, // east
    {2, 6, SPELL_BREATH_SE_TO_NW,       -63.5156f, -240.096f, -65.477f,  -85.066696f}, // south-east
    {3, 7, SPELL_BREATH_SOUTH_TO_NORTH, -65.8444f, -213.809f, -65.2985f, -84.298462f}, // south
    {4, 0, SPELL_BREATH_SW_TO_NE,       -58.2509f, -189.020f, -65.790f,  -85.292267f}, // south-west
    {5, 1, SPELL_BREATH_WEST_TO_EAST,   -33.5561f, -182.682f, -65.9457f, -88.945686f}, // west
    {6, 2, SPELL_BREATH_NW_TO_SE,         6.8951f, -180.246f, -65.896f,  -85.634293f}, // north-west
    {7, 3, SPELL_BREATH_NORTH_TO_SOUTH,  22.8763f, -217.152f, -65.0548f, -85.054054f}, // north
};

static float afSpawnLocations[2][3] =
{
    { -30.127f, -254.463f, -89.440f},
    { -30.817f, -177.106f, -89.258f}
};

struct TRINITY_DLL_DECL boss_onyxiaAI : public ScriptedAI
{
    boss_onyxiaAI(Creature* c) : ScriptedAI(c) {}

    uint32 m_uiPhase;
    uint32 m_uiTransTimer;
    uint32 m_uiTransCount;
    bool m_bTransition;

    uint32 m_uiFlameBreathTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiWingBuffetTimer;
    uint32 m_uiKnockAwayTimer;

    uint32 m_uiFireballTimer;
    uint32 m_uiMovementTimer;
    uint32 m_uiDeepBreathTimer;
    bool   m_bDeepBreathIsCasting;
    int    m_uiMovePoint;

    sOnyxMove* m_pPointData;

    uint32 m_uiSummonWhelpsTimer;
    uint32 m_uiWhelpTimer;
    uint8  m_uiSummonCount;
    uint8  m_uiWhelpsToSummon;
    bool   m_bIsSummoningWhelps;
    
    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiEruptTimer;
    bool   m_bEruptPhase;

    uint32 m_uiAggroRadiusTimer;
    uint32 m_uiLeashCheckTimer;
    uint32 m_uiSummonCheckTimer;

    std::list<GameObject*> GOListe;

    void Reset()
    {
        m_uiPhase              = 1;
        m_bTransition          = false;
        m_uiTransTimer         = 0;
        m_uiTransCount         = 0;

        m_uiFlameBreathTimer   = urand(10000, 20000);
        m_uiCleaveTimer        = urand(2000, 5000);
        m_uiWingBuffetTimer    = urand(10000, 20000);
        m_uiKnockAwayTimer     = urand(15000, 25000);
        m_uiTailSweepTimer     = 5000;

        m_uiFireballTimer      = 3000;
        m_uiMovementTimer      = 20000;
        m_uiMovePoint          = 7; // set North as the initial Phase 2 waypoint
        m_pPointData           = GetMoveData();
        m_uiDeepBreathTimer    = 0;
        m_bDeepBreathIsCasting = false;

        m_uiSummonWhelpsTimer  = 5000;
        m_uiWhelpTimer         = 1000;
        m_uiSummonCount        = 0;
        m_uiWhelpsToSummon     = 16;
        m_bIsSummoningWhelps   = false;

        m_uiBellowingRoarTimer = 10000;
        m_uiEruptTimer         = 0;
        
        m_uiAggroRadiusTimer   = 5000;
        m_uiLeashCheckTimer    = 5000;
        m_uiSummonCheckTimer   = 5000;

        if (GameObject *pGo = m_creature->GetMap()->GetGameObjectInMap(GO_ONYXIA_DOOR))
                            pGo->SetGoState(GO_STATE_ACTIVE);
        
        m_creature->SetSpeed(MOVE_RUN, ONYXIA_NORMAL_SPEED, true);
        m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 15.0f);
        m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 16.0f);

        // Daemon: remise en mode "dort"
        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
        m_creature->RemoveUnitMovementFlag(MOVEMENTFLAG_LEVITATING + MOVEMENTFLAG_ONTRANSPORT);
        m_creature->SetUnitMovementFlags(MOVEMENTFLAG_WALK_MODE);
    }

    void DelayEventIfNeed(uint32& event, uint32 delay)
    {
        if (event < (delay + 150)) // Time runs in increments of 150ms
            event = delay + 150;
    }

    void DelayCastEvents(uint32 delay)
    {
        DelayEventIfNeed(m_uiFlameBreathTimer, delay);
        DelayEventIfNeed(m_uiTailSweepTimer, delay);
        DelayEventIfNeed(m_uiCleaveTimer, delay);
        DelayEventIfNeed(m_uiWingBuffetTimer, delay);
        DelayEventIfNeed(m_uiKnockAwayTimer, delay);
    }
    
    void CheckForTargetsInAggroRadius(uint32 uiDiff)
    {
        // There is a grid-related bug preventing Onyxia from receiving MoveInLineOfSight calls from units in the front of her chamber, so use this check instead.
        if (m_uiAggroRadiusTimer < uiDiff)
            m_uiAggroRadiusTimer = 1000;
        else
        {
            m_uiAggroRadiusTimer -= uiDiff;
            return;
        }

        if (!m_creature->isInCombat() && !m_creature->IsInEvadeMode())
        {
            Map::PlayerList const& lPlayers = m_creature->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                if (Player* pPlayer = itr->getSource())
                {
                    if (m_creature->IsWithinDistInMap(pPlayer, ONYXIA_AGGRO_RANGE) && pPlayer->isTargetableForAttack())
                    {
                        pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                        m_creature->AI()->AttackStart(pPlayer);
                        m_creature->SetInCombatWithZone();
                        break;
                    }
                }
            }
        }
    }

    void LeashIfOutOfCombatArea(uint32 uiDiff)
    {
        if (m_uiLeashCheckTimer < uiDiff)
            m_uiLeashCheckTimer = 3500;
        else
        {
            m_uiLeashCheckTimer -= uiDiff;
            return;
        }

        if (m_creature->GetPositionX() < -95.0f)
            EnterEvadeMode();
    }

    void SummonPlayerIfOutOfReach(uint32 uiDiff)
    {
        if (m_uiSummonCheckTimer < uiDiff)
            m_uiSummonCheckTimer = 3000;
        else
        {
            m_uiSummonCheckTimer -= uiDiff;
            return;
        }

        /** Teleport victim to the center of the chamber if too far away from Onyxia */
        if (Unit* pVictim = m_creature->getVictim())
        {
            if (isOnyxiaFlying() && !m_creature->IsMoving()) 
            {
                if (pVictim->GetPositionX() < -105.0f)
                    pVictim->NearTeleportTo(-12.866907f, -216.626007f, -88.057808f, 0.0f);
            }
            else if (!isOnyxiaFlying())
            {
                if (m_creature->GetDistance2d(pVictim) > 90.0f /*|| !m_creature->GetCurrentMovementGenerator()->IsReachable()*/)
                    pVictim->NearTeleportTo(-12.866907f, -216.626007f, -88.057808f, 0.0f); 
            }
       } 
    }

    void Aggro(Unit* pWho)
    {
        // Daemon: Fix orientation.
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
        if (GameObject *pGo = m_creature->GetMap()->GetGameObjectInMap(GO_ONYXIA_DOOR))
                            pGo->SetGoState(GO_STATE_READY);

        std::list<Creature*> WarderList;
        /*
        GetCreatureListWithEntryInGrid(WarderList, m_creature, NPC_ONYXIAN_WARDER, 200.0f);
        for (std::list<Creature*>::iterator itr = WarderList.begin(); itr != WarderList.end(); ++itr)
            if (!(*itr)->isAlive())
                (*itr)->Respawn();
                */
    }

    void EnterEvadeMode() override
    {
        std::list<Creature*> WhelpList;
        /*
        GetCreatureListWithEntryInGrid(WhelpList, m_creature, NPC_ONYXIAN_WHELP, 200.0f);
        for (std::list<Creature*>::iterator itr = WhelpList.begin(); itr != WhelpList.end(); ++itr)            
            (*itr)->ForcedDespawn();
            */
        
        ScriptedAI::EnterEvadeMode();
    }
    
    void JustSummoned(Creature *pSummoned)
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);

        ++m_uiSummonCount;
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    bool isOnyxiaFlying()
    {
        return m_creature->HasAura(SPELL_HOVER,0);
    }
   
    sOnyxMove* GetMoveData()
    {
        uint32 uiMaxCount = sizeof(aMoveData) / sizeof(sOnyxMove);
        for (uint32 i = 0; i < uiMaxCount; ++i)
        {
            if (aMoveData[i].uiLocId == m_uiMovePoint)
                return &aMoveData[i];
        }

        return NULL;
    } 

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_BELLOWINGROAR)
        {

            m_uiEruptTimer = 500;
        }
    }
    
    void PhaseOne(uint32 uiDiff)
    {
        if (m_creature->isInFlight())
        {
            m_creature->SetUnitMovementFlags(MOVEMENTFLAG_WALK_MODE);
        }
        
        if (m_uiFlameBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAMEBREATH) == CAST_OK)
            {
                DelayCastEvents(2000); // 2sec de cast
                m_uiFlameBreathTimer = urand(10000, 20000);
            }
        }
        else
            m_uiFlameBreathTimer -= uiDiff;

        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(2000, 5000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiWingBuffetTimer < uiDiff)
        {
            if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WINGBUFFET) == CAST_OK)
                {
                    DelayCastEvents(1500);
                    m_uiWingBuffetTimer = urand(15000, 30000);
                }
            }
        }
        else
            m_uiWingBuffetTimer -= uiDiff;

        if (m_uiKnockAwayTimer < uiDiff)
        {
            if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY) == CAST_OK)
                {
                    if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                        m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -25);

                    DelayCastEvents(1500);
                    m_uiKnockAwayTimer = urand(15000, 30000);
                }
            }
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        if (m_uiTailSweepTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TAILSWEEP) == CAST_OK)
                m_uiTailSweepTimer = 3500;
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void PhaseTwo(uint32 uiDiff)
    {
        if (m_uiMovementTimer < uiDiff)
        {
            m_uiMovementTimer = urand(15000, 25000);
            m_uiFireballTimer = 5000;
            if (DoMovement())
            {
                // casting Deep Breath
                m_uiMovementTimer = urand(20000, 25000);
                m_uiFireballTimer = 10000;
            }
        }
        else
            m_uiMovementTimer -= uiDiff;

        if (m_bDeepBreathIsCasting)
        {
            if (m_uiDeepBreathTimer <= uiDiff)
            {
                m_creature->SetSpeed(MOVE_RUN, ONYXIA_BREATH_SPEED, true);
                m_creature->GetMotionMaster()->MovePoint(m_pPointData->uiLocId, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZ);
                // heat up egg pit during Deep Breath
                m_creature->CastSpell(m_creature, SPELL_HEATED_GROUND_EAST, true);
                m_creature->CastSpell(m_creature, SPELL_HEATED_GROUND_WEST, true);
                m_bDeepBreathIsCasting = false;
                m_uiDeepBreathTimer = 0;
            }
            else
                m_uiDeepBreathTimer -= uiDiff;
        }

        if (m_uiFireballTimer < uiDiff)
        {
            if (m_uiMovementTimer > 3500 && m_creature->IsStopped() && !m_bDeepBreathIsCasting)
            {
                if (Unit* pTarget = m_creature->getVictim())
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_FIREBALL) == CAST_OK)
                    {
                        if (m_creature->getThreatManager().getThreat(pTarget))
                            m_creature->getThreatManager().modifyThreatPercent(pTarget, -100);
                        m_uiFireballTimer = 3000;
                    }
                }
            }
        }
        else
            m_uiFireballTimer -= uiDiff;

        if (m_bIsSummoningWhelps)
        {
            if (m_uiSummonCount < m_uiWhelpsToSummon)
            {
                if (m_uiWhelpTimer < uiDiff)
                {
                    m_creature->SummonCreature(NPC_ONYXIAN_WHELP, afSpawnLocations[0][0], afSpawnLocations[0][1], afSpawnLocations[0][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                    m_creature->SummonCreature(NPC_ONYXIAN_WHELP, afSpawnLocations[1][0], afSpawnLocations[1][1], afSpawnLocations[1][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                    m_uiWhelpTimer = 1000;
                }
                else
                    m_uiWhelpTimer -= uiDiff;
            }
            else
            {
                m_bIsSummoningWhelps = false;
                m_uiSummonCount = 0;
                m_uiWhelpsToSummon = 5 + urand(0, 2);
                m_uiSummonWhelpsTimer = 30000;
            }
        }
        else
        {
            if (m_uiSummonWhelpsTimer < uiDiff)
                m_bIsSummoningWhelps = true;
            else
                m_uiSummonWhelpsTimer -= uiDiff;
        }
    }

    bool DoMovement()
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_pPointData = GetMoveData();

        switch (urand(0, 2))
        {
            case 0:     // Move clockwise
                m_uiMovePoint = (m_uiMovePoint + 1) % 8;
                break;
            case 1:     // Move counter-clockwise
                m_uiMovePoint = (m_uiMovePoint + 8 - 1) % 8;
                break;
            case 2:     // Deep Breath
                m_uiMovePoint = (m_uiMovePoint + 4) % 8;
                DoScriptText(EMOTE_BREATH, m_creature);
                
                m_bDeepBreathIsCasting = true;
                m_uiDeepBreathTimer = 5000;
                m_creature->CastSpell(m_creature, m_pPointData->uiSpellId, true);
                // face destination and clear target
                m_pPointData = GetMoveData();
                //m_creature->SetFacingTo(m_creature->GetAngle(m_pPointData->fX, m_pPointData->fY));
                m_creature->SetTargetGuid(m_creature->GetGUID()); 
                return true;
        }

        m_pPointData = GetMoveData();
        m_creature->SetSpeed(MOVE_RUN, ONYXIA_NORMAL_SPEED, true);
        m_creature->GetMotionMaster()->MovePoint(m_pPointData->uiLocId, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZ);
        return false;
    }

    void PhaseThree(uint32 uiDiff)
    {
        if (m_uiBellowingRoarTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BELLOWINGROAR) == CAST_OK)
            {
                DoScriptText(EMOTE_ROAR, m_creature);
                m_uiBellowingRoarTimer = urand(15000, 30000);
                // Do not be interrupted by other casts.
                DelayCastEvents(2000);
            }
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        if (m_uiSummonWhelpsTimer < uiDiff)
        {
            switch (urand(0, 1))
            {
                case 0: m_creature->SummonCreature(NPC_ONYXIAN_WHELP, afSpawnLocations[0][0], afSpawnLocations[0][1], afSpawnLocations[0][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 120000); break;
                case 1: m_creature->SummonCreature(NPC_ONYXIAN_WHELP, afSpawnLocations[1][0], afSpawnLocations[1][1], afSpawnLocations[1][2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 120000); break;
            }
            m_uiSummonWhelpsTimer = urand(1000, 10000);
        }
        else
            m_uiSummonWhelpsTimer -= uiDiff;
        
        PhaseOne(uiDiff);
    }

    bool Erupt()
    {
        uint32 Var = 0;
        while (Var < 10 && !GOListe.empty())
        {
            std::list<GameObject*>::iterator itr = GOListe.begin();
            std::advance(itr, rand() % GOListe.size());
            if (GameObject* GO = *itr)
            {
                GO->Use(m_creature);
                if (Creature* Cre = m_creature->SummonCreature(NPC_ERUPTION_TRIGGER, GO->GetPositionX(), GO->GetPositionY(), GO->GetPositionZ(), GO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 2000))
                {
                    Cre->CastSpell(Cre, SPELL_ERUPTION, true);
                    Cre->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    Cre->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Cre->AI()->EnterEvadeMode();
                }
                GOListe.erase(itr);
                Var++;
            }
        }

        return !GOListe.empty();
    }

    void PhaseTransition(uint32 uiDiff, bool bDebut)
    {
        //m_creature->CombatStop(true);
        m_creature->clearUnitState(UNIT_STAT_MELEE_ATTACKING);

        /** P2 Event to take off */
        if (m_uiPhase == PHASE_TWO)
        {
            /** Stop combat and move to the first waypoint before taking off */
            if (bDebut && m_uiTransCount == 0)
            {
                m_creature->AttackStop();
                DoScriptText(SAY_PHASE_2_TRANS, m_creature);
                m_creature->InterruptNonMeleeSpells(false);

                m_creature->AddUnitMovementFlag(MOVEMENTFLAG_LEVITATING + MOVEMENTFLAG_ONTRANSPORT);
                m_creature->SetHover(true);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();

                m_creature->SetSpeed(MOVE_RUN, ONYXIA_NORMAL_SPEED, true);
                m_creature->GetMotionMaster()->MovePoint(DEPART_FLIGHT, -57.750641f, -215.610077f, -85.094727f);
                m_creature->SetOrientation(0.0f);
                m_uiTransTimer = 1000;
                m_uiTransCount = 1;

            }
            /** Take off in progress */
            else if (m_uiTransTimer < uiDiff && m_uiTransCount == 1)
            {
                //m_creature->InterruptNonMeleeSpells(false);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->GetMotionMaster()->MovePoint(0, -57.204933f, -215.592148f, -85.156929f);
                m_uiTransTimer = 2000;
                m_uiTransCount = 2;
            }
            /** Fly mode added, move to the first waypoint */
            else if (m_uiTransTimer < uiDiff && m_uiTransCount == 2)
            {
                m_creature->CastSpell(m_creature, SPELL_HOVER, true); /** Start flying */
                m_bTransition = false;
                m_uiTransTimer = 0;
                float X = m_creature->GetPositionX() - m_pPointData->fX;
                float Y = m_creature->GetPositionY() - m_pPointData->fY;
                // increase Onyxia's hitbox while in the air to make it slightly easier for melee to use specials on her
                m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 21.0f);
                m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 22.0f);
                
                m_pPointData = GetMoveData();
                m_creature->GetMotionMaster()->MovePoint(m_pPointData->uiLocId, m_pPointData->fX, m_pPointData->fY, m_pPointData->fZ);
            }
        }
        /** P3 event to land */
        else if (m_uiPhase == PHASE_THREE)
        {
            /** Fly to the landing waypoint */
            if (bDebut && m_uiTransCount == 2)
            {
                DoResetThreat();
                DoScriptText(SAY_PHASE_3_TRANS, m_creature);
                m_creature->SetSpeed(MOVE_RUN, ONYXIA_NORMAL_SPEED, true);
                m_creature->InterruptNonMeleeSpells(false);

                if (m_creature->GetPositionX() < -40.0f)
                    m_creature->GetMotionMaster()->MovePoint(LANDING_FLIGHT, -59.895f, -214.876f, -84.855f); // South
                else
                    m_creature->GetMotionMaster()->MovePoint(LANDING_FLIGHT, -8.86f, -212.752f, -88.542f);   // North

                m_creature->RemoveAurasDueToSpell(SPELL_HOVER); /** Stop flying */
                m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 15.0f);
                m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 16.0f);
                m_uiTransTimer = 60000; // handled by MovementInform
            }
            /** Landing in progress */
            else if (m_uiTransTimer < uiDiff && m_uiTransCount == 3)
            {
                m_creature->GetMotionMaster()->MovePoint(0, -8.860f, -212.752f, -87.482f);
                m_uiTransTimer = 2000;
                m_uiTransCount = 4;
            }
            /** Landed. Restore target and start combat movement.*/
            else if (m_uiTransTimer < uiDiff && m_uiTransCount == 4)
            {               
                if (Unit* pVictim = m_creature->getVictim())
                    m_creature->SetTargetGuid(pVictim->GetGUID()); 

                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                m_bTransition  = false;
                m_uiTransTimer = 0;

                m_uiFlameBreathTimer   = urand(10000, 15000);
                m_uiTailSweepTimer     = 5000;
                m_uiCleaveTimer        = urand(2000, 5000);
                m_uiWingBuffetTimer    = urand(10000, 20000);
                m_uiKnockAwayTimer     = urand(10000, 20000);
            }
        }

        if (m_uiTransTimer >= uiDiff)
            m_uiTransTimer -= uiDiff;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        // restore Onyxia's target after movement in Phase 2
        if (uiPointId == m_pPointData->uiLocId)
        {
            if (Unit* pVictim = m_creature->getVictim())
                m_creature->SetTargetGuid(pVictim->GetGUID()); 
        }

        switch (uiPointId)
        {
            case DEPART_FLIGHT:
                m_creature->SetOrientation(0.0f);
                m_uiTransTimer = 1000;
                m_uiTransCount = 1;
                break;
            case LANDING_FLIGHT:
                m_creature->SetOrientation(0.0f);
                m_uiTransTimer = 1000;
                m_uiTransCount = 3;
                m_creature->RemoveUnitMovementFlag(MOVEMENTFLAG_LEVITATING + MOVEMENTFLAG_ONTRANSPORT);
                m_creature->SetUnitMovementFlags(MOVEMENTFLAG_WALK_MODE);
                m_creature->CastSpell(m_creature, SPELL_BELLOWINGROAR, true);
                m_uiBellowingRoarTimer = urand (15000, 30000);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        CheckForTargetsInAggroRadius(uiDiff);

        if (!m_creature->isInCombat() || !m_creature->SelectNearestTarget(60))
        {
            if (!UpdateVictim())
                return;
        }
        
        /** whenever Onyxia is moving to a waypoint or casting Deep Breath, clear her target */
        if (m_bTransition || m_bDeepBreathIsCasting || (m_uiPhase == PHASE_TWO && m_creature->IsMoving()))
            m_creature->SetTargetGuid(0); 

        if (m_uiEruptTimer)
        {
            // handle Erupt here because of mid-air fear cast during P3 transition
            if (m_uiEruptTimer <= uiDiff)
                m_uiEruptTimer = Erupt() ? 500 : 0;
            else
                m_uiEruptTimer -= uiDiff;
        }

        if (m_bTransition)
        {
            PhaseTransition(uiDiff, false);
            return;
        }
        
        LeashIfOutOfCombatArea(uiDiff);
        
        SummonPlayerIfOutOfReach(uiDiff); 

        /** Switch to P3 */
        if (m_creature->GetHealthPercent() < 40.0f && m_uiPhase == PHASE_TWO && !m_creature->IsMoving() && !m_bDeepBreathIsCasting)
        {
            m_uiPhase = PHASE_THREE;
            m_bTransition = true;
            PhaseTransition(0, true);
            return;
        }
        /** Switch to P2 */
        else if (m_creature->GetHealthPercent() < 65.0f && m_uiPhase == PHASE_ONE)
        {
            m_uiPhase = PHASE_TWO;
            m_bTransition = true;
            PhaseTransition(0, true);
            return;
        }

        switch (m_uiPhase)
        {
            case PHASE_ONE:
                PhaseOne(uiDiff);
                break;
            case PHASE_TWO:
                PhaseTwo(uiDiff);
                break;
            case PHASE_THREE:
                PhaseThree(uiDiff);
                break;
            default:
                break;
        }
    }
};

CreatureAI* GetAI_boss_onyxiaAI(Creature *_Creature)
{
    return new boss_onyxiaAI (_Creature);
}

void AddSC_boss_onyxia()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_onyxia";
    newscript->GetAI = &GetAI_boss_onyxiaAI;
    newscript->RegisterSelf();
}

