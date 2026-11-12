#pragma once
#include "Engine\\GameObject.h"

class Vehicle :
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Vehicle(GameObject* parent);
	Vehicle(GameObject* parent, const std::string& name);
	~Vehicle();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;
	void RotationBody(Transform& tra);
	void CalculateSlideVector(GameObject* terget, Transform mytf, XMVECTOR& ret);
	XMFLOAT3 MovingVectorCreation(Transform ts, XMVECTOR vt);
	XMFLOAT3 HitObjevtMove(GameObject* terget, Transform mytf, float pb, XMFLOAT3 Vt);
	float GetLandmarkdirection();

	float gravity;
	float verticalVelocity;
	float verticalVelocityMax;
	XMFLOAT3 GroundRotate_;
	const int STAGE_SIZE = 245;
	const float speed = 0.1f;
	XMVECTOR Vt;
	float aRotY;
};