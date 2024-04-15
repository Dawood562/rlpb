#include "pch.h"
#include "InventoryModel.h"
#include <ranges>

InventoryModel::InventoryModel(std::shared_ptr<GameWrapper> gw) : m_gw(std::move(gw))
{
	auto items = m_gw->GetItemsWrapper();
	specialEditionDb = items.GetSpecialEditionDB();
	paintDatabase = items.GetPaintDB();
	LOG("Items::Init-products");
	InitProducts();
}


std::string InventoryModel::GetSlotName(int slotNo) {
	switch (slotNo) {
	case 0:
		return "Body";
	case 1:
		return "Decal";
	case 2:
		return "Wheels";
	case 3:
		return "Boost";
	case 4:
		return "Antenna";
	case 5:
		return "Topper";
	case 6:
		return "Paint";
	case 7:
		return "PaintFinish";
	case 12:
		return "PaintAlt";
	case 14:
		return "Trail";
	case 15:
		return "GoalExplosion";
	case 16:
		return "Banner";
	case 20:
		return "AvatarBorder";
	case 21:
		return "PlayerTitle";
	}
	return "";
}

OnlineProdData InventoryModel::GetProdData(OnlineProductWrapper& onlineProd)
{
	OnlineProdData data;
	if (!onlineProd)
	{
		DEBUGLOG("null online product");
		return data;
	}

	auto productWrapper = onlineProd.GetProduct();
	data.SetOfflineProductData(productWrapper);
	data.instanceId = onlineProd.GetInstanceIDV2();
	data.favorite = onlineProd.IsFavorited();
	std::string seEdition;

	auto attributes = onlineProd.GetAttributes();
	for (auto attribute : attributes)
	{
		auto attributeType = attribute.GetAttributeType();
		if (attributeType == "ProductAttribute_Painted_TA")
		{
			auto paintAttribute = ProductAttribute_PaintedWrapper(attribute.memory_address);
			const auto paintId = paintAttribute.GetPaintID();
			auto label = paintAttribute.GetSortLabel().ToString();
			if (label.find("Painted") != std::string::npos)
			{
				data.paint = label.substr(7);
			}
			//data.paint = paintDatabase.GetPaintName(paintId);
			continue;
		}
		if (attributeType == "ProductAttribute_SpecialEdition_TA")
		{
			if (auto specialEditionAttribute = ProductAttribute_SpecialEditionWrapper(attribute.memory_address))
			{
				auto label = specialEditionDb.GetSpecialEditionName(specialEditionAttribute.GetEditionID());
				if (label.find("Edition_") != std::string::npos)
				{
					seEdition = label.substr(8) + " ";
				}
			}
		}
	}
	if (!data.paint.empty() || !seEdition.empty())
	{
		data.name += " (" + seEdition + data.paint + ")";
	}

	return data;
}

OnlineProdData InventoryModel::GetProdData(const ProductInstanceID& instanceId)
{
	auto items = m_gw->GetItemsWrapper();
	if (!items) return {};
	if (auto onlineProduct = items.GetOnlineProduct(instanceId))
	{
		return GetProdData(onlineProduct);
	}
	OnlineProdData item;
	const auto productId = instanceId.lower_bits;
	if (auto product = items.GetProduct(productId))
	{
		// Non default items can't be equipped without a valid instance id..
		if (product.GetUnlockMethod() != 0 /*EUnlockMethod_UnlockMethod_Default*/)
		{
			return item;
		}
		item.SetOfflineProductData(product);
		item.instanceId = instanceId;
	}
	else
	{
		LOG("no product: {}:{}", static_cast<long>(instanceId.lower_bits), static_cast<long>(instanceId.upper_bits));
	}
	return item;
}


void InventoryModel::InitProducts()
{
	m_products.clear();

	auto items = m_gw->GetItemsWrapper();

	auto onlineProductWrappers = items.GetOwnedProducts();
	auto c = onlineProductWrappers.Count();
	LOG("OwnedProducts: {}", c);

	for (auto onlineProd : onlineProductWrappers)
	{
		auto data = GetProdData(onlineProd);
		m_products[data.slot].push_back(data);
		//DEBUGLOG("Online Item: {}", data.ToString());
	}
	auto productWrappers = items.GetCachedUnlockedProducts();
	auto c2 = productWrappers.Count();
	//LOG("CachedUnlockedProducts: {}", c2);
	for (auto unlockedProduct : productWrappers)
	{
		if (unlockedProduct.IsNull()) continue;
		unsigned long long id = unlockedProduct.GetID();
		auto data = GetProdData(ProductInstanceID{ 0, id });
		if (data.prodId != 0)
		{
			m_products[data.slot].push_back(data);
			//DEBUGLOG("Cached Item: {}", data.ToString());
		}
	}

	for (auto& [slotIndex, slotProducts] : m_products)
	{
		// TODO add DefaultProduct_New to slot wrapper?
		//auto slot = GetSlot(slotIndex);
		//if (slot && !slot->_DefaultProduct_New())

		static std::set noDefaultItemSlots = { 1, 4, 5, 13 };
		if (noDefaultItemSlots.contains(slotIndex))
		{
			OnlineProdData defaultItem;
			defaultItem.name = " None";
			defaultItem.slot = slotIndex;
			slotProducts.push_back(defaultItem);
		}
		// TODO: We should eliminate duplicates here. They occur when merging online and offline data. Matching by name & slot should be enough. We should prefer Online version as it has instance ID.
		std::ranges::sort(slotProducts, {}, &OnlineProdData::name);
	}

	m_products[12] = m_products[7];
	for (auto& data : m_products[12])
	{
		data.slot = 12;
	}
}

const std::set<int>& InventoryModel::GetForcedSlotForBody(int bodyId)
{
	//TODO add body asset equip profile wrappers to make this feature work again
	if (auto it = m_forcedProductSlotCache.find(bodyId); it != m_forcedProductSlotCache.end())
	{
		return it->second;
	}
	static std::set<int> emptySet{};
	return emptySet;
}

const std::vector<OnlineProdData>& InventoryModel::GetSlotProducts(int slot)
{
	if (auto it = m_products.find(slot); it != m_products.end())
	{
		return it->second;
	}
	static std::vector<OnlineProdData> emptyVector{};
	return emptyVector;
}