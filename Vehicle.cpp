#include "Vehicle.h"
#include "Ground.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Debug.h"
#include "Engine/Camera.h"
#include "Engine/SphereCollider.h"
#include "Engine/Timer.h"
#include "Landmark.h"

Vehicle::Vehicle(GameObject* parent)
	:GameObject(parent, "Vehicle")
{
}

Vehicle::Vehicle(GameObject* parent, const std::string& name)
	:GameObject(parent, name)
{
}

Vehicle::~Vehicle()
{
}

void Vehicle::Initialize()
{
}

void Vehicle::Update()
{
}

void Vehicle::Draw()
{
}

void Vehicle::Release()
{
}

void Vehicle::RotationBody(Transform& tra)
{
	Ground* pGround = (Ground*)FindObject("Ground");    //ステージオブジェクトを探す
	int hGroundModel = pGround->GetModelHundle();    //モデル番号を取得
	RayCastData data1;
	XMFLOAT3 startPos;
	startPos = tra.position_;
	startPos.y = 0;
	data1.start = startPos;           //レイの発射位置
	data1.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data1); //レイを発射

	XMFLOAT3 n = data1.normalVector;

	// 戦車のY軸回転（ラジアン）
	float rotY = XMConvertToRadians(tra.rotate_.y);

	// 地面の法線
	XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&n));

	// 戦車の旋回を打ち消す回転行列
	XMMATRIX invRotY = XMMatrixRotationY(-rotY);

	// 法線を戦車のローカル座標系に変換
	XMVECTOR localNormal = XMVector3TransformNormal(normal, invRotY);

	XMFLOAT3 localN;
	XMStoreFloat3(&localN, localNormal);

	// ローカル座標の傾斜角度を求める
	GroundRotate_.x = XMConvertToDegrees(
		atan2f(localN.z, localN.y)
	);

	GroundRotate_.z = XMConvertToDegrees(
		atan2f(-localN.x, localN.y)
	);

	tra.rotate_.x = tra.rotate_.x + (GroundRotate_.x - tra.rotate_.x) * Timer::GetDeltaTime();
	tra.rotate_.z = tra.rotate_.z + (GroundRotate_.z - tra.rotate_.z) * Timer::GetDeltaTime();

	float nextY = tra.position_.y - verticalVelocity * Timer::GetDeltaTime();


	if (nextY <= 0 - data1.dist)
	{
		tra.position_.y = 0 - data1.dist;
		verticalVelocity = 0;
	}
	else
	{
		tra.position_.y -= verticalVelocity * Timer::GetDeltaTime();
		verticalVelocity += gravity * Timer::GetDeltaTime();
		if (verticalVelocity >= verticalVelocityMax)
		{
			verticalVelocity = verticalVelocityMax;
		}
	}
}

void Vehicle::CalculateSlideVector(GameObject* terget, Transform mytf, XMVECTOR& ret)
{
	XMFLOAT3 myPos = mytf.position_;
	XMFLOAT3 tarPos = terget->GetPosition();
	XMVECTOR vMyPos = XMLoadFloat3(&myPos);
	XMVECTOR vTarPos = XMLoadFloat3(&tarPos);

	XMVECTOR normal = XMVector3Normalize(XMVectorSubtract(vMyPos, vTarPos));
	XMMATRIX rot = XMMatrixRotationY(XMConvertToRadians(mytf.rotate_.y));
	XMMATRIX invRot = XMMatrixTranspose(rot);
	normal = XMVector3TransformNormal(normal, invRot);
	float dot = XMVectorGetX(XMVector3Dot(ret, normal));

	if (dot < 0.0f)
	{
		ret = XMVectorSubtract(ret, XMVectorScale(normal, dot));
	}
}

XMFLOAT3 Vehicle::MovingVectorCreation(Transform ts, XMVECTOR vt)
{
	XMFLOAT3 output = { 0,0,0 };
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

float Vehicle::GetLandmarkdirection()
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

XMFLOAT3 Vehicle::HitObjevtMove(GameObject* terget, Transform mytf, float pb, XMFLOAT3 Vt)
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