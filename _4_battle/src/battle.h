#pragma once
#include "looper.h"

namespace Battle {

	// simulate 1 monster walk through, random stun
	// action == skill == behavior == buff

	// todo: AI actions
	// todo: random position generate & search & move to & hit

	/*********************************************************************************************/

	enum class ActionTypes : int32_t {
		Move,
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
		std::array<uint64_t, 2> _;	// todo: change length for cross all actions
		struct {
		ActionTypes type;
		int32_t __;
		};
		};
	};

	template<typename A>
	constexpr bool ActionStructCheck = sizeof(Action) >= sizeof(A) && alignof(Action) == alignof(A);

	/*********************************************************************************************/
	// Action's union types ... pod & sizeof(T) <= sizeof(Action)

	struct alignas(8) Action_Move {
		ActionTypes type;
		float speed;
	};
	static_assert(ActionStructCheck<Action_Move>);

	struct alignas(8) Action_Stun {
		ActionTypes type;
		int32_t timeoutFrameNumber;
	};
	static_assert(ActionStructCheck<Action_Stun>);

	struct alignas(8) Action_AI_SearchTarget {
		ActionTypes type;
		float searchRange, castDelaySeconds;
		int32_t timeoutFrameNumber;
	};
	static_assert(ActionStructCheck<Action_AI_SearchTarget>);

	struct alignas(8) Action_AI_MoveToTarget {
		ActionTypes type;
		float movementSpeed, distanceLimit;
		int32_t timeoutFrameNumber;
	};
	static_assert(ActionStructCheck<Action_AI_MoveToTarget>);

	struct alignas(8) Action_AI_HitTarget {
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
		Action actions[2];		// todo: set more cap

		bool ActionExists(ActionTypes bt);
		void ActionSetFlag(ActionTypes bt);
		void ActionClearFlag(ActionTypes bt);
		// return -1 mean not found
		int32_t ActionFind(ActionTypes bt);
		// call after ActionFind
		void ActionRemove(ActionTypes bt, int32_t index);
		// return -1 mean not found
		bool ActionRemove(ActionTypes bt);
		/***************************************************/

		bool ActionAdd_Move(float speed);
		bool ActionAdd_Stun(int32_t numFrames);
		bool ActionAdd_AI_SearchTarget(float searchRange, float castDelaySeconds);
		bool ActionAdd_AI_MoveToTarget(float movementSpeed, float distanceLimit);
		bool ActionAdd_AI_HitTarget(float distanceLimit);
		// ...

		void ActionCall_Move(Action_Move& b, int32_t frameNumber, int32_t index);
		void ActionCall_Stun(Action_Stun& b, int32_t frameNumber, int32_t index);
		void ActionCall_AI_SearchTarget(Action_Move& b, int32_t frameNumber, int32_t index);
		void ActionCall_AI_MoveToTarget(Action_Stun& b, int32_t frameNumber, int32_t index);
		void ActionCall_AI_HitTarget(Action_Stun& b, int32_t frameNumber, int32_t index);
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
#include "battle_monster_action_util.hpp"
#include "battle_monster_actions.hpp"
// ...
