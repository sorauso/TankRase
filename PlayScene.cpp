#include "PlayScene.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Image.h"
#include "Engine/SceneManager.h"
#include "Tank.h"
#include "Ground.h"
#include "LandmarkMother.h"
#include "UI.h"

namespace
{
	int counter = 0;;
}

PlayScene::PlayScene(GameObject* parent)
	:GameObject(parent, "PlayScene"),hModel_(-1),counter_(0)
{
}

void PlayScene::Initialize()
{
	hModel_ = Image::Load("PLAY.png");
	assert(hModel_ >= 0);

	Instantiate<Ground>(this);
	Instantiate<TankBody>(this);
	Instantiate<LandmarkMother>(this);
	Instantiate<UI>(this);
}

void PlayScene::Update()
{
	if (counter > 30)
	{
		if (Input::IsKeyDown(DIK_P))
		{
			SceneManager* sceneManager = (SceneManager*)(this->GetParent());
			sceneManager->ChangeScene(SCENE_ID_CLEAR);
		}
	}
	else
	{
		counter++;
	}
}

void PlayScene::Draw()
{
	transform_.position_ = { 0,0,0, };
	transform_.scale_ = { 1.0f,1.0f,1.0f };//画像の大きさの設定
	Image::SetTransform(hModel_, transform_);//画像の位置や向きの設定
	Image::Draw(hModel_);//画像の描画
}

void PlayScene::Release()
{
}
