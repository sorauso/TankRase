#pragma once
#include "Engine\GameObject.h"
class Obstacle :
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Obstacle(GameObject* parent);
	~Obstacle();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	void RotationGroud();
private:
	int hModel_;
	int initialUpdateCount_;
};

