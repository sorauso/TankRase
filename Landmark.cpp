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

	initialUpdateCount_ = 0;
}

void Landmark::Update()
{
	if (initialUpdateCount_ < 3)
	{
		initialUpdateCount_++;
		RotationGroud();
	}
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

	// 戦車のY軸回転（ラジアン）
	float rotY = XMConvertToRadians(transform_.rotate_.y);

	// 地面の法線
	XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&n));

	// 戦車の旋回を打ち消す回転行列
	XMMATRIX invRotY = XMMatrixRotationY(-rotY);

	// 法線を戦車のローカル座標系に変換
	XMVECTOR localNormal = XMVector3TransformNormal(normal, invRotY);

	XMFLOAT3 localN;
	XMStoreFloat3(&localN, localNormal);

	// ローカル座標の傾斜角度を求める
	transform_.rotate_.x = XMConvertToDegrees(atan2f(localN.z, localN.y));

	transform_.rotate_.z = XMConvertToDegrees(atan2f(-localN.x, localN.y));

	transform_.position_.y = 0 - data1.dist;
}