#pragma once
#include "Engine\\GameObject.h"
class Car :
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Car(GameObject* parent);
	~Car();
	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;
	float GetLandmarkdirection();
	void SetCarNunber(int num);
	int GetCarNunber() { return CarNunber; };
	void OnCollision(GameObject* pTarget) override;
private:
	int hModel_;
private:
	void RotationCar();
	XMFLOAT3 HitObjevtMove(GameObject* terget, Transform mytf, float pb, XMFLOAT3 Vt);
	void HandRing();
	void AcceleWerk();
	XMFLOAT3 MovingVectorCreation(Transform ts, XMVECTOR vt);
private:
	XMVECTOR Vt;
	float aRotY;
	bool isHitCar;
	bool isHitTank;
	bool isHitExplosion;
	bool isFlying;
	GameObject* pHitTeaget;
	int CarNunber;
	unsigned int groundFollowFrameCount;
	float gravity;
	float verticalVelocity;
	float verticalVelocityMax;
	XMFLOAT3 GroundRotate_;

	struct CAR_STATES
	{
		float rotSpeed;
		float ACCELERATION;
		float DECELERATION;
		float ACCELERTION_MAX;
		float BACK_ACCELERTION_MAX;

		CAR_STATES()
			:rotSpeed(0), ACCELERATION(0), DECELERATION(0), ACCELERTION_MAX(0), BACK_ACCELERTION_MAX(0)
		{
		}
		CAR_STATES(float rot, float ac, float dc, float acMax, float bacrmax)
			:rotSpeed(rot), ACCELERATION(ac), DECELERATION(dc), ACCELERTION_MAX(acMax), BACK_ACCELERTION_MAX(bacrmax)
		{
		}
	};
	CAR_STATES myStates;
};