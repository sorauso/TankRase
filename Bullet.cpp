#include "Bullet.h"
#include "Engine\Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"
#include "Ground.h"
#include "Engine/Timer.h"

namespace
{
	const float DEREAT_POSITION = 700.0f;
	const float DESTROI_TIME = 0.5f;
	const float EXPLOSION_ENABLE_DELAY_FRAMES = 0.01f;
	const float GRAVITY = 0.005f;
}

Bullet::Bullet(GameObject* parent)
	:GameObject(parent, "Bullet"), hModel_(-1), hExplosionModel_(-1)
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize()
{
	hModel_ = Model::Load("Bullet.fbx");
	hExplosionModel_ = Model::Load("DebugCollision/SphereCollider.fbx");
	assert(hExplosionModel_ >= 0);
	move_ = XMFLOAT3(0, 0, 0);
	inExplosion = false;
	destroyTimer = 0;
	explosionEnableFrameTimer = 0;
}

void Bullet::Update()
{

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
	XMVECTOR vMove = XMLoadFloat3(&move_);
	vMove -= XMVECTOR{ 0,GRAVITY,0,0 };
	vPos = vPos + vMove;
	XMStoreFloat3(&move_, vMove);
	XMStoreFloat3(&transform_.position_, vPos);
	if (transform_.position_.z > DEREAT_POSITION ||
		transform_.position_.z < -DEREAT_POSITION ||
		transform_.position_.x > DEREAT_POSITION ||
		transform_.position_.x < -DEREAT_POSITION ||
		transform_.position_.y < -DEREAT_POSITION)
	{
		KillMe();
	}

	if (inExplosion)
	{
		destroyTimer += Timer::GetDeltaTime();
		if (destroyTimer > DESTROI_TIME)
		{
			KillMe();
		}
		return;
	}
	if (explosionEnableFrameTimer >= EXPLOSION_ENABLE_DELAY_FRAMES)
	{
		if (OnProjectileImpact())
		{
			SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 10.0f);
			AddCollider(collision);
			inExplosion = true;
		}
		explosionEnableFrameTimer = 0;
	}
	explosionEnableFrameTimer += Timer::GetDeltaTime();
}

void Bullet::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
	if (inExplosion)
	{
		Transform expTransform = transform_;
		expTransform.scale_ = XMFLOAT3(10,10,10);
		Model::SetTransform(hExplosionModel_, expTransform);
		Model::Draw(hExplosionModel_);
	}
}

void Bullet::Release()
{
}

void Bullet::SetMoveVector(XMFLOAT3 mv)
{
	move_ = mv;
}

void Bullet::OnCollision(GameObject* pTarget)
{
}

bool Bullet::OnProjectileImpact()
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
	if (transform_.position_.y < 0 - data1.dist && not inExplosion)
	{
		return true;
	}
	return false;
}
