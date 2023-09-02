#pragma once
#include <list>
#include <memory>
typedef struct entitylaginfo_s {
	Vector Origin;
	Vector Angles;
	int GaitSequence;
	int Sequence;
	float Frame;
	float FrameRate;
	float AnimTime;

	void Clear() {
		Origin = Vector(0, 0, 0);
		Angles = Vector(0, 0, 0);
		GaitSequence = 0;
		Sequence = 0;
		Frame = 0;
		FrameRate = 0;
		AnimTime = 0;
	}
}entitylaginfo_t;

typedef struct entityinfo_s {
	entitylaginfo_t info;
	float time;
}entityinfo_t;
class CEntityObject {
public:
	std::list<std::shared_ptr<entityinfo_t>> aryLagInfo;
	void Clean() {
		aryLagInfo.clear();
	}
	static void InitGameObject();
	static void ClearGameObject();
	static void RemoveGameObject(int index);
	static CEntityObject* GetGameObject(int index);
	static entitylaginfo_t* GetLastLagInfo(int index);
};