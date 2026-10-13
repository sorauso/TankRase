#pragma once
#include "Engine\GameObject.h"
class Goal :
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Goal(GameObject* parent);
	~Goal();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	void RotationGroud();
	void OnCollision(GameObject* pTarget) override;
private:
	int hModel_;
	XMFLOAT3 move_;
	GameObject* pAcquisitionCar;
};

