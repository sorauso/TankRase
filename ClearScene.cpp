#include "ClearScene.h"
#include "Engine/Image.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "Engine/Timer.h"
#include "Engine/Model.h"
#include "Engine/Camera.h"
#include "Engine/GameData.h"
namespace
{
	const float SCENE_CHANGE_INPUT_DELAY_FRAMES = 0.5f;
	float counter = 0;
}

ClearScene::ClearScene(GameObject* parent)
	:GameObject(parent, "ClearScene"), hImage_(-1),hModel_(-1)
{
}

void ClearScene::Initialize()
{
	hImage_ = Image::Load("CLEAR.png");
	assert(hImage_ >= 0);
	counter = 0;

	hModel_ = Model::Load("Crusader.fbx");
	assert(hModel_ >= 0);
	tankTransform_.position_ = XMFLOAT3(-200, 0, 200);
	tankTransform_.rotate_ = XMFLOAT3(0, 180, 0);

	Camera::SetPosition(XMFLOAT3(0, 170, 0));
	Camera::SetTarget(XMFLOAT3(-150, 0, 270));

	GameData* gameData = (GameData*)this->GetParent()->GetParent()->FindChildObject("GameData");
	std::vector<int> landmarkOwners_;
	landmarkOwners_ = gameData->GetLandmarkOwners();
	for (int i = 0;i < landmarkOwners_.size();i++)
	{
		char buf[256];
		sprintf_s(buf, "%d\n", landmarkOwners_[i]);
		OutputDebugStringA(buf);
	}
}

void ClearScene::Update()
{
	counter += Timer::GetDeltaTime();
	if (counter > SCENE_CHANGE_INPUT_DELAY_FRAMES)
	{
		if (Input::IsKeyDown(DIK_SPACE))
		{
			SceneManager* sceneManager = (SceneManager*)(this->GetParent());
			sceneManager->ChangeScene(SCENE_ID_TITLE);
		}
	}

	tankTransform_.rotate_.y += 0.5f;
}

void ClearScene::Draw()
{
	Image::SetTransform(hImage_,transform_);
	Image::Draw(hImage_);

	Model::SetTransform(hModel_, tankTransform_);
	Model::Draw(hModel_);
}

void ClearScene::Release()
{
}
