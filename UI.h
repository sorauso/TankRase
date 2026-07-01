#pragma once
#include "Engine\GameObject.h"
class UI:
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	UI(GameObject* parent);
	~UI();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	void Draw() override;

	//開放
	void Release() override;
	void OnCollision(GameObject* pTarget) override;
private:
	int hImage_;
};

