#include "Car.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Debug.h"
#include "Engine/Camera.h"
#include "Engine/SphereCollider.h"

#include "Ground.h"
#include "Landmark.h"
#include "Engine/Timer.h"

namespace
{
	const float EXPLOSION_KNOCKBACK_FORCE = 150;
	// 車との衝突時の反発力
	constexpr float CAR_COLLISION_REPULSION_FORCE = 0.02f;

	// 戦車との衝突時の反発力
	constexpr float TANK_COLLISION_REPULSION_FORCE = 0.02f;
}

Car::Car(GameObject* parent)
    :Vehicle(parent, "Car"), hModel_(-1)
{
}

Car::~Car()
{
}

void Car::Initialize()
{
    hModel_ = Model::Load("TankBody.fbx");
    assert(hModel_ >= 0);
    SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 2.0f);
    AddCollider(collision);

	const float rot = ((rand() % 50) + 10) * 0.001f;
	const float AC = ((rand() % 15) + 1) * 0.001f;
	const float DEC = ((rand() % 15) + 1) * 0.001f;
	const float AC_MAX = ((rand() % 600) + 1500) * 0.001f;
	const float B_MAX = ((rand() % 500) + 500) * 0.001f;
	myStates = CAR_STATES(rot, AC, DEC, AC_MAX,B_MAX);

	const float sPosX = (rand() % 40) + 200;
	const float sPosZ = (rand() % 40) + 200;
	transform_.position_ = XMFLOAT3(-sPosX, 0, sPosZ);

	Vt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	aRotY = 0;

	isHitCar = false;
	isHitTank = false;
	isHitExplosion = false;

	pHitTeaget = nullptr;
	isFlying = false;


	gravity = 9.8f;
	verticalVelocity = 0.0f;
	verticalVelocityMax = 40.0f;


	Ground* pGround = (Ground*)FindObject("Ground");    //ステージオブジェクトを探す
	int hGroundModel = pGround->GetModelHundle();    //モデル番号を取得
	RayCastData data1;
	XMFLOAT3 startPos;
	startPos = transform_.position_;
	startPos.y = 0;
	data1.start = startPos;           //レイの発射位置
	data1.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data1); //レイを発射
	transform_.position_.y = 0 - data1.dist;
}

void Car::Update()
{
	

	XMFLOAT3 oldPostion = transform_.position_;

	HandRing();
	AcceleWerk();
	if (pHitTeaget != nullptr)
	{
		if (isHitCar)
		{
			XMFLOAT3 Vtf;
			XMStoreFloat3(&Vtf, Vt);
			Vtf = HitObjevtMove(pHitTeaget,transform_, CAR_COLLISION_REPULSION_FORCE,Vtf);
			Vt = XMLoadFloat3(&Vtf);
		}
		if (isHitTank)
		{
			
		}
		if (!isFlying)
		{
			if (isHitExplosion)
			{
				XMFLOAT3 tPosF3 = pHitTeaget->GetPosition();
				tPosF3.y = tPosF3.y - 10;
				XMVECTOR tPosVC = XMLoadFloat3(&tPosF3);
				XMVECTOR myPosVC = XMLoadFloat3(&transform_.position_);
				XMVECTOR vecT = myPosVC - tPosVC;
				vecT = XMVector3Normalize(vecT);
				myPosVC = vecT * EXPLOSION_KNOCKBACK_FORCE * Timer::GetDeltaTime();
				Vt += myPosVC;
				isFlying = true;
			}
		}
	}
	transform_.position_ = MovingVectorCreation(transform_, Vt);

	RotationBody(transform_);
	isHitCar = false;
	isHitTank = false;
	isHitExplosion = false;

	if (transform_.position_.x > STAGE_SIZE || transform_.position_.x < -STAGE_SIZE)
	{
		transform_.position_.x = oldPostion.x;
	}
	if (transform_.position_.z > STAGE_SIZE || transform_.position_.z < -STAGE_SIZE)
	{
		transform_.position_.z = oldPostion.z;
	}
}

void Car::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Car::Release()
{
}

void Car::SetCarNunber(int num)
{
	CarNunber = num;
}

void Car::OnCollision(GameObject* pTarget)
{
	if (pTarget->GetObjectName() == "Car")
	{
		CalculateSlideVector(pTarget, transform_, Vt);
	}
	if (pTarget->GetObjectName() == "TankBody")
	{
		CalculateSlideVector(pTarget, transform_, Vt);
	}
	if (pTarget->GetObjectName() == "Bullet")
	{
		pHitTeaget = pTarget;
		isHitExplosion = true;
	}
}

void Car::HandRing()
{
	if (GetLandmarkdirection() < -5)
	{
		if (aRotY > -1.5f)
		{
			aRotY -= myStates.rotSpeed;
		}
	}
	else if (GetLandmarkdirection() > 5)
	{
		if (aRotY < 1.5f)
		{
			aRotY += myStates.rotSpeed;
		}
	}
	else
	{
		if (aRotY > 0.01f)
		{
			aRotY -= myStates.rotSpeed;
		}
		else if (aRotY < -0.01f)
		{
			aRotY += myStates.rotSpeed;
		}
		else
		{
			aRotY = 0.0f;
		}
	}
	if (!isFlying)
	{
		transform_.rotate_.y += aRotY;
	}
}

void Car::AcceleWerk()
{
	if (!isHitCar || !isHitExplosion)
	{
		XMFLOAT3 Vtf;
		XMStoreFloat3(&Vtf, Vt);
		if (Vtf.z < myStates.ACCELERTION_MAX)
		{
			Vt += XMVectorSet(0.0f, 0.0f, myStates.ACCELERATION, 0.0f);
		}
	}
	else
	{
		Vt -= XMVectorSet(0.0f, 0.0f, myStates.DECELERATION, 0.0f);
	}
}