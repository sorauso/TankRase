#include "Goal.h"
#include "Engine\Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"
#include "Engine/SceneManager.h"

#include "Ground.h"
#include "LandmarkMother.h"
#include "Car.h"

Goal::Goal(GameObject* parent)
	:GameObject(parent, "LandMark"), hModel_(-1)
{}

Goal::~Goal()
{

}

void Goal::Initialize()
{
	hModel_ = Model::Load("Enemy.fbx");
	move_ = XMFLOAT3(0, 0, 0);
	SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 10.0f);
	AddCollider(collision);

	transform_.scale_ = XMFLOAT3(10, 10, 10);
}

void Goal::Update()
{
	RotationGroud();
}

void Goal::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Goal::Release()
{

}

void Goal::OnCollision(GameObject* pTarget)
{
	if (pTarget->GetObjectName() == "TankBody")
	{
		SceneManager* sceneManager = (SceneManager*)(this->GetParent()->GetParent()->GetParent());
		sceneManager->ChangeScene(SCENE_ID_CLEAR);
	}
	else
	{
		pTarget->KillMe();
	}
}

void Goal::RotationGroud()
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
	transform_.rotate_.x = atan2f(n.z, n.y) * (180 / XM_PI);
	transform_.rotate_.z = atan2f(-n.x, n.y) * (180 / XM_PI);

	transform_.position_.y = 0 - data1.dist;
}