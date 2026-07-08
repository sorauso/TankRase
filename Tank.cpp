#include "Tank.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Debug.h"
#include "Engine/Camera.h"
#include "Engine/SphereCollider.h"

#include "Bullet.h"
#include "Ground.h"
#include "Landmark.h"

namespace
{
    XMVECTOR Vt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    float aRotY = 0;
}

TankBody::TankBody(GameObject* parent)
    :GameObject(parent, "TankBody"), hModel_(-1)
{
}

TankBody::~TankBody()
{
}

void TankBody::Initialize()
{
    hModel_ = Model::Load("TankBody.fbx");
    assert(hModel_ >= 0);
    Instantiate<TankHead>(this);
	transform_.position_ = XMFLOAT3(-220,0,220);
	SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 2.0f);
	AddCollider(collision);
}

void TankBody::Update()
{
	/*char buf[256];
	sprintf_s(buf, "pos:%.0f:%.0f\n", transform_.position_.x, transform_.position_.z);
	OutputDebugStringA(buf);*/

	const float rotSpeed = 0.05f;
	if (Input::IsKey(DIK_A))
	{
		if (aRotY > -1.5f)
		{
			aRotY -= rotSpeed;
		}
	}
	else if (Input::IsKey(DIK_D))
	{
		if (aRotY < 1.5f)
		{
			aRotY += rotSpeed;
		}
	}
	else
	{
		if (aRotY > 0.01f)
		{
			aRotY -= rotSpeed;
		}
		else if (aRotY < -0.01f)
		{
			aRotY += rotSpeed;
		}
		else
		{
			aRotY = 0.0f;
		}
	}
	transform_.rotate_.y += aRotY;

	//Debug::Log("Yangle = ");
	//Debug::Log(transform_.rotate_.y,true);

	const float ACCELERATION = 0.01f;
	const float DECELERATION = 0.01f;
	const float ACCELERTION_MAX = 2.0f;
	const float BACK_ACCELERTION_MAX = 0.8f;
	if (Input::IsKey(DIK_W))
	{
		XMFLOAT3 Vtf;
		XMStoreFloat3(&Vtf, Vt);
		if (Vtf.z < ACCELERTION_MAX)
		{
			Vt += XMVectorSet(0.0f, 0.0f, ACCELERATION, 0.0f);
		}
	}
	else if (Input::IsKey(DIK_S))
	{
		XMFLOAT3 Vtf;
		XMStoreFloat3(&Vtf, Vt);
		if (Vtf.z > -BACK_ACCELERTION_MAX)
		{
			Vt -= XMVectorSet(0.0f, 0.0f, ACCELERATION, 0.0f);
		}
	}
	else
	{
		XMFLOAT3 Vtf;
		XMStoreFloat3(&Vtf, Vt);
		if (Vtf.z > 0.01f)
		{
			Vt -= XMVectorSet(0.0f, 0.0f, DECELERATION, 0.0f);
		}
		else if (Vtf.z < -0.01f)
		{
			Vt += XMVectorSet(0.0f, 0.0f, DECELERATION, 0.0f);
		}
		else
		{
			Vt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		}
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
	transform_.position_.y = 0 - data1.dist;

	char buf[256];
	sprintf_s(buf, "%.2f:%.2f:%.2f\n", data1.normalVector.x, data1.normalVector.y, data1.normalVector.z);
	OutputDebugStringA(buf);

	SetTpsRotCamera();
}

void TankBody::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void TankBody::Release()
{
}

float TankBody::GetLandmarkdirection()
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

void TankBody::RotationTank()
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

void TankBody::SetTpsRotCamera()
{
	XMMATRIX matRot = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
	XMFLOAT3 camPos;
	XMFLOAT3 teagetPos;
	XMVECTOR vCam = { 0.0f,3.0f,-5.0f,0 };
	vCam = XMVector3TransformCoord(vCam, matRot);
	XMStoreFloat3(&camPos, vPos + vCam);
	Camera::SetPosition(camPos);
	XMVECTOR vTeaget = { 0.0f,1.0f,5.0f,0 };
	vTeaget = XMVector3TransformCoord(vTeaget, matRot);
	XMStoreFloat3(&teagetPos, vPos + vTeaget);
	Camera::SetTarget(teagetPos);
}

XMFLOAT3 TankBody::MovingVectorCreation(Transform ts, XMVECTOR vt)
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

TankHead::TankHead(GameObject* parent)
	:GameObject(parent, "TankHead"), hModel_(-1)
{
}

TankHead::~TankHead()
{
}

void TankHead::Initialize()
{
	hModel_ = Model::Load("Tankhead.fbx");
}

void TankHead::Update()
{
	if (Input::IsKey(DIK_LEFT))
	{
		transform_.rotate_.y -= 0.5;
	}
	if (Input::IsKey(DIK_RIGHT))
	{
		transform_.rotate_.y += 0.5;
	}
	if (Input::IsKeyDown(DIK_SPACE))
	{
		const float BULLET_SPEED = 0.2f;
		XMFLOAT3 cannonTpo = Model::GetBonePosition(hModel_, "Top");
		XMFLOAT3 cannonRoot = Model::GetBonePosition(hModel_, "Root");
		XMVECTOR vCannonTpo = XMLoadFloat3(&cannonTpo);
		XMVECTOR vCannonRoot = XMLoadFloat3(&cannonRoot);
		XMVECTOR vMove = vCannonTpo - vCannonRoot;
		vMove = XMVector3Normalize(vMove);
		vMove = XMVectorScale(vMove, BULLET_SPEED);
		XMFLOAT3 fMove;
		XMStoreFloat3(&fMove, vMove);
		Bullet* pBullet = Instantiate<Bullet>(this->GetParent()->GetParent());
		pBullet->SetMoveVector(fMove);
		pBullet->SetPosition(cannonTpo);
	}
}

void TankHead::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void TankHead::Release()
{
}
