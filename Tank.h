#pragma once
#include "Engine\\GameObject.h"
class TankBody:
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	TankBody(GameObject* parent);
	~TankBody();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;
	float GetLandmarkdirection();
private:
	int hModel_;
private:
	void RotationTank();
	void SetTpsRotCamera();

	XMFLOAT3 MovingVectorCreation(Transform ts, XMVECTOR vt);
};

class TankHead:
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	TankHead(GameObject* parent);
	~TankHead();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;
private:
	int hModel_;
};

