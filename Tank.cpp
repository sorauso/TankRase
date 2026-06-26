#include "Tank.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Debug.h"
#include "Engine/Camera.h"

#include "Bullet.h"

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
}

void TankBody::Update()
{
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
	const float ACCELERTION_MAX = 0.8f;
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
		if (Vtf.z > -ACCELERTION_MAX)
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
	RotationTank();

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

void TankBody::RotationTank()
{
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
	const int STAGE_SIZE = 22;
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
