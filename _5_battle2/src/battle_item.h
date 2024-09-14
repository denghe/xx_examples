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

	//enum class ItemDataTypes {
	// StatPack
	// QuantityPack
	//};

	enum class ItemTypes : uint16_t {
		Potion1,
		Staff1,
		Sword1,
		// ...
	};

	struct Monster;
	struct Skill {
		typedef bool (*FuncType)(Monster* caster, Monster* target, int32_t* args);
		static bool HealthAdd(Monster* caster, Monster* target, int32_t* args);
		// ...
		inline static FuncType funcs[] = {
			Skill::HealthAdd
			// ...
		};
		enum class Types : uint32_t {
			HealthAdd,
			// ...
		};
	};


	/*********************************************************************************************/
	// base data struct


	struct alignas(8) ItemDataBase {
		ItemTypes type;
		ItemQualities qualitity;
		ItemEquipmentLocations equipmentLocation;
		int32_t quantityLimit;
	};

	struct alignas(8) ItemData {
		union {
			ItemDataBase base;
			std::array<uint64_t, 2> _;	// todo: resize?
		};
	};
	template<typename T>
	constexpr bool ItemDataCheck = alignof(ItemData) == alignof(T) && sizeof(T) <= sizeof(ItemData);

	/*********************************************************************************************/
	// datas

	struct alignas(8) ItemData_Potion1 {
		static constexpr ItemTypes cType{ ItemTypes::Potion1 };
		ItemDataBase base;
		Skill::Types skillType;
		int32_t health;

		void Init();
		bool Cast(Monster* caster, Monster* target);
	};
	static_assert(ItemDataCheck<ItemData_Potion1>);


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

	struct Monster;
	struct Item {
		// public
		xx::SpaceWeak<Monster> owner;			// == null mean on floor
		XY pos{};								// floor pos or owner's offset pos
		float radians{};
		// private
		ItemData data;

		template<typename T>
		T& DataInit();

		template<typename T>
		T& DataRef() const;

		void Init(xx::SpaceWeak<Monster> const& owner_, XY const& pos_, float radians_);
	};

}
