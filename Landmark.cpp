#include "Landmark.h"
#include "Engine\Model.h"
#include "Engine/Input.h"
#include "Engine/SphereCollider.h"

#include "Ground.h"
#include "LandmarkMother.h"
#include "Car.h"

Landmark::Landmark(GameObject* parent)
	:GameObject(parent, "LandMark"), hModel_(-1)
{
}

Landmark::~Landmark()
{

}

void Landmark::Initialize()
{
	hModel_ = Model::Load("Enemy.fbx");
	move_ = XMFLOAT3(0, 0, 0);
	SphereCollider* collision = new SphereCollider(XMFLOAT3(0, 0, 0), 10.0f);
	AddCollider(collision);

	transform_.scale_ = XMFLOAT3(10,10,10);

	Model::SetAnimFrame(hModel_, 0, 100, 1);
}

void Landmark::Update()
{
	RotationGroud();
}

void Landmark::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Landmark::Release()
{
	LandmarkMother* lmm = (LandmarkMother*)this->GetParent();
	if (lmm == nullptr)
	{
		return;
	}
	if (pAcquisitionCar == nullptr)
	{
		return;
	}
	if (pAcquisitionCar->GetObjectName() == "TankBody")
	{
		lmm->SetAcquisitionCarNunber(0);
	}
	else if (pAcquisitionCar->GetObjectName() == "Car")
	{
		Car* pCar = (Car*)pAcquisitionCar;
		int carNum = pCar->GetCarNunber();
		lmm->SetAcquisitionCarNunber(carNum);
	}
	lmm->MakeNextLandmark();
}

void Landmark::OnCollision(GameObject* pTarget)
{
	pAcquisitionCar = pTarget;
	if (pAcquisitionCar->GetObjectName() == "Bullet")
	{
		return;
	}
	KillMe();
}

void Landmark::RotationGroud()
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