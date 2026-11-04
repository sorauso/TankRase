#include "ClearScene.h"
#include "Engine/Image.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "Engine/Timer.h"
namespace
{
	const float SCENE_CHANGE_INPUT_DELAY_FRAMES = 0.5f;
	float counter = 0;
}

ClearScene::ClearScene(GameObject* parent)
	:GameObject(parent, "ClearScene"), hImage_(-1)
{
}

void ClearScene::Initialize()
{
	hImage_ = Image::Load("CLEAR.png");
	assert(hImage_ >= 0);
	counter = 0;
}

void ClearScene::Update()
{
	counter += Timer::GetDeltaTime();
	if (counter > SCENE_CHANGE_INPUT_DELAY_FRAMES)
	{
		if (Input::IsKeyDown(DIK_P))
		{
			SceneManager* sceneManager = (SceneManager*)(this->GetParent());
			sceneManager->ChangeScene(SCENE_ID_TITLE);
		}
	}
}

void ClearScene::Draw()
{
	Image::SetTransform(hImage_,transform_);
	Image::Draw(hImage_);
}

void ClearScene::Release()
{
}
