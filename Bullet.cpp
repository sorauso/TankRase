#include "Bullet.h"
#include "Engine\Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"
#include "Ground.h"

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
	inExplosion = false;
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

	Ground* pGround = (Ground*)FindObject("Ground");    //ステージオブジェクトを探す
	int hGroundModel = pGround->GetModelHundle();    //モデル番号を取得
	RayCastData data1;
	XMFLOAT3 startPos;
	startPos = transform_.position_;
	startPos.y = 0;
	data1.start = startPos;           //レイの発射位置
	data1.dir = XMFLOAT3(0, -1, 0);    //レイの方向
	Model::RayCast(hGroundModel, &data1); //レイを発射
	if (transform_.position_.y < 0-data1.dist && not inExplosion)
	{
		SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 10.0f);
		AddCollider(collision);
		inExplosion = true;
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
