#pragma once
#include "looper.h"

namespace Battle {

	// simulate 1 monster walk through, random stun
	// action == skill == behavior == buff

	// todo: AI actions
	// todo: random position generate & search & move to & hit

	/*********************************************************************************************/

	enum class ActionTypes : int32_t {
		Move,	// deprecated
		Stun,
		AI_SearchTarget,
		AI_MoveToTarget,
		AI_HitTarget,
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
	constexpr bool ActionStructCheck = sizeof(Action) >= sizeof(A) && alignof(Action) == alignof(A);

	/*********************************************************************************************/
	// Actions

	struct alignas(8) Action_Move {
		static constexpr ActionTypes cType{ ActionTypes::Move };
		ActionTypes type;
		float speed;
	};
	static_assert(ActionStructCheck<Action_Move>);


	struct alignas(8) Action_Stun {
		static constexpr ActionTypes cType{ ActionTypes::Stun };
		ActionTypes type;
		int32_t timeoutFrameNumber;
	};
	static_assert(ActionStructCheck<Action_Stun>);


	struct alignas(8) Action_AI_SearchTarget {
		static constexpr ActionTypes cType{ ActionTypes::AI_SearchTarget };
		ActionTypes type;
		float searchRange, castDelaySeconds;
		int32_t timeoutFrameNumber;
	};
	static_assert(ActionStructCheck<Action_AI_SearchTarget>);


	struct alignas(8) Action_AI_MoveToTarget {
		static constexpr ActionTypes cType{ ActionTypes::AI_MoveToTarget };
		ActionTypes type;
		float movementSpeed, distanceLimit;
		int32_t timeoutFrameNumber;
	};
	static_assert(ActionStructCheck<Action_AI_MoveToTarget>);


	struct alignas(8) Action_AI_HitTarget {
		static constexpr ActionTypes cType{ ActionTypes::AI_HitTarget };
		ActionTypes type;
		float distanceLimit;
	};
	static_assert(ActionStructCheck<Action_AI_HitTarget>);

	// ...

	/*********************************************************************************************/

	struct Scene;
	struct Monster {
		// for public use
		Scene* scene{};
		XY pos{}, movementDirection{};
		int32_t timeoutFrameNumber;
		xx::SpaceWeak<Monster> target;
		// ...

		void Init(Scene* scene_);
		int32_t Update();
		void TryAddBaseActions();

		/***************************************************/
		int32_t id{};
		int32_t actionsLen{};
		uint64_t actionFlags{};
		Action actions[2];				// todo: set more cap

		bool ActionExists(ActionTypes bt);						// return true: exists
		void ActionSetFlag(ActionTypes bt);
		void ActionClearFlag(ActionTypes bt);
		int32_t ActionFind(ActionTypes bt);						// return -1: not found
		void ActionRemove(ActionTypes bt, int32_t index);
		bool ActionTryRemove(ActionTypes bt);					// return -1: not found

		template<typename T> bool ActionExists();
		template<typename T> void ActionSetFlag();
		template<typename T> void ActionClearFlag();
		template<typename T> int32_t ActionFind();
		template<typename T> void ActionRemove(int32_t index);
		template<typename T> bool ActionTryRemove();
		template<typename...AS> void ActionTryRemoves();
		template<typename T> T& ActionAdd();
		/***************************************************/

		void Add_Action_Move(float speed);
		void Add_Action_Stun(float durationSeconds);
		void Add_Action_AI_SearchTarget(float searchRange, float castDelaySeconds);
		void Add_Action_AI_MoveToTarget(float movementSpeed, float distanceLimit, float timeoutSeconds);
		void Add_Action_AI_HitTarget(float distanceLimit);
		// ...

		void Case_(Action_Move& b, int32_t frameNumber, int32_t index);
		void Case_(Action_Stun& b, int32_t frameNumber, int32_t index);
		void Case_(Action_AI_SearchTarget& b, int32_t frameNumber, int32_t index);
		void Case_(Action_AI_MoveToTarget& b, int32_t frameNumber, int32_t index);
		void Case_(Action_AI_HitTarget& b, int32_t frameNumber, int32_t index);
		// ...

		/***************************************************/

		// for logic call
		void Stun(float durationSeconds);
		// ...
	};

	/*********************************************************************************************/

	struct Scene {
		int32_t frameNumber{};
		int32_t autoId{};
		XY screenAreaMin{}, screenAreaMax{};
		xx::SpaceGrid<Monster> monsters;
		xx::Rnd rnd;
		// todo: wall ?

		void Init();
		void BeforeUpdate();
		int32_t Update();
		void Draw();
	};

};

#include "battle_scene.hpp"
#include "battle_monster.hpp"
#include "battle_monster_action_add.hpp"
#include "battle_monster_action_case.hpp"
#include "battle_monster_action_utils.hpp"
#include "battle_monster_actions.hpp"
// ...
