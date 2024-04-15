#pragma once

struct OnlineProdData {
	int prodId = 0;
	std::string name;
	ProductInstanceID instanceId = { 0,0 };
	std::string paint;
	int slot = -1;
	bool canEquip = false;
	bool favorite{ false };

	void SetOfflineProductData(ProductWrapper& productWrapper);

	std::string ToString() const;
};

struct PaintData
{
	int primaryId = 0;
	int accentId = 0;
	LinearColor primaryColour = {};
	LinearColor accentColour = {};
};

struct PresetData {
	std::string name;
	PaintData blueCol;
	PaintData orangeCol;
	std::vector<OnlineProdData> blueLoadout = {};
	std::vector<OnlineProdData> orangeLoadout = {};
};