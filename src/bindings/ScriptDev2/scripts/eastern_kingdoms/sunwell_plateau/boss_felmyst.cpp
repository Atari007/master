/* Copyright (C) 2006 - 2008 ScriptDev2 <http://www.scriptdev2.com/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
 
/* ScriptData
SDName: boss_felmyst
SD%Complete:
SDComment:
SDCategory: Sunwell Plateau
SDAuthor: Dexter, <http://excalibur-wow.com>
EndScriptData */

#include "precompiled.h"
#include "sunwell_plateau.h"
 
enum
{
    YELL_REBIRTH        = -1580036,
    YELL_SLAY1          = -1580037,
    YELL_SLAY2          = -1580038,
    YELL_DEATH          = -1580042,
    YELL_TAKEOFF        = -1580040,
    YELL_BREATH         = -1580039,
    YELL_BERSERK        = -1580041,
    YELL_KALECGOS       = -1580043,

    //Mics
    SPELL_NOXIOUSFUMES			= 47002,
	SPELL_ENRAGE                = 26662,
 
    //Land Phase
    SPELL_CLEAVE                = 19983, 
    SPELL_CORROSION             = 45866,
    SPELL_GASNOVA               = 45855,
	SPELL_ENCAPSULATE			= 45665,
    SPELL_ENCAPSULATE_CHANNEL   = 45661,
   
    //Flight Phase
	SPELL_DEMONIC_VAPOR_TRIGG	= 45399,	
    SPELL_FOG_OF_CORRUPTION		= 45582,
	SPELL_FOG_CHARM             = 45717,
	SPELL_SOUL_SEVER			= 45917,
	SPELL_SUMMON_VAPOR			= 45391,
	SPELL_SUMMON_VAPOR_TRIAL	= 45410,
	SPELL_VAPOR_BEAM			= 45389,

	// npcs
    NPC_UNYIELDING_DEAD          = 25268,
 
    NPC_FELMYST_VISUAL			= 25041,
    NPC_FLIGHT_LEFT				= 25357,
    NPC_FLIGHT_RIGHT			= 25358,
 
	// vapors
    NPC_VAPOR					= 25265,
    NPC_VAPOR_TRAIL				= 25267,

	// fog
	SPELL_DEATH_COULD_VISUAL	= 45212,	// not used
	SPELL_SUMMON_DEATH_CLOUD	= 45884,
    NPC_DEATH_CLOUD				= 25703,

	// transforms
	SPELL_TRANSFORM_TRIGGER         = 44885,   // madrigosa to self, trigger 46350
    SPELL_TRANSFORM_FELMYST         = 45068,   // become fel

	// phases
	PHASE_IDLE			= 0,
	PHASE_GROUND		= 1,
	PHASE_AIR			= 2,
};

//Positional defines 
struct LocationsXY
{
    float x, y, z;
    uint32 id;
};

// Movement coordinates
static LocationsXY MoveLoc[]=
{
	{1489.307f, 701.451f, 40.5f},	// right - 0
	{1447.672f, 649.917f, 40.5f},
	{1430.092f, 604.318f, 40.5f},	// center - 2
	{1462.899f, 536.334f, 40.5f},
	{1537.197f, 522.199f, 40.5f},	// left - 4
};
 
/*######
## boss_felmyst
######*/
struct MANGOS_DLL_DECL boss_felmystAI : public ScriptedAI
{
    boss_felmystAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
		m_bHasChecked = false;       
        Reset();
    }

    ScriptedInstance* m_pInstance;

	uint32 m_uiPhase;
	uint32 m_uiEnrageTimer;
	uint32 m_uiMovemetnTimer;
 
    //Ground Phase
    uint32 m_uiFlyPhaseTimer;
    uint32 m_uiCorrosionTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiEncapsulateTimer;
    uint32 m_uiGasNovaTimer;
 
    //FlyPhase Timers
	uint32 m_uiBreathCount;
	uint32 m_uiLastPointId;
    uint32 m_uiDemonicVaporTimer;
	uint32 m_uiVaporSummonTimer;
	uint32 m_uiVaporCount;
    uint32 m_uiFogOfCorruptionTimer;
	uint32 m_uiFogSummonTimer;
	ObjectGuid m_uiVaporTargetGUID;
	bool m_bIsBreath;
	bool m_bHasChecked;

    uint32 m_uiEncounterCheckTimer;
 
    void Reset()
    {
		m_uiPhase				= PHASE_IDLE;
        m_uiEnrageTimer         = 600000;
		m_uiMovemetnTimer		= 11000;
		
		//Ground Phase
		m_uiCorrosionTimer      = 10000;
		m_uiCleaveTimer         = 4000;
		m_uiGasNovaTimer        = 20000;
		m_uiEncapsulateTimer    = 15000; 
        m_uiFlyPhaseTimer       = 60000;
        
        //FlyPhase Timers
        m_uiFogOfCorruptionTimer= 30000;
		m_uiFogSummonTimer		= 1000;
        m_uiDemonicVaporTimer	= 10000;
		m_uiVaporSummonTimer	= 1000;
		m_uiVaporCount			= 0;
        m_uiBreathCount			= 0;
		m_bIsBreath				= false;
		m_uiVaporTargetGUID.Clear();

        m_uiEncounterCheckTimer = 10000;
    }

	void JustReachedHome()
	{
		if(m_pInstance)
			m_pInstance->SetData(TYPE_FELMYST, NOT_STARTED);
	}
 
    void Aggro(Unit *who)
    { 
		m_uiPhase = PHASE_GROUND;
		m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
		m_creature->SetLevitate(false);
		DoCast(m_creature, SPELL_NOXIOUSFUMES);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, IN_PROGRESS);
    }

	void AttackStart(Unit* pWho)
    {
		if(m_pInstance->GetData(TYPE_BRUTALLUS) != DONE)
			return;

        if(!pWho)
            return;

        if (m_creature->Attack(pWho, true)) 
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
        }
    }
 
    void KilledUnit(Unit* victim)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(YELL_SLAY1, m_creature); break;
            case 1: DoScriptText(YELL_SLAY2, m_creature); break;
        }
    }
 
    void JustDied(Unit* Killer)
    {
        DoScriptText(YELL_DEATH, m_creature);

       if(Creature* pKalecgos = m_creature->SummonCreature(NPC_KALECGOS, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 30, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000))
            DoScriptText(YELL_KALECGOS, pKalecgos);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, DONE);
    }

	void MovementInform(uint32 uiType, uint32 uiPointId)
	{
		if(uiType != POINT_MOTION_TYPE)
			return;

		if(uiPointId == 1 && m_uiPhase == PHASE_IDLE)
			m_uiMovemetnTimer = 300;

		if(uiPointId == 1 && m_uiPhase == PHASE_AIR)
		{
			m_bIsBreath = false;
			m_uiFogOfCorruptionTimer = 5000;
		}
	}
 
    void UpdateAI(const uint32 uiDiff)
    {
		if(m_uiPhase == PHASE_IDLE)
		{
			// make boss visible after brutallus is defeated
			if(m_pInstance && m_pInstance->GetData(TYPE_BRUTALLUS) == DONE && m_pInstance->GetData(TYPE_FELMYST) == NOT_STARTED)
			{
				if(m_uiEncounterCheckTimer < uiDiff && !m_bHasChecked)
				{
					m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					DoScriptText(YELL_REBIRTH, m_creature);
					DoCast(m_creature, SPELL_TRANSFORM_FELMYST);
					m_bHasChecked = true;
					// start flying
					m_creature->SetLevitate(true);
					m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
					m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
					m_uiLastPointId = urand(0, 4);
					m_creature->GetMotionMaster()->MovePoint(1, MoveLoc[m_uiLastPointId].x, MoveLoc[m_uiLastPointId].y, MoveLoc[m_uiLastPointId].z);
					m_uiMovemetnTimer = 60000;
				}
				else m_uiEncounterCheckTimer -= uiDiff;
			}

			if(m_uiMovemetnTimer < uiDiff && m_bHasChecked)
			{
				m_creature->SetLevitate(true);
				m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
				m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
				// movement
				if(m_uiLastPointId < 2)
					m_uiLastPointId = urand(2, 4);
				else if(m_uiLastPointId > 2)
					m_uiLastPointId = urand(0, 2);
				else while(m_uiLastPointId == 2)
					m_uiLastPointId = urand(0, 4);
				m_creature->GetMotionMaster()->MovePoint(1, MoveLoc[m_uiLastPointId].x, MoveLoc[m_uiLastPointId].y, MoveLoc[m_uiLastPointId].z);
				m_uiMovemetnTimer = 60000;
			}
			else m_uiMovemetnTimer -= uiDiff;
		}
 
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiEnrageTimer < uiDiff)
        {
            DoScriptText(YELL_BERSERK, m_creature);
            DoCast(m_creature, SPELL_ENRAGE);
            m_uiEnrageTimer = 30000;
        }
		else m_uiEnrageTimer -= uiDiff;

		// Ground
		if(m_uiPhase == PHASE_GROUND)
		{
			if(m_uiCleaveTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_CLEAVE);
                m_uiCleaveTimer = urand(4000, 6000);
            }
			else m_uiCleaveTimer -= uiDiff;

			if(m_uiCorrosionTimer < uiDiff)
            {
				if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
					DoCast(pTarget, SPELL_CORROSION);
				m_uiCorrosionTimer = urand(15000, 20000);
			}
			else m_uiCorrosionTimer -= uiDiff;

			if(m_uiGasNovaTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_GASNOVA);
                m_uiGasNovaTimer = urand(3000, 35000);
            }
			else m_uiGasNovaTimer -= uiDiff;

			if(m_uiEncapsulateTimer < uiDiff)
            {
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                {
					DoScriptText(-1580107,m_creature,pTarget);
					pTarget->CastSpell(pTarget, SPELL_ENCAPSULATE, false);
                    DoCast(pTarget, SPELL_ENCAPSULATE_CHANNEL);
                }
                m_uiEncapsulateTimer = urand(30000,40000);
            }
			else m_uiEncapsulateTimer -= uiDiff;

			if(m_uiFlyPhaseTimer < uiDiff)
            {
                m_creature->GetMotionMaster()->Clear();
				SetCombatMovement(false);
				m_creature->SetLevitate(true);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
				m_uiLastPointId = urand(0, 4);
				m_creature->GetMotionMaster()->MovePoint(0, MoveLoc[m_uiLastPointId].x, MoveLoc[m_uiLastPointId].y, MoveLoc[m_uiLastPointId].z);
                DoScriptText(YELL_TAKEOFF, m_creature);
				m_uiPhase					= PHASE_AIR;
                m_uiDemonicVaporTimer		= 10000;
                m_uiFogOfCorruptionTimer    = 35000;
                m_uiBreathCount				= 0;
				m_uiVaporCount				= 0;
            }
			else m_uiFlyPhaseTimer -= uiDiff;
 
            DoMeleeAttackIfReady();
		}
		// Air phase
		else if(m_uiPhase == PHASE_AIR)
		{
			if(m_uiFogOfCorruptionTimer < uiDiff)
			{
				++m_uiBreathCount;
				if(m_uiBreathCount == 4)
				{
					SetCombatMovement(true);
					m_creature->GetMotionMaster()->Clear();
					m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
					m_creature->SetLevitate(false);
					m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
					m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
					m_uiPhase = PHASE_GROUND;
					m_uiFlyPhaseTimer = 60000;
					return;
				}
				else
				{
					DoScriptText(YELL_BREATH, m_creature);
					m_uiFogSummonTimer = 1000;
					m_uiFogOfCorruptionTimer = 60000;
					m_bIsBreath	= true;
					// movement
					if(m_uiLastPointId < 2)
						m_uiLastPointId = urand(2, 4);
					else if(m_uiLastPointId > 2)
						m_uiLastPointId = urand(0, 2);
					else while(m_uiLastPointId == 2)
						m_uiLastPointId = urand(0, 4);
					m_creature->GetMotionMaster()->MovePoint(1, MoveLoc[m_uiLastPointId].x, MoveLoc[m_uiLastPointId].y, MoveLoc[m_uiLastPointId].z);
				}
			}
			else m_uiFogOfCorruptionTimer -= uiDiff;

			if(m_uiFogSummonTimer < uiDiff && m_bIsBreath)
			{
				m_creature->SummonCreature(NPC_DEATH_CLOUD, m_creature->GetPositionX(), m_creature->GetPositionY(), 25.5f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
				m_uiFogSummonTimer = 3000;
			}
			else m_uiFogSummonTimer -= uiDiff;

			if(m_uiDemonicVaporTimer < uiDiff && m_uiVaporCount < 1)
			{
				if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				{
					m_uiVaporTargetGUID = pTarget->GetObjectGuid();
					m_creature->SummonCreature(NPC_VAPOR, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
				}
				m_uiVaporSummonTimer = 2000;
				m_uiDemonicVaporTimer = 7000;
				++m_uiVaporCount;
			}
			else m_uiDemonicVaporTimer -= uiDiff;

			if(m_uiVaporSummonTimer < uiDiff && m_uiVaporCount < 1)
			{
				if(Unit* pTarget = m_creature->GetMap()->GetUnit(m_uiVaporTargetGUID))
					m_creature->SummonCreature(NPC_VAPOR, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
				m_uiVaporSummonTimer = 2000;
			}
			else m_uiVaporSummonTimer -= uiDiff;
		}            
    }
};
 
/*######
## mob_fog_of_corruption
######*/
struct MANGOS_DLL_DECL mob_fog_of_corruptionAI : public Scripted_NoMovementAI
{
    mob_fog_of_corruptionAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		pCreature->SetDisplayId(11686);     // make invisible
		SetCombatMovement(false);
		pCreature->setFaction(14);
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
 
    void Reset()
    {
		DoCast(m_creature, SPELL_FOG_OF_CORRUPTION);
    }

	void AttackStart(Unit* pWho)
	{
		return;
	}

	void MoveInLineOfSight(Unit* pWho)
	{
		if(pWho->GetTypeId() == TYPEID_PLAYER && !pWho->HasAura(SPELL_FOG_CHARM, EFFECT_INDEX_0) && pWho->IsWithinDist2d(m_creature->GetPositionX(), m_creature->GetPositionY(), 20.0f))
		{
			pWho->CastSpell(pWho, SPELL_FOG_CHARM, false);
			pWho->CastSpell(pWho, SPELL_SOUL_SEVER, false);
		}
	}

    void UpdateAI(const uint32 diff)
    {
    }
};
 
/*######
## mob_felmyst_vapor
######*/
struct MANGOS_DLL_DECL mob_felmyst_vaporAI : public Scripted_NoMovementAI
{
    mob_felmyst_vaporAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		pCreature->SetDisplayId(11686);     // make invisible
		SetCombatMovement(false);
		pCreature->setFaction(14);
        Reset();
    }
 
    ScriptedInstance* m_pInstance;
    uint32 m_uiCheckTimer;
	uint32 m_uiSummonDelayTimer;
	bool m_bCanSummon;
 
    void Reset()
    { 
        DoCast(m_creature, SPELL_DEMONIC_VAPOR_TRIGG);
		m_uiCheckTimer			= 5000;
		m_uiSummonDelayTimer	= 5000;
		m_bCanSummon			= false;
    }

	void MoveInLineOfSight(Unit* pWho)
	{
		if(pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDist2d(m_creature->GetPositionX(), m_creature->GetPositionY(), 2.0f) && m_bCanSummon)
		{
			m_creature->SummonCreature(NPC_UNYIELDING_DEAD, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
			m_uiSummonDelayTimer = 1000;
			m_bCanSummon		 = false;
		}
	}

	void AttackStart(Unit* pWho)
	{
		return;
	}

	void JustSummoned(Creature* pSummon)
	{
		pSummon->SetInCombatWithZone();
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if(m_uiCheckTimer < uiDiff)
		{
			for(uint8 i = 0; i < 3; i++)
				m_creature->SummonCreature(NPC_UNYIELDING_DEAD, m_creature->GetPositionX() + urand(0, 5), m_creature->GetPositionY() + urand(0, 5), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
			m_bCanSummon = true;
			m_uiCheckTimer = 60000;
		}
		else m_uiCheckTimer -= uiDiff;

        if(m_uiSummonDelayTimer < uiDiff && !m_bCanSummon)
			m_bCanSummon = true;
		else m_uiSummonDelayTimer -= uiDiff;
    }
};
 
CreatureAI* GetAI_mob_fog_of_corruption(Creature* pCreature)
{
    return new mob_fog_of_corruptionAI(pCreature);
}
 
CreatureAI* GetAI_mob_felmyst_vapor(Creature* pCreature)
{
    return new mob_felmyst_vaporAI(pCreature);
}
 
CreatureAI* GetAI_boss_felmyst(Creature *pCreature)
{
    return new boss_felmystAI(pCreature);
}
 
void AddSC_boss_felmyst()
{
    Script *pNewScript;
 
    pNewScript = new Script;
    pNewScript->Name = "boss_felmyst";
    pNewScript->GetAI = &GetAI_boss_felmyst;
    pNewScript->RegisterSelf();
 
    pNewScript = new Script;
    pNewScript->Name="mob_felmyst_vapor";
    pNewScript->GetAI = &GetAI_mob_felmyst_vapor;
    pNewScript->RegisterSelf();
 
    pNewScript = new Script;
    pNewScript->Name = "mob_fog_of_corruption";
    pNewScript->GetAI = &GetAI_mob_fog_of_corruption;
    pNewScript->RegisterSelf();
}
