#include "Bullet.h"
#include "Engine\Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"

Bullet::Bullet(GameObject* parent)
	:GameObject(parent, "Bullet"), hModel_(-1)
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize()
{
	hModel_ = Model::Load("Bullet.fbx");
	move_ = XMFLOAT3(0, 0, 0);
	SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 0.3f);
	AddCollider(collision);
}

void Bullet::Update()
{

	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
	XMVECTOR vMove = XMLoadFloat3(&move_);
	vMove -= XMVECTOR{ 0,0.005f,0,0 };
	vPos = vPos + vMove;
	XMStoreFloat3(&move_, vMove);
	XMStoreFloat3(&transform_.position_, vPos);
	const float DereatPosition = 250.0f;
	if (transform_.position_.z > DereatPosition ||
		transform_.position_.z < -DereatPosition ||
		transform_.position_.x > DereatPosition ||
		transform_.position_.x < -DereatPosition ||
		transform_.position_.y < -DereatPosition)
	{
		KillMe();
	}
}

void Bullet::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
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
