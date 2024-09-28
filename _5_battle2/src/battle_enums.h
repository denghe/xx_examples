#pragma once

namespace Battle {


	// int array's index
	enum class StatTypes : uint32_t {
		Health,
		Mana,
		Damage,
		// ...
	};

	/**********************************************************************************************/
	/**********************************************************************************************/

	enum class ActionTypes : int32_t {
		Stun,
		Search,
		MoveToItem,
		MoveToTarget,
		HitTarget,
		SetColor,
		// ...
		MaxValue
	};
	static_assert((int32_t)ActionTypes::MaxValue <= 64);	// uint64_t actionFlags limit 

	/**********************************************************************************************/
	/**********************************************************************************************/

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

	enum class ItemTypes : uint16_t {
		Body,		// is not a item
		Potion1,
		Potion2,
		Staff1,
		Sword1,
		// ...
	};

	/**********************************************************************************************/
	/**********************************************************************************************/
	// every item have own stat list

	enum class StatTypes_Body : uint32_t {
		Health,
		HealthRegeneration,		// 0 ~ n ( percentage )

		Energy,
		EnergyRegeneration,

		Damage,					// 1 ~ n ( percentage )
		CritChance,				// 0 ~ n. n > 1: CritDamage += CritChance - 1
		CritDamage,				// 1 ~ n ( percentage )
		//CoolDown/AttackSpeed	// 0 ~ 0.99999.. ( percentage )

		Defense,				// 0 ~ n. 1 - D / ( V + D )  ==>  V / ( V + D )      ( V default == 100 )
		Evasion,				// 0 ~ n. V / ( V + E )      ( V default == 100 )
		MovementSpeed,
		// ...
	};

	enum class StatTypes_Potion1 : uint32_t {
		HealthRestore			// body.Health += HealthRestore
	};

	enum class StatTypes_Potion2 : uint32_t {
		EnergyRestore			// body.Energy += EnergyRestore
	};

	enum class StatTypes_Staff1 : uint32_t {
		BaseDamage,				// final damage = BaseDamage * body.Damage
		AimRange,
		FireballRadius,
		AttackSpeed,
		EnergyCost,
	};

	enum class StatTypes_Sword1 : uint32_t {
		BaseDamage,
		SwordLength,
		AttackSpeed,
		EnergyCost,
	};

	// ...
}
