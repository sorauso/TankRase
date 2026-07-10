#include "Car.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Debug.h"
#include "Engine/Camera.h"
#include "Engine/SphereCollider.h"

#include "Ground.h"
#include "Landmark.h"

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
	isHitExplosion = false;

	pHitTeaget = nullptr;


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
	transform_.rotate_.y += aRotY;

	if (not isHitCar && not isHitExplosion)
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
	if (pHitTeaget != nullptr)
	{
		if (isHitCar)
		{

			XMFLOAT3 tPosF3 = pHitTeaget->GetPosition();
			XMVECTOR tPosVC = XMLoadFloat3(&tPosF3);
			XMVECTOR myPosVC = XMLoadFloat3(&transform_.position_);
			XMVECTOR vecT = tPosVC - myPosVC;
			float yaw = XMConvertToRadians(this->transform_.rotate_.y);
			XMFLOAT3 rotF3 = XMFLOAT3(sinf(yaw), 0, cosf(yaw));
			XMVECTOR rotVC = XMLoadFloat3(&rotF3);
			rotVC = XMVector3Normalize(rotVC);
			vecT = XMVector3Normalize(vecT);
			float side = XMVectorGetY(XMVector3Cross(rotVC, vecT));
			if (side > 0.01f)
			{
				Vt += XMVectorSet(-0.01f, 0, 0, 0);
			}
			else if (side < -0.01f)
			{
				Vt += XMVectorSet(0.01f, 0, 0, 0);
			}
			else
			{
			}
		}
		if (isHitExplosion)
		{
			XMFLOAT3 tPosF3 = pHitTeaget->GetPosition();
			XMVECTOR tPosVC = XMLoadFloat3(&tPosF3);
			XMVECTOR myPosVC = XMLoadFloat3(&transform_.position_);
			XMVECTOR vecT = myPosVC - tPosVC;
			vecT = XMVector3Normalize(vecT);
			myPosVC = myPosVC + vecT;
			XMStoreFloat3(&transform_.position_, myPosVC);
		}
	}
	else
	{
		Vt *= XMVectorSet(0, 1, 1, 1);
	}
	transform_.position_ = MovingVectorCreation(transform_, Vt);
	//RotationTank();

	
	Ground* pGround = (Ground*)FindObject("Ground");    //ステージオブジェクトを探す
	int hGroundModel = pGround->GetModelHundle();    //モデル番号を取得
	RayCastData data1;
	XMFLOAT3 startPos;
	startPos = transform_.position_;
	startPos.y = 0;
	data1.start = startPos;           //レイの発射位置
	data1.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data1); //レイを発射

	if (transform_.position_.y <= 0 - data1.dist)
	{
		transform_.position_.y = 0 - data1.dist;
	}
	else
	{
		transform_.position_.y -= 0.05f;
	}


	isHitExplosion = false;
	isHitCar = false;
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
	if (pTarget->GetObjectName() == "Bullet")
	{
		pHitTeaget = pTarget;
		isHitExplosion = true;
	}
}

void Car::RotationTank()
{
	float BoxSozeX = 1.5f;
	float BoxSozeZ = 2.5f;
	XMFLOAT3 rayPos = transform_.position_;
	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
	XMVECTOR vPos = { 0.0f,0.0f,0.0f,0.0f };
	XMVECTOR ryaMovePos = XMLoadFloat3(&rayPos);
	Ground* pGround = (Ground*)FindObject("Ground");    //ステージオブジェクトを探す
	int hGroundModel = pGround->GetModelHundle();    //モデル番号を取得
	RayCastData data1;
	XMFLOAT3 startPos;
	vPos = { -BoxSozeX ,0,0,0 };
	vPos = XMVector3TransformCoord(vPos, matRot);
	XMStoreFloat3(&startPos, ryaMovePos + vPos);
	startPos.y = 0;
	data1.start = startPos;           //レイの発射位置
	data1.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data1); //レイを発射

	RayCastData data2;
	vPos = { BoxSozeX ,0,0,0 };
	vPos = XMVector3TransformCoord(vPos, matRot);
	XMStoreFloat3(&startPos, ryaMovePos + vPos);
	startPos.y = 0;
	data2.start = startPos;      //レイの発射位置
	data2.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data2); //レイを発射

	RayCastData data3;
	vPos = { 0 ,0,BoxSozeZ,0 };
	vPos = XMVector3TransformCoord(vPos, matRot);
	XMStoreFloat3(&startPos, ryaMovePos + vPos);
	startPos.y = 0;
	data3.start = startPos;        //レイの発射位置
	data3.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data3); //レイを発射

	RayCastData data4;
	vPos = { 0 ,0,-BoxSozeZ,0 };
	vPos = XMVector3TransformCoord(vPos, matRot);
	XMStoreFloat3(&startPos, ryaMovePos + vPos);
	startPos.y = 0;
	data4.start = startPos;           //レイの発射位置
	data4.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data4); //レイを発射

	float d1y = (data1.dist - data2.dist) / 2;
	float d1s = atan2(d1y, BoxSozeX / 2);

	transform_.rotate_.z = d1s * 180.0f / 3.14159265f;

	float d2y = (data3.dist - data4.dist) / 2;
	float d2s = atan2(d2y, BoxSozeZ / 2);

	transform_.rotate_.x = d2y * 180.0f / 3.14159265f;

	float Mini = min(min(data1.dist, data2.dist), min(data3.dist, data4.dist));
	float Maxe = max(max(data1.dist, data2.dist), max(data3.dist, data4.dist));

	transform_.position_.y = 0 - (Mini + Maxe) / 2;
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
