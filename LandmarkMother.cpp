#include "LandmarkMother.h"
#include "Landmark.h"

namespace
{
	const int positionListMax = 15;
	float positionX[positionListMax] = {-110.0f,-20.0f,-90.0f,-130.0f,-190.0f,-210.0f,-60.0f,80.0f,100.0f,130.0f,80.0f,10.0f,-110.0f,-30.0f,-220.0f };
	float positionY[positionListMax] = { 220.0f,160.0f,120.0f,60.0f,-20.0f,-140.0f,-170.0f,-160.0f,-60.0f,70.0f,110.0f,160.0f,120.0f,190.0f,220.0f };
	int acquisitionCarS[positionListMax];
}

LandmarkMother::LandmarkMother(GameObject* parent)
	:GameObject(parent, "LadmarkMother"), hModel_(-1), counter_(0)
{
}

LandmarkMother::~LandmarkMother()
{
}

void LandmarkMother::Initialize()
{
	MakeNextLandmark();
}

void LandmarkMother::Update()
{
	//char buf[256];
	//sprintf_s(buf, "%d\n", acquisitionCarS[counter_-1]);
	//OutputDebugStringA(buf);
}

void LandmarkMother::Draw()
{
}

void LandmarkMother::Release()
{
}

void LandmarkMother::OnCollision(GameObject* pTarget)
{
}

void LandmarkMother::MakeNextLandmark()
{
	if (counter_ > positionListMax)
	{
		return;
	}
	Landmark* lm = Instantiate<Landmark>(this);
	lm->SetPosition(XMFLOAT3(positionX[counter_], 0, positionY[counter_]));
	counter_++;
}

void LandmarkMother::GetAcquisitionCarNunber(int num)
{
	acquisitionCarS[counter_] = num;
}
