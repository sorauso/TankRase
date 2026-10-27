#include "TitleScene.h"
#include "Engine/Image.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "Engine/Model.h"
#include "Engine/Camera.h"
namespace
{
	int counter = 0;
}

TitleScene::TitleScene(GameObject* parent)
	:GameObject(parent, "TitleScene"), hImage_(-1)
{
}

void TitleScene::Initialize()
{
	hImage_ = Image::Load("TITLE.png");
	assert(hImage_ >= 0);
	counter = 0;

	hModel_ = Model::Load("Crusader.fbx");
	assert(hModel_ >= 0);
	tankTransform_.position_ = XMFLOAT3(-150, 0, 150);
	tankTransform_.rotate_ = XMFLOAT3(0, 180, 0);

	Camera::SetPosition(XMFLOAT3(-10, 10, 0));
	Camera::SetTarget(XMFLOAT3(-30, 20, 30));
}

void TitleScene::Update()
{

	if (counter > 30)
	{
		if (Input::IsKeyDown(DIK_P))
		{
			SceneManager* sceneManager = (SceneManager*)(this->GetParent());
			sceneManager->ChangeScene(SCENE_ID_PLAY);
		}
	}
	else
	{
		counter++;
	}
}

void TitleScene::Draw()
{
	transform_.position_ = { 0,0,0, };
	transform_.scale_ = { 1.5f,1.5f,1.0f };//画像の大きさの設定
	Image::SetTransform(hImage_, transform_);//画像の位置や向きの設定
	Image::Draw(hImage_);//画像の描画

	Model::SetTransform(hModel_, tankTransform_);
	Model::Draw(hModel_);
}

void TitleScene::Release()
{
}
