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
	const int MOVE_LIMIT = 40;
	const float POSITION_NUM = 0.02f;
	TankBody* pTk = (TankBody*)FindObject("TankBody");
	float dir = pTk->GetLandmarkdirection();
	if (dir > MOVE_LIMIT) { dir = MOVE_LIMIT; }
	else if (dir < -MOVE_LIMIT) { dir = -MOVE_LIMIT; }
	transform_.position_.x = dir * POSITION_NUM;
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
