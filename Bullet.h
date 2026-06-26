#pragma once
#include "Engine\GameObject.h"
class Bullet :
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Bullet(GameObject* parent);
	~Bullet();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	void SetMoveVector(XMFLOAT3 mv);
	void SetPosition(XMFLOAT3 mv) { transform_.position_ = mv; };
	void OnCollision(GameObject* pTarget) override;
private:
	int hModel_;
	XMFLOAT3 move_;
};

