#pragma once

namespace Battle {

	// todo

	enum class ItemTypes {
		Unknown,
		Sword,
		Staff
		// ...
	};

	/*********************************************************************************************/
	// base data struct

	struct alignas(8) ItemData {
		union {
			std::array<uint64_t, 2> _;	// todo: resize?
			struct {
				ItemTypes type;
				int32_t __;
			};
		};
	};
	template<typename T>
	constexpr bool ItemDataCheck = alignof(ItemData) == alignof(T) && sizeof(T) <= sizeof(ItemData);

	/*********************************************************************************************/
	// datas

	struct alignas(8) ItemData_Sword {
		static constexpr ItemTypes cType{ ItemTypes::Sword };
		ItemTypes type;
		// todo
	};
	static_assert(ItemDataCheck<ItemData_Sword>);

	struct alignas(8) ItemData_Staff {
		static constexpr ItemTypes cType{ ItemTypes::Staff };
		ItemTypes type;
		// todo
	};
	static_assert(ItemDataCheck<ItemData_Staff>);

	// ...

	/*********************************************************************************************/

	struct Monster;
	struct Item : ItemData {
		xx::SpaceWeak<Monster> owner;			// == null mean on floor
		XY pos{};								// floor pos or owner's offset pos
		float radians{};
		int32_t lineNumber{};

		void Init(xx::SpaceWeak<Monster> const& owner_, XY const& pos_, float radians_);
		int32_t UpdateCore();
		bool Update();
	};

}
