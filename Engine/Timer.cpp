#include "Timer.h"

namespace
{
	float deltaTime;
	DWORD currentTime;
	DWORD previousTime;
}
void Timer::Initialize()
{
	deltaTime = 0.0f;
	currentTime = timeGetTime();
	previousTime = timeGetTime();
}
void Timer::Update()
{
	currentTime = timeGetTime();

	deltaTime = (currentTime - previousTime) / 1000.0f;

	previousTime = currentTime;
}
void Timer::Release() 
{

}
float Timer::GetDeltaTime()
{
	return deltaTime;
}
