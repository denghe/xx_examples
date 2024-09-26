#pragma once

namespace Battle {

	struct Scene;
	struct Monster : Drawable {
		static constexpr float cRadius{ 32 };
		static constexpr float cSearchDistance{ 1000 };
		static constexpr xx::RGBA8 cColor{ xx::RGBA8_White };

		StatInfo statInfo, statInfoMax;
		xx::Listi32<xx::Shared<Item>> skills;					// todo: ItemEquipmentLocations map? event map?

		xx::SpaceGridWeak<Monster> targetMonster;
		xx::Weak<Item> targetItem;
		XY movementDirection{};
		bool isDead{};

		// ...

		/***************************************************/
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
		template<typename T> int32_t ActionFind();
		template<typename T> void ActionRemove(int32_t index);
		template<typename T> void ActionRemove(T& o);
		template<typename T> void ActionRemove(T&, int32_t index);	// maybe faster
		template<typename T> bool ActionTryRemove();
		template<typename...AS> void ActionTryRemoves();
		template<typename T> T& ActionAdd();
		template<typename T> T& ActionAddOrRef();
		/***************************************************/

		void Add_Action_Stun(float durationSeconds);
		void Add_Action_SearchTarget(float searchRange);
		void Add_Action_MoveToItem(float movementSpeed);
		void Add_Action_MoveToTarget(float movementSpeed, float timeoutSeconds);
		void Add_Action_HitTarget(float castDelaySeconds);
		void Add_Action_SetColor(xx::RGBA8 color, float durationSeconds);
		// ...

		void Case_(Action_Stun& o);
		void Case_(Action_Search& o);
		void Case_(Action_MoveToItem& o);
		void Case_(Action_MoveToTarget& o);
		void Case_(Action_HitTarget& o);
		void Case_(Action_SetColor& o);
		// ...

		/***************************************************/

		void Init(XY const& pos_ = gLooper.mapSize_2);
		int32_t Update();
		void DrawBars();	// health, mana, exp... bars

		// for logic call
		void Destroy();							// remove this from scene.monsters & destruct & free memory
		void Kill(Monster* killer);					// drop item + play death effect + Destroy(), killer + exp
		bool Hurt(Monster* killer, int32_t damage_);	// health -= killer.damage, health <= 0: Kill()
		// todo: more hurt type here ?
		void TryRestoreBornAbility();
		bool BlocksLimit();				// return true: limit fail. out of area
		void Stun(float durationSeconds);
		void MakeBladeLight(XY const& shootPos, float radians, float radius, int32_t damage);
		void MakeFireball(XY const& shootPos, float radians, float radius, float speed, float lifeSeconds, int32_t damage);
		// ...
	};
}
