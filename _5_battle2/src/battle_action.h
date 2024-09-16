#pragma once

namespace Battle {

	enum class ActionTypes : int32_t {
		Stun,
		SearchTarget,
		MoveToTarget,
		HitTarget,
		SetColor,
		// ...
		MaxValue
	};
	static_assert((int32_t)ActionTypes::MaxValue <= 64);	// uint64_t actionFlags limit 

	/*********************************************************************************************/
	// base data struct

	struct alignas(8) Action {
		union {
			std::array<uint64_t, 2> _;	// todo: resize?
			struct {
				ActionTypes type;
				int32_t __;
			};
		};
	};

	template<typename A>
	constexpr bool ActionStructCheck = alignof(Action) == alignof(A) && sizeof(A) <= sizeof(Action);

	/*********************************************************************************************/
	// Actions

	struct alignas(8) Action_Stun {
		static constexpr ActionTypes cType{ ActionTypes::Stun };
		ActionTypes type;
		float timeout;
	};
	static_assert(ActionStructCheck<Action_Stun>);


	struct alignas(8) Action_SearchTarget {
		static constexpr ActionTypes cType{ ActionTypes::SearchTarget };
		ActionTypes type;
		float searchRange;
		float timeout;
	};
	static_assert(ActionStructCheck<Action_SearchTarget>);


	struct alignas(8) Action_MoveToTarget {
		static constexpr ActionTypes cType{ ActionTypes::MoveToTarget };
		ActionTypes type;
		float movementSpeed, distanceLimit;
		float timeout;
	};
	static_assert(ActionStructCheck<Action_MoveToTarget>);


	struct alignas(8) Action_HitTarget {
		static constexpr ActionTypes cType{ ActionTypes::HitTarget };
		ActionTypes type;
		float distanceLimit, castDelaySeconds;
		float timeout;
	};
	static_assert(ActionStructCheck<Action_HitTarget>);


	struct alignas(8) Action_SetColor {
		static constexpr ActionTypes cType{ ActionTypes::SetColor };
		ActionTypes type;
		xx::RGBA8 color;
		float timeout;
	};
	static_assert(ActionStructCheck<Action_SetColor>);

	// ...
}
