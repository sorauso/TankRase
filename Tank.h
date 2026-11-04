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
	float gravity;
	float verticalVelocity;
	float verticalVelocityMax;
	XMFLOAT3 GroundRotate_;
	XMFLOAT3 turretDirection;
private:
	void RotationTank();
	void SetTpsRotCamera();
	void UpdateTurret();  // 砲塔の動きを更新
	void Fire();    // 射撃処理を更新
	void UpdateMove();    // 移動処理を更新
	XMFLOAT3 MovingVectorCreation(Transform ts, XMVECTOR vt);
};

