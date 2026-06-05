#include "TitleScene.h"
#include "Engine/Image.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
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
}

void TitleScene::Release()
{
}
