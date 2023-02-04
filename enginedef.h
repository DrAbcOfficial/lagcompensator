#pragma once
#include "progdefs.h"

void UninstallHook();
// SV_Physics
typedef void(*fnSV_Physics)();
void NewSV_Physics();
PRIVATE_FUNCTION_EXTERN(SV_Physics);