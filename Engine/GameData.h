#pragma once
#include "GameObject.h"
#include <vector>

class GameData : public GameObject
{
public:
	GameData(GameObject* parent);

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	void AddLandmarkOwner(int tankNumber)
	{
		landmarkOwners_.push_back(tankNumber);
	}

	const std::vector<int>& GetLandmarkOwners() const
	{
		return landmarkOwners_;
	}
private:
	std::vector<int> landmarkOwners_;
};