#include "ClearScene.h"
#include "Engine/Image.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
namespace
{
	int counter = 0;
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
	if (counter > 30)
	{
		if (Input::IsKeyDown(DIK_P))
		{
			SceneManager* sceneManager = (SceneManager*)(this->GetParent());
			sceneManager->ChangeScene(SCENE_ID_TITLE);
		}
	}
	else
	{
		counter++;
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
