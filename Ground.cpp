#include "Ground.h"
#include "Engine/Model.h"
#include "Obstacle.h"

Ground::Ground(GameObject* parent)
	:GameObject(parent, "Ground"), hModel_(-1)
{
}

Ground::~Ground()
{
}

void Ground::Initialize()
{
	hModel_ = Model::Load("Ground.fbx");
	assert(hModel_ >= 0);
	transform_.scale_ = XMFLOAT3(10,10,10);

	Obstacle *ob = Instantiate<Obstacle>(this);
	ob->SetPosition(XMFLOAT3(-200, 0, 200));
}

void Ground::Update()
{

}

void Ground::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Ground::Release()
{
}
