#include "pch.h"
#include "ItemStructs.h"

void OnlineProdData::SetOfflineProductData(ProductWrapper& productWrapper)
{
	if (!productWrapper)
	{
		LOG("null productWrapper");
		return;
	}
	prodId = productWrapper.GetID();
	favorite = productWrapper.IsFavorited();
	//canEquip = product_wrapper.CanEquip();
	name = productWrapper.GetLongLabel().ToString();
	slot = productWrapper.GetSlot().GetSlotIndex();
	LOG("slot: {}", slot);
}

void OnlineProdData::something() {
	
}

std::string OnlineProdData::ToString() const
{
	return std::format("Name: {}, Id: {}, Fav: {}, Slot: {}, Lo: {}, Hi: {}",
		name, prodId, favorite, slot, instanceId.lower_bits, instanceId.upper_bits);
}