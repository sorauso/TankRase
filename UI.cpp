#include "UI.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Image.h"

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
}

void UI::Update()
{
}

void UI::Draw()
{
	transform_.position_ = { 0,0,0, };
	transform_.scale_ = { 1.0f,1.0f,1.0f };//画像の大きさの設定
	Image::SetTransform(hImage_, transform_);//画像の位置や向きの設定
	Image::Draw(hImage_);//画像の描画
}

void UI::Release()
{
}

void UI::OnCollision(GameObject* pTarget)
{
}
