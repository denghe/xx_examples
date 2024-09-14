#pragma once

namespace Battle {

	XX_INLINE void Item::Init(xx::SpaceWeak<Monster> const& owner_, XY const& pos_, float radians_) {
		owner = owner_;
		pos = pos_;
		radians = radians_;
	}

	template<typename T>
	XX_INLINE T& Item::DataInit() {
		((T&)data).Init();
		return (T&)data;
	}

	template<typename T>
	XX_INLINE T& Item::DataRef() const {
		assert(((T&)data).base.type == T::cType);
		return (T&)data;
	}

	XX_INLINE bool Skill::HealthAdd(Monster* caster, Monster* target, int32_t* args) {
		// todo

		return false;
	}

	XX_INLINE void ItemData_Potion1::Init() {
		base.type = cType;
		base.qualitity = ItemQualities::Normal;
		base.equipmentLocation = ItemEquipmentLocations::Bag;
		base.quantityLimit = 999;
		skillType = Skill::Types::HealthAdd;
		health = 50;
	}

	XX_INLINE bool ItemData_Potion1::Cast(Monster* caster, Monster* target) {
		return Skill::funcs[(size_t)skillType](caster, target, &health);
	}
};
