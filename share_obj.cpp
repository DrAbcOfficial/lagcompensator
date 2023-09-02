#include <extdll.h>
#include <meta_api.h>
#include "share_obj.h"

CEntityObject* g_EntityObjs;
entitylaginfo_t* g_LastInfos;

void CEntityObject::InitGameObject() {
	g_EntityObjs = new CEntityObject[gpGlobals->maxEntities];
	g_LastInfos = new entitylaginfo_t[gpGlobals->maxEntities];
}
void CEntityObject::ClearGameObject() {
	for (int i = 0; i < gpGlobals->maxEntities; i++) {
		g_EntityObjs[i].Clean();
		g_LastInfos[i].Clear();
	}
}
void CEntityObject::RemoveGameObject(int index) {
	g_EntityObjs[index].Clean();
	g_LastInfos[index].Clear();
}
CEntityObject* CEntityObject::GetGameObject(int index) {
	if (index < gpGlobals->maxEntities)
		return &g_EntityObjs[index];
	return nullptr;
}
entitylaginfo_t* CEntityObject::GetLastLagInfo(int index){
	return &g_LastInfos[index];
}