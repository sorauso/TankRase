#include "Car.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Debug.h"
#include "Engine/Camera.h"
#include "Engine/SphereCollider.h"

#include "Ground.h"
#include "Landmark.h"
#include "Engine/Timer.h"

Car::Car(GameObject* parent)
    :GameObject(parent, "Car"), hModel_(-1)
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
	const int STAGE_SIZE = 245;

	XMFLOAT3 oldPostion = transform_.position_;

	HandRing();
	AcceleWerk();
	if (pHitTeaget != nullptr)
	{
		if (isHitCar)
		{
			XMFLOAT3 Vtf;
			XMStoreFloat3(&Vtf, Vt);
			Vtf = HitObjevtMove(pHitTeaget,transform_,0.01f,Vtf);
			Vt = XMLoadFloat3(&Vtf);
		}
		if (isHitTank)
		{
			XMFLOAT3 Vtf;
			XMStoreFloat3(&Vtf, Vt);
			Vtf = HitObjevtMove(pHitTeaget, transform_, 0.02f,Vtf);
			Vt = XMLoadFloat3(&Vtf);
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
				myPosVC = vecT * 100.0f * Timer::GetDeltaTime();
				Vt += myPosVC;
				isFlying = true;
			}
		}
	}
	transform_.position_ = MovingVectorCreation(transform_, Vt);

	RotationCar();
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

float Car::GetLandmarkdirection()
{
	float ret = 0;
	Landmark* pLDK = (Landmark*)FindObject("LandMark");
	if (pLDK == nullptr)
	{
		return ret;
	}
	XMFLOAT3 ldPosF3 = pLDK->GetPosition();
	ldPosF3.y = 0;
	XMVECTOR ldPosVC = XMLoadFloat3(&ldPosF3);
	XMFLOAT3 tankPosF3 = this->transform_.position_;
	tankPosF3.y = 0;
	XMVECTOR tankPosVC = XMLoadFloat3(&tankPosF3);
	XMVECTOR vecT = ldPosVC - tankPosVC;
	float yaw = XMConvertToRadians(this->transform_.rotate_.y);
	XMFLOAT3 rotF3 = XMFLOAT3(sinf(yaw), 0, cosf(yaw));
	XMVECTOR rotVC = XMLoadFloat3(&rotF3);
	rotVC = XMVector3Normalize(rotVC);
	vecT = XMVector3Normalize(vecT);
	float side = XMVectorGetY(XMVector3Cross(rotVC, vecT));
	float front = XMVectorGetX(XMVector3Dot(rotVC, vecT));
	ret = XMConvertToDegrees(atan2f(side, front));
	return ret;
}

void Car::SetCarNunber(int num)
{
	CarNunber = num;
}

void Car::OnCollision(GameObject* pTarget)
{
	if (pTarget->GetObjectName() == "Car")
	{
		pHitTeaget = pTarget;
		isHitCar = true;
	}
	if (pTarget->GetObjectName() == "TankBody")
	{
		pHitTeaget = pTarget;
		isHitTank = true;
	}
	if (pTarget->GetObjectName() == "Bullet")
	{
		pHitTeaget = pTarget;
		isHitExplosion = true;
	}
}

void Car::RotationCar()
{
	Ground* pGround = (Ground*)FindObject("Ground");    //ステージオブジェクトを探す
	int hGroundModel = pGround->GetModelHundle();    //モデル番号を取得
	RayCastData data1;
	XMFLOAT3 startPos;
	startPos = transform_.position_;
	startPos.y = 0;
	data1.start = startPos;           //レイの発射位置
	data1.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data1); //レイを発射
	XMFLOAT3 n = data1.normalVector;
	float rotY = transform_.rotate_.y * (XM_PI / 180);
	n.x = n.x * cosf(rotY);
	n.z = n.z * cosf(rotY);
	GroundRotate_.x = atan2f(n.z, n.y) * (180 / XM_PI);
	GroundRotate_.z = atan2f(-n.x, n.y) * (180 / XM_PI);

	transform_.rotate_.x = transform_.rotate_.x + (GroundRotate_.x - transform_.rotate_.x) * Timer::GetDeltaTime();
	transform_.rotate_.z = transform_.rotate_.z + (GroundRotate_.z - transform_.rotate_.z) * Timer::GetDeltaTime();

	//次のフレームの時のポジションを計算
	float nextY = transform_.position_.y - verticalVelocity * Timer::GetDeltaTime();

	//落下処理
	if (nextY <= 0 - data1.dist)
	{
		transform_.position_.y = 0 - data1.dist;
		verticalVelocity = 0;
		Vt = XMVectorSetY(Vt,0);
		isFlying = false;
	}
	else
	{
		transform_.position_.y -= verticalVelocity * Timer::GetDeltaTime();
		verticalVelocity += gravity * Timer::GetDeltaTime();
		if (verticalVelocity >= verticalVelocityMax)
		{
			verticalVelocity = verticalVelocityMax;
		}
	}
}

XMFLOAT3 Car::HitObjevtMove(GameObject* terget,Transform mytf,float pb,XMFLOAT3 Vt)
{
	XMVECTOR retv = XMLoadFloat3(&Vt);
	XMFLOAT3 retf;
	XMFLOAT3 tPosF3 = terget->GetPosition();
	XMVECTOR tPosVC = XMLoadFloat3(&tPosF3);
	XMVECTOR myPosVC = XMLoadFloat3(&mytf.position_);
	XMVECTOR vecT = tPosVC - myPosVC;
	float yaw = XMConvertToRadians(mytf.rotate_.y);
	XMFLOAT3 rotF3 = XMFLOAT3(sinf(yaw), 0, cosf(yaw));
	XMVECTOR rotVC = XMLoadFloat3(&rotF3);
	rotVC = XMVector3Normalize(rotVC);
	vecT = XMVector3Normalize(vecT);
	float side = XMVectorGetY(XMVector3Cross(rotVC, vecT));
	if (side > 0.01f)
	{
		retv += XMVectorSet(-pb, 0, 0, 0);
	}
	else if (side < -0.01f)
	{
		retv += XMVectorSet(pb, 0, 0, 0);
	}
	else
	{
	}
	XMStoreFloat3(&retf, retv);
	return retf;
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

XMFLOAT3 Car::MovingVectorCreation(Transform ts, XMVECTOR vt)
{
	const int STAGE_SIZE = 245;
	XMFLOAT3 output = { 0,0,0 };
	const float speed = 0.1f;
	//回転行列の生成
	const float pitch = XMConvertToRadians(ts.rotate_.x);
	const float yaw = XMConvertToRadians(ts.rotate_.y);
	const float roll = XMConvertToRadians(ts.rotate_.z);
	XMMATRIX Mr = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	//合成ベクトルを生成
	XMVECTOR xM = XMVector3TransformCoord(vt, Mr);
	//positionをベクトルに
	XMVECTOR pos = XMLoadFloat3(&ts.position_);
	//positionと合成ベクトルをかけ合わせる
	pos = XMVectorAdd(pos, XMVectorScale(xM, speed));
	//出力
	XMStoreFloat3(&output, pos);
	if (output.x > STAGE_SIZE || output.x < -STAGE_SIZE)
	{
		output.x = ts.position_.x;
	}
	if (output.z > STAGE_SIZE || output.z < -STAGE_SIZE)
	{
		output.z = ts.position_.z;
	}
	return output;
}
