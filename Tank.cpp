#include "Tank.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Debug.h"
#include "Engine/Camera.h"
#include "Engine/SphereCollider.h"
#include "Engine/Timer.h"

#include "Bullet.h"
#include "Ground.h"
#include "Landmark.h"

namespace
{
	XMVECTOR separationVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	float CanomPith = 0;

	float boostTimer = 0.0f;
	bool isBoost = false;
	float reloadTimer = 0.0f;
	bool isReloading = false;

	const float BULLET_SPEED = 1.0f;
	const float rotSpeed = 0.05f;

	const float ACCELERATION = 0.01f;
	const float DECELERATION = 0.01f;
	const float ACCELERTION_MAX = 2.0f;
	const float BACK_ACCELERTION_MAX = 0.8f;
	const float ROTATE_ROTATION_Y_MAX_SPEED = 1.5f;
	const float CAMERA_PITH_ANGLE = 0.8f;
	const float BOOST_ANGLE = 20;
	const float BOOST_TIME = 1.0f;
	const float RELOAD_TIME = 5.0f;

	// 砲身の仰俯角の回転速度
	const float BARREL_ELEVATION_SPEED = 0.5f;

	// 最大仰角
	const float MAX_BARREL_ELEVATION_ANGLE = 20.0f;

	// 最大俯角
	const float MAX_BARREL_DEPRESSION_ANGLE = 10.0f;

	// 砲塔の旋回速度
	const float TURRET_ROTATION_SPEED = 0.5f;

	const float floatMAX_TURRET_TRAVERSE_ANGLE = 180;
}

TankBody::TankBody(GameObject* parent)
    :Vehicle(parent, "TankBody"), hModel_(-1)
{
}

TankBody::~TankBody()
{
}

void TankBody::Initialize()
{
    hModel_ = Model::Load("CrusaderV.fbx");
	//hModel_ = Model::Load("TankBody.fbx");
    assert(hModel_ >= 0);
   // Instantiate<TankHead>(this);
	transform_.position_ = XMFLOAT3(-220,0,220);
	SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 2.0f);
	AddCollider(collision);
	gravity = 9.8f;
	verticalVelocity = 0.0f;
	verticalVelocityMax = 10.0f;
	transform_.scale_ = XMFLOAT3(0.02, 0.02, 0.02);
	turretDirection = XMFLOAT3(0,0,0);
	Vt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	aRotY = 0;

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

	//Model::SetAnimFrame(hModel_, 0, 100, 1);
}

void TankBody::Update()
{
	if (isReloading)
	{
		reloadTimer += Timer::GetDeltaTime();
		if (reloadTimer > RELOAD_TIME)
		{
			isReloading = false;
		}
	}
	else
	{
		reloadTimer = 0;
	}
	UpdateMove();

	RotationBody(transform_);

	UpdateTurret();

	SetTpsRotCamera();


	if (Input::IsKeyDown(DIK_SPACE))
	{
		if (!isReloading)
		{
			isReloading = true;
			Fire();
		}
	}

}

void TankBody::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void TankBody::Release()
{
}



void TankBody::OnCollision(GameObject* pTarget)
{
	if (pTarget->GetObjectName() == "Obstacle")
	{
		CalculateSlideVector(pTarget,transform_,Vt);
	}
}

void TankBody::SetTpsRotCamera()
{
	const float pitch = XMConvertToRadians(turretDirection.x * CAMERA_PITH_ANGLE);
	const float yaw = XMConvertToRadians(transform_.rotate_.y + turretDirection.y);
	const float roll = XMConvertToRadians(turretDirection.z);
	XMMATRIX matRot = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
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

void TankBody::UpdateTurret()
{

	if (Input::IsKey(DIK_LEFT))
	{
		turretDirection.y -= TURRET_ROTATION_SPEED;
		if (turretDirection.y <= -floatMAX_TURRET_TRAVERSE_ANGLE)
		{
			turretDirection.y = -floatMAX_TURRET_TRAVERSE_ANGLE;
		}
	}
	else if (Input::IsKey(DIK_RIGHT))
	{
		turretDirection.y += TURRET_ROTATION_SPEED;
		if (turretDirection.y >= floatMAX_TURRET_TRAVERSE_ANGLE)
		{
			turretDirection.y = floatMAX_TURRET_TRAVERSE_ANGLE;
		}
	}
	else
	{
		if (turretDirection.y >= 0.1f)
		{
			//turretDirection.y -= TURRET_ROTATION_SPEED;
		}
		else if (turretDirection.y <= -0.1f)
		{
			//turretDirection.y += TURRET_ROTATION_SPEED;
		}
		else
		{
			turretDirection.y = 0.0;
		}
	}
	if (Input::IsKey(DIK_DOWN))
	{
		turretDirection.x -= BARREL_ELEVATION_SPEED;
		if (turretDirection.x <= -MAX_BARREL_ELEVATION_ANGLE)
		{
			turretDirection.x = -MAX_BARREL_ELEVATION_ANGLE;
		}
	}
	else if (Input::IsKey(DIK_UP))
	{
		turretDirection.x += BARREL_ELEVATION_SPEED;
		if (turretDirection.x >= MAX_BARREL_DEPRESSION_ANGLE)
		{
			turretDirection.x = MAX_BARREL_DEPRESSION_ANGLE;
		}
	}
	else
	{
		if (turretDirection.x >= 0.1f)
		{
			turretDirection.x -= BARREL_ELEVATION_SPEED;
		}
		else if (turretDirection.x <= -0.1f)
		{
			turretDirection.x += BARREL_ELEVATION_SPEED;
		}
		else
		{
			turretDirection.x = 0.0;
		}
	}
	assert(Model::RotationBonePosition(hModel_, "Under", XMFLOAT3(turretDirection.x, 0, 0)));
	assert(Model::RotationBonePosition(hModel_, "Head", XMFLOAT3(0, turretDirection.y, 0)));
}

void TankBody::Fire()
{
	XMFLOAT3 position = transform_.position_;
	XMFLOAT3 cannonTip = Model::GetBonePosition(hModel_, "Tip");
	XMFLOAT3 cannonTop = Model::GetBonePosition(hModel_, "Top");
	XMFLOAT3 cannonRoot = Model::GetBonePosition(hModel_, "Under");
	XMVECTOR vPosition = XMLoadFloat3(&position);
	XMVECTOR vCannonTip = XMLoadFloat3(&cannonTip);
	XMVECTOR vCannonTpo = XMLoadFloat3(&cannonTop);
	XMVECTOR vCannonRoot = XMLoadFloat3(&cannonRoot);
	XMVECTOR vMove = vCannonTpo - vCannonRoot;
	vMove = XMVector3Normalize(vMove);
	vMove = XMVectorScale(vMove, BULLET_SPEED);
	XMFLOAT3 fMove;
	XMStoreFloat3(&fMove, vMove);
	Bullet* pBullet = Instantiate<Bullet>(this->GetParent());
	pBullet->SetMoveVector(fMove);
	pBullet->SetPosition(cannonTip);
}

void TankBody::UpdateMove()
{
	
	if (Input::IsKey(DIK_A))
	{
		if (aRotY > -ROTATE_ROTATION_Y_MAX_SPEED)
		{
			aRotY -= rotSpeed;
		}
	}
	else if (Input::IsKey(DIK_D))
	{
		if (aRotY < ROTATE_ROTATION_Y_MAX_SPEED)
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
	if (Input::IsKey(DIK_LSHIFT))
	{
		if (!isBoost && !isReloading)
		{
			isBoost = true;
			isReloading = true;
		}
	}
	if (isBoost)
	{
		Vt += XMVectorSet(0.0f, 0.0f, ACCELERATION * BOOST_ANGLE, 0.0f);
		boostTimer += Timer::GetDeltaTime();
		if (boostTimer > BOOST_TIME)
		{
			isBoost = false;
			boostTimer = 0;
		}
	}
	else
	{
		XMFLOAT3 Vtf;
		XMStoreFloat3(&Vtf, Vt);
		if (Vtf.z > ACCELERTION_MAX)
		{
			Vt -= XMVectorSet(0.0f, 0.0f, ACCELERATION * BOOST_ANGLE, 0.0f);
		}
	}

	transform_.position_ = MovingVectorCreation(transform_, Vt);
}

