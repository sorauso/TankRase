#pragma once
#include "Vehicle.h"
class TankBody:
	public Vehicle
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
	void OnCollision(GameObject* pTarget) override;
private:
	int hModel_;
	XMFLOAT3 turretDirection;
private:
	void SetTpsRotCamera();
	void UpdateTurret();  // 砲塔の動きを更新
	void Fire();    // 射撃処理を更新
	void UpdateMove();    // 移動処理を更新
};

