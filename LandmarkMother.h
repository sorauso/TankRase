#pragma once
#include "Engine\GameObject.h"

class LandmarkMother:
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	LandmarkMother(GameObject* parent);
	~LandmarkMother();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	void Draw() override;

	//開放
	void Release() override;
	void OnCollision(GameObject* pTarget) override;
	void MakeNextLandmark();
	void GetAcquisitionCarNunber(int num);
private:
	int hModel_;
	XMFLOAT3 move_;
	int counter_;
};


