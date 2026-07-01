#include "UI.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Image.h"

#include "Tank.h"

UI::UI(GameObject* parent)
	:GameObject(parent, "UI"), hImage_(-1)
{
}

UI::~UI()
{
}

void UI::Initialize()
{
	hImage_ = Image::Load("cheacle.png");
	transform_.position_ = { 0,0.8f,0, };
}

void UI::Update()
{
	TankBody* pTk = (TankBody*)FindObject("TankBody");
	float dir = pTk->GetLandmarkdirection();
	transform_.position_.x = dir;
}

void UI::Draw()
{
	Image::SetTransform(hImage_, transform_);//画像の位置や向きの設定
	Image::Draw(hImage_);//画像の描画
}

void UI::Release()
{
}

void UI::OnCollision(GameObject* pTarget)
{
}
