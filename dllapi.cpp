/*
 * Copyright (c) 2001-2006 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include <extdll.h>

#include <vector>

#include <detours.h>
#include <dllapi.h>
#include <meta_api.h>
#include <log_meta.h>
#include "usercmd.h"

#include "signatures.h"
#include "share_obj.h"

#include "dlldef.h"

#define CALL_ANGELSCRIPT(pfn, ...) if (ASEXT_CallHook){(*ASEXT_CallHook)(g_AngelHook.pfn, 0, __VA_ARGS__);}
cvar_t* g_pCVarUnlag = nullptr;

bool g_HookedFlag = false;
void ServerActivate (edict_t* pEdictList, int edictCount, int clientMax) {
	CEntityObject::ClearGameObject();
	if (g_HookedFlag) {
		SET_META_RESULT(MRES_IGNORED);
		return;
	}
	g_HookedFlag = true;
	g_pCVarUnlag = CVAR_GET_POINTER("sv_unlag");
	SET_META_RESULT(MRES_HANDLED);
}
int AllowLagCompensation() {
	SET_META_RESULT(MRES_SUPERCEDE);
	return g_pCVarUnlag->value > 0 ? 1 : 0;
}
void EndFrame() {
//大概有400ms
#define MAX_RECORD 40
//0 World
//1-33 Players
	for (int i = 34; i < gpGlobals->maxEntities; i++){
		edict_t* ent = INDEXENT(i);
		if (ent == nullptr)
			continue;
		CEntityObject* obj = CEntityObject::GetGameObject(i);
		//entity freed?
		if (ent->free){
			CEntityObject::RemoveGameObject(i);
			continue;
		}
		entvars_t* vars = VARS(ent);
		if ((vars->deadflag > 0) || (vars->movetype = MOVETYPE_NONE) || (vars->solid == SOLID_BSP))
			continue;
		std::shared_ptr<entityinfo_t> lagInfo = std::make_shared<entityinfo_t>();
		lagInfo->info.Angles = vars->angles;
		lagInfo->info.AnimTime = vars->animtime;
		lagInfo->info.Frame = vars->frame;
		lagInfo->info.FrameRate = vars->framerate;
		lagInfo->info.GaitSequence = vars->gaitsequence;
		lagInfo->info.Origin = vars->origin;
		lagInfo->info.Sequence = vars->sequence;
		lagInfo->time = g_engfuncs.pfnTime();
		obj->aryLagInfo.push_back(lagInfo);
		if (obj->aryLagInfo.size() > MAX_RECORD) {
			obj->aryLagInfo.pop_front();
		}
	}
	SET_META_RESULT(MRES_HANDLED);
#undef MAX_RECORD
}
void SetEntityLagState(edict_t* ent, entitylaginfo_t* lag) {
	SET_ORIGIN(ent, lag->Origin);
	entvars_t* vars = VARS(ent);
	vars->angles = lag->Angles;
	vars->animtime = lag->AnimTime;
	vars->frame = lag->Frame;
	vars->framerate = lag->FrameRate;
	vars->gaitsequence = lag->GaitSequence;
	vars->sequence = lag->Sequence;
}
void CmdStart(const edict_t* player, const struct usercmd_s* cmd, unsigned int random_seed) {
	if (player != nullptr && g_pCVarUnlag->value > 0) {
		int ping, loss;
		g_engfuncs.pfnGetPlayerStats(player, &ping, &loss);
		float flRecallTime = g_engfuncs.pfnTime() - ((float)ping / 1000.0f);
		for (int i = 34; i < gpGlobals->maxEntities; i++) {
			edict_t* ent = INDEXENT(i);
			if (ent == nullptr)
				continue;
			if (ent->free)
				continue;
			if ((ent->v.deadflag > 0) || (ent->v.movetype = MOVETYPE_NONE) || (ent->v.solid == SOLID_BSP))
				continue;
			CEntityObject* obj = CEntityObject::GetGameObject(i);
			if (obj != nullptr && obj->aryLagInfo.size() > 0) {
				entvars_t* vars = VARS(ent);
				auto pLastInfo = CEntityObject::GetLastLagInfo(i);
				pLastInfo->Angles = vars->angles;
				pLastInfo->AnimTime = vars->animtime;
				pLastInfo->Frame = vars->frame;
				pLastInfo->FrameRate = vars->framerate;
				pLastInfo->GaitSequence = vars->gaitsequence;
				pLastInfo->Origin = vars->origin;
				pLastInfo->Sequence = vars->sequence;
				for (auto iter = obj->aryLagInfo.rbegin(); iter != obj->aryLagInfo.rend(); iter++) {
					if ((*iter)->time <= flRecallTime) {
						SetEntityLagState(ent, &(*iter)->info);
					}
				}
			}
		}
		SET_META_RESULT(MRES_HANDLED);
		return;
	}
	SET_META_RESULT(MRES_IGNORED);
}
void CmdEnd(const edict_t* player) {
	if (player != nullptr && g_pCVarUnlag->value > 0) {
		for (int i = 34; i < gpGlobals->maxEntities; i++) {
			edict_t* ent = INDEXENT(i);
			if (ent == nullptr)
				continue;
			if (ent->free)
				continue;
			if ((ent->v.deadflag > 0) || (ent->v.movetype = MOVETYPE_NONE) || (ent->v.solid == SOLID_BSP))
				continue;
			CEntityObject* obj = CEntityObject::GetGameObject(i);
			if (obj != nullptr && obj->aryLagInfo.size() > 0)
				SetEntityLagState(ent, CEntityObject::GetLastLagInfo(i));
		}
		SET_META_RESULT(MRES_HANDLED);
		return;
	}
	SET_META_RESULT(MRES_IGNORED);
}
void GameInit() {
	CEntityObject::InitGameObject();
}

static DLL_FUNCTIONS gFunctionTable = {
	GameInit,					// pfnGameInit
	NULL,					// pfnSpawn
	NULL,					// pfnThink
	NULL,					// pfnUse
	NULL,				// pfnTouch
	NULL,				// pfnBlocked
	NULL,					// pfnKeyValue
	NULL,					// pfnSave
	NULL,					// pfnRestore
	NULL,					// pfnSetAbsBox

	NULL,					// pfnSaveWriteFields
	NULL,					// pfnSaveReadFields

	NULL,					// pfnSaveGlobalState
	NULL,					// pfnRestoreGlobalState
	NULL,					// pfnResetGlobalState

	NULL,					// pfnClientConnect
	NULL,					// pfnClientDisconnect
	NULL,					// pfnClientKill
	NULL,					// pfnClientPutInServer
	NULL,					// pfnClientCommand
	NULL,					// pfnClientUserInfoChanged
	ServerActivate,					// pfnServerActivate
	NULL,					// pfnServerDeactivate

	NULL,					// pfnPlayerPreThink
	NULL,					// pfnPlayerPostThink

	NULL,					// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink

	NULL,					// pfnSys_Error

	NULL,					// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType

	NULL,					// pfnSetupVisibility
	NULL,					// pfnUpdateClientData
	NULL,					// pfnAddToFullPack
	NULL,					// pfnCreateBaseline
	NULL,					// pfnRegisterEncoders
	NULL,					// pfnGetWeaponData
	CmdStart,					// pfnCmdStart
	CmdEnd,					// pfnCmdEnd
	NULL,					// pfnConnectionlessPacket
	NULL,					// pfnGetHullBounds
	NULL,					// pfnCreateInstancedBaselines
	NULL,					// pfnInconsistentFile
	AllowLagCompensation,					// pfnAllowLagCompensation
};
C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS* pFunctionTable,
	int* interfaceVersion){
	if (!pFunctionTable) {
		UTIL_LogPrintf("GetEntityAPI2 called with null pFunctionTable");
		return(FALSE);
	}
	else if (*interfaceVersion != INTERFACE_VERSION) {
		UTIL_LogPrintf("GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}
	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));
	return(TRUE);
}

static DLL_FUNCTIONS gFunctionTable_Post = {
	NULL,					// pfnGameInit
	NULL,					// pfnSpawn
	NULL,					// pfnThink
	NULL,					// pfnUse
	NULL,				// pfnTouch
	NULL,				// pfnBlocked
	NULL,					// pfnKeyValue
	NULL,					// pfnSave
	NULL,					// pfnRestore
	NULL,					// pfnSetAbsBox

	NULL,					// pfnSaveWriteFields
	NULL,					// pfnSaveReadFields

	NULL,					// pfnSaveGlobalState
	NULL,					// pfnRestoreGlobalState
	NULL,					// pfnResetGlobalState

	NULL,					// pfnClientConnect
	NULL,					// pfnClientDisconnect
	NULL,					// pfnClientKill
	NULL,					// pfnClientPutInServer
	NULL,					// pfnClientCommand
	NULL,					// pfnClientUserInfoChanged
	NULL,					// pfnServerActivate
	NULL,					// pfnServerDeactivate

	NULL,					// pfnPlayerPreThink
	NULL,					// pfnPlayerPostThink

	NULL,					// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink

	NULL,					// pfnSys_Error

	NULL,					// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType

	NULL,					// pfnSetupVisibility
	NULL,					// pfnUpdateClientData
	NULL,					// pfnAddToFullPack
	NULL,					// pfnCreateBaseline
	NULL,					// pfnRegisterEncoders
	NULL,					// pfnGetWeaponData
	NULL,					// pfnCmdStart
	NULL,					// pfnCmdEnd
	NULL,					// pfnConnectionlessPacket
	NULL,					// pfnGetHullBounds
	NULL,					// pfnCreateInstancedBaselines
	NULL,					// pfnInconsistentFile
	NULL,					// pfnAllowLagCompensation
};
C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS* pFunctionTable,
	int* interfaceVersion) {
	if(!pFunctionTable) {
		UTIL_LogPrintf("GetEntityAPI2 called with null pFunctionTable");
		return(FALSE);
	}
	else if (*interfaceVersion != INTERFACE_VERSION) {
		UTIL_LogPrintf("GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}
	memcpy(pFunctionTable, &gFunctionTable_Post, sizeof(DLL_FUNCTIONS));
	return(TRUE);
}