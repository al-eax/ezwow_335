#pragma once

#include <inttypes.h>
#include <sstream>

#include "lua_functions.hpp"
#include "utils.hpp"

enum item_rarity : uint8_t
{
    item_rarity_grey = 0,
    item_rarity_white = 1,
    item_rarity_green = 2,
    item_rarity_blue = 3,
    item_rarity_epic = 4,
    item_rarity_legendary = 5,
    item_rarity_artifact = 6,
};

struct WoWInventory
{
    struct InventoryItem
    {
        int bag_index;
        int slot_index;
        std::string name;
        std::string link;
        item_rarity rarity;
        uint32_t level;
        uint32_t min_level;
        std::string type;
        std::string sub_type;
        uint32_t stack_count;
        std::string equip_location;
        std::string texture;
        uint32_t sell_price;

        void use()
        {
            std::stringstream ss;
            ss << "UseContainerItem(" << bag_index << "," << slot_index << ")";
            Lua::execute(ss.str());
        }

        std::string to_str()
        {
            return "(" + std::to_string(bag_index) + "," + std::to_string(slot_index) + ") " + name + " | " + link;
        }
      
    };

    std::vector<InventoryItem> get_items()
    {
        std::vector<InventoryItem> items;
        for (int bag_index = 0; bag_index <= 4; bag_index++)
        {
            auto slot_count_str = Lua::execute("slotCount = GetContainerNumSlots(" + std::to_string(bag_index) + ");", "slotCount");
            if (slot_count_str.empty())
                continue;
            int slot_count = std::stoi(slot_count_str);
            for (int slot_index = 0; slot_index < slot_count; slot_index++)
            {
                std::stringstream ss;
                ss << "itemLink = GetContainerItemLink(" << bag_index << ", " << slot_index << ");";
                auto item_link = Lua::execute(ss.str(), "itemLink");
                if (item_link.empty())
                    continue;

                Lua::execute("itemName, itemLink, itemRarity, itemLevel, itemMinLevel, itemType, itemSubType,"
                             "itemStackCount, itemEquipLoc, itemTexture, itemSellPrice = GetItemInfo( '" +
                             item_link + "');");

                items.push_back({
                    bag_index,
                    slot_index,
                    Lua::get_text("itemName"),
                    Lua::get_text("itemLink"),
                    (item_rarity)std::stoi(Lua::get_text("itemRarity")),
                    std::stoi(Lua::get_text("itemLevel")),
                    std::stoi(Lua::get_text("itemMinLevel")),
                    Lua::get_text("itemType"),
                    Lua::get_text("itemSubType"),
                    std::stoi(Lua::get_text("itemStackCount")),
                    Lua::get_text("itemEquipLoc"),
                    Lua::get_text("itemTexture"),
                    std::stoi(Lua::get_text("itemSellPrice")),
                });
            }
        }
        return items;
    }

    int get_free_slots()
    {
        auto lua_str = R"(
            freeSlotCount = 0;
            for bag = 0,4 do
                freeSlots, bagType = GetContainerNumFreeSlots(bag);
                freeSlotCount = freeSlotCount + freeSlots;
            end
        )";
        auto free_slot_count_str = Lua::execute(lua_str, "freeSlotCount");
        if (free_slot_count_str.empty())
            return 0;
        return std::stoi(free_slot_count_str);
    }
};

/*
local function GreySell()
  for bag=0,4 do
    for slot=0,GetContainerNumSlots(bag) do
      local link = GetContainerItemLink(bag, slot)
      if link and select(3, GetItemInfo(link)) == 0 then
        ShowMerchantSellCursor(1)
        UseContainerItem(bag, slot)
      end
    end
  end
end

local f = CreateFrame("Frame")
f:RegisterEvent("MERCHANT_SHOW")
f:SetScript("OnEvent", GreySell)
if MerchantFrame:IsVisible() then GreySell() end

    public void SellAllGrayItems()
        {
            LuaDoString("
            local p,N,n=0 
            for b=0,4 do for s=1,GetContainerNumSlots(b) do
                n=GetContainerItemLink(b,s) 
                if n and string.find(n,\"9d9d9d\") then 
                    N={GetItemInfo(n)}
                    p=p+N[11]
                    UseContainerItem(b,s) 
                end 
            end 
            end");
        }


*/
