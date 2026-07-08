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
	if (pAcquisitionCar->GetObjectName() == "tankBody")
	{
		lmm->GetAcquisitionCarNunber(0);
	}
	else if (pAcquisitionCar->GetObjectName() == "Car")
	{
		Car* pCar = (Car*)pAcquisitionCar;
		int carNum = pCar->GetCarNunber();
		lmm->GetAcquisitionCarNunber(carNum);
	}
	lmm->MakeNextLandmark();
}

void Landmark::OnCollision(GameObject* pTarget)
{
	pAcquisitionCar = pTarget;
	KillMe();
}

void Landmark::RotationGroud()
{
	float BoxSozeX = 5.0f;
	float BoxSozeZ = 5.0f;
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

	transform_.position_.y = 0 - Maxe;
}