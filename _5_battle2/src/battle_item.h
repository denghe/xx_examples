#pragma once

namespace Battle {

	// todo

	enum class ItemEquipmentLocations : uint8_t {
		Bag,
        Helm,
        Amulet,
        Shoulder,
        Armor,
        Belt,
        Pants,
        Glove,
        Boots,
        Ring,
        Weapon,
        Shield,
        Accessory,
		// ...
	};

	enum class ItemQualities : uint8_t {
		Normal,
		Excellent,
		Rare,
		Epic,
		Legendary,
		Ancient,
		Archaic
		// ...
	};

	enum class StatTypes {
		Health, 
		Damage,
		// ...
	};

	// Monster's part
	struct alignas(8) StatArea {
		int32_t health;
		int32_t damage;
		// ...

		// todo: operator +=
	};

	// StatArea calculate result
	struct StatResult {
		// todo
	};

	// StatPack's item
	struct alignas(8) StatNode {
		StatTypes type;
		int32_t value;
	};

	// maybe fixed array + len is better?
	// or List Pool ?
	using StatPack = xx::Listi32<StatNode>;

	// todo: StatPack to StatArea function


	enum class ItemTypes : uint16_t {
		Potion1,
		Staff1,
		Sword1,
		// ...
	};


	enum class SkillTypes : uint32_t {
		Unknown,
		HealthAdd,
		// ...
	};

	struct Monster;
	struct Skill {
		typedef bool (*FuncType)(Monster* caster, Monster* target, int32_t* args);
		static bool HealthAdd(Monster* caster, Monster* target, int32_t* args);
		// ...
		inline static FuncType funcs[] = {
			HealthAdd
			// ...
		};
	};

	// for easy copy( ItemTemplate to Item )
	struct ItemInfo {
		ItemTypes type;
		ItemQualities qualitity;
		ItemEquipmentLocations equipmentLocation;
		int32_t quantityLimit;
	};

	// for generate item
	struct ItemTemplate {
		ItemInfo info;
		// todo: stat list & range ?
	};

	struct Item {
		xx::SpaceWeak<Monster> owner;			// == null mean on floor
		XY pos{};								// floor pos or owner's offset pos
		float radians{};						// for display

		ItemInfo info;							// copy from template

		xx::TinyList<StatNode> statPack;		// for generate StatArea

		SkillTypes skillType;					// Unknown: none
		int32_t skillArgs[3];					// for call skill funcs args
	};


	/*********************************************************************************************/
	// base data struct


	//struct alignas(8) ItemDataBase {
	//	ItemTypes type;
	//	ItemQualities qualitity;
	//	ItemEquipmentLocations equipmentLocation;
	//	int32_t quantityLimit;
	//};

	//struct alignas(8) ItemData {
	//	union {
	//		ItemDataBase base;
	//		std::array<uint64_t, 2> _;	// todo: resize?
	//	};
	//};
	//template<typename T>
	//constexpr bool ItemDataCheck = alignof(ItemData) == alignof(T) && sizeof(T) <= sizeof(ItemData);

	/*********************************************************************************************/
	// datas

	//struct alignas(8) ItemData_Potion1 {
	//	static constexpr ItemTypes cType{ ItemTypes::Potion1 };
	//	ItemDataBase base;
	//	Skill::Types skillType;
	//	int32_t health;

	//	void Init();
	//	bool Cast(Monster* caster, Monster* target);
	//};
	//static_assert(ItemDataCheck<ItemData_Potion1>);


	//struct alignas(8) ItemData_Sword {
	//	ItemTypes type;
	//	ItemEquipmentLocations equipLocation;
	//	// todo
	//};
	//static_assert(ItemDataCheck<ItemData_Sword>);


	//struct alignas(8) ItemData_Staff {
	//	ItemTypes type;
	//	ItemEquipmentLocations equipLocation;
	//	// todo
	//};
	//static_assert(ItemDataCheck<ItemData_Staff>);

	// ...

	/*********************************************************************************************/


}
