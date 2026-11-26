#pragma once
#include "Vehicle.h"
class Car :
	public Vehicle
{
	struct CAR_STATES
	{
		float rotSpeed;
		float ACCELERATION;
		float DECELERATION;
		float ACCELERTION_MAX;
		float BACK_ACCELERTION_MAX;
		float VEHICLE_WEIGHT;

		CAR_STATES()
			:rotSpeed(0), ACCELERATION(0), DECELERATION(0), ACCELERTION_MAX(0), BACK_ACCELERTION_MAX(0), VEHICLE_WEIGHT(0)
		{
		}
		CAR_STATES(float rot, float ac, float dc, float acMax, float bacrmax, float vehicleWeight)
			:rotSpeed(rot), ACCELERATION(ac), DECELERATION(dc), ACCELERTION_MAX(acMax), BACK_ACCELERTION_MAX(bacrmax), VEHICLE_WEIGHT(vehicleWeight)
		{
		}
	};
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
	
	void SetCarNunber(int num);
	int GetCarNunber() { return CarNunber; };
	void OnCollision(GameObject* pTarget) override;
	float GetWeight() { return myStates.VEHICLE_WEIGHT; }
private:
	int hModel_;
private:

	void HandRing();
	void AcceleWerk();
	
private:
	bool isHitExplosion;
	bool isFlying;
	GameObject* pHitTeaget;
	int CarNunber;
	CAR_STATES myStates;
};