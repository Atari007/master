/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_rage_winterchill
SD%Complete: 
SDComment: 
SDCategory: Caverns of Time, Mount Hyjal
SDAuthor: Dexter, <http://excalibur-wow.com>
EndScriptData */

#include "precompiled.h"
#include "hyjal.h"

enum
{
	//Spells
	SPELL_ICEBOLT			= 31249,
	SPELL_DEATH_AND_DECAY	= 39658,
	SPELL_FROST_NOVA		= 31250,
	SPELL_FROST_ARMOR		= 31256,
	SPELL_BERSERK			= 26662,

	//Texts
	YELL_AGGRO				= -1999930,
	YELL_FROST1				= -1999931,
	YELL_FROST2				= -1999932,
	YELL_DND1				= -1999933,
	YELL_DND2				= -1999934,
	YELL_KILL1				= -1999935,
	YELL_KILL2				= -1999936,
	YELL_KILL3				= -1999937,
	YELL_DEATH				= -1999938
};


struct MANGOS_DLL_DECL boss_rage_winterchillAI : public ScriptedAI
{
    boss_rage_winterchillAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
	ScriptedInstance* m_pInstance;

    uint32 m_uiIceboltTimer;
    uint32 m_uiFrostNovaTimer;
    uint32 m_uiDeathAndDecayTimer;
	uint32 m_uiFrostArmorTimer;
    uint32 m_uiBerserkTimer;

	void Reset()
    {
		m_uiIceboltTimer = 9000;
		m_uiFrostNovaTimer = 15000;
		m_uiDeathAndDecayTimer = 30000;
		m_uiFrostArmorTimer = 11000;
		m_uiBerserkTimer = 600000;
	}

	void JustReachedHome()
	{
		if(m_pInstance)
		{
			if(m_pInstance->GetData(TYPE_WINTERCHILL) != FAIL)
				m_pInstance->SetData(TYPE_WINTERCHILL, FAIL);
		}
	}


	void Aggro(Unit* pWho)
    {
        if(pWho->GetTypeId() != TYPEID_PLAYER)
			return;

		DoScriptText(YELL_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_WINTERCHILL, IN_PROGRESS);
    }

	void KilledUnit(Unit* pVictim)
    {
		if (pVictim->GetTypeId() != TYPEID_PLAYER)
           return;

        switch(urand(0, 2))
        {
            case 0: DoScriptText(YELL_KILL1, m_creature); break;
            case 1: DoScriptText(YELL_KILL2, m_creature); break;
            case 2: DoScriptText(YELL_KILL3, m_creature); break;
        }
    }
 


	void JustDied(Unit *victim)
    {
		DoScriptText(YELL_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_WINTERCHILL, DONE);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		if(m_uiBerserkTimer < uiDiff)
		{
			DoCast(m_creature, SPELL_BERSERK);
			m_uiBerserkTimer = 60000;
		}
		else m_uiBerserkTimer -= uiDiff;

		if(m_uiFrostArmorTimer < uiDiff)
		{
			DoCast(m_creature, SPELL_FROST_ARMOR);
			m_uiFrostArmorTimer = urand(10000,11000);
		}
		else m_uiFrostArmorTimer -= uiDiff;

		if(m_uiIceboltTimer < uiDiff)
		{
			switch(urand(0, 1))
			{
            case 0: DoScriptText(YELL_FROST1, m_creature); break;
            case 1: DoScriptText(YELL_FROST2, m_creature); break;
			}
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				m_creature->CastSpell(pTarget, SPELL_ICEBOLT, false);
			m_uiIceboltTimer = urand(8000,9000);
		}
		else m_uiIceboltTimer -= uiDiff;

		if(m_uiDeathAndDecayTimer < uiDiff)
		{
			switch(urand(0, 1))
			{
            case 0: DoScriptText(YELL_DND1, m_creature); break;
            case 1: DoScriptText(YELL_DND2, m_creature); break;
			}
			m_creature->InterruptNonMeleeSpells(true);
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				m_creature->CastSpell(pTarget, SPELL_DEATH_AND_DECAY, false);
			m_uiDeathAndDecayTimer = urand(25000,30000);
		}
		else m_uiDeathAndDecayTimer -= uiDiff;

		if(m_uiFrostNovaTimer < uiDiff)
		{
			switch(urand(0, 1))
			{
            case 0: DoScriptText(YELL_FROST1, m_creature); break;
            case 1: DoScriptText(YELL_FROST2, m_creature); break;
			}
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				m_creature->CastSpell(pTarget, SPELL_FROST_NOVA, false);
			m_uiFrostNovaTimer = urand(14000,15000);
		}
		else m_uiFrostNovaTimer -= uiDiff;

	DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rage_winterchill(Creature* pCreature)
{
    return new boss_rage_winterchillAI(pCreature);
}

void AddSC_boss_rage_winterchill()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_rage_winterchill";
    pNewScript->GetAI = &GetAI_boss_rage_winterchill;
    pNewScript->RegisterSelf();
}