#pragma once
#include <Windows.h>
namespace Timer
{
	void Initialize();
	void Update();
	void Release();
	float GetDeltaTime();
};