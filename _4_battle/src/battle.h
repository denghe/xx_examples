#pragma once
#include <looper.h>

namespace Battle {

	// simulate 1 monster walk through, random stun
	// action == skill == behavior == buff

	/*********************************************************************************************/

	enum class ActionTypes : int32_t {
		Move,
		Stun,
		// ...
		//Hurt,
		//RestoreHealth,
		//// ...
		MaxValue
	};
	static_assert((int32_t)ActionTypes::MaxValue <= 64);	// uint64_t actionFlags limit 

	/*********************************************************************************************/

	// data struct
	struct Action {
		union {
		std::array<uint64_t, 3> _;
		struct {
		ActionTypes type;
		int32_t __;
		};
		};
	};

	// skins ... pod & sizeof(T) <= sizeof(Action)
	struct Action_Move {
		ActionTypes type;
		float speed;
	};

	struct Action_Stun {
		ActionTypes type;
		int32_t timeoutFrameNumber;
	};

	struct Monster;
	struct Action_MoveTo {
		ActionTypes type;
		float speed;
		xx::SpaceWeak<Monster> target;
	};
	// ...

	/*********************************************************************************************/

	struct Scene;
	struct Monster {
		Scene* scene{};
		int32_t id{};
		//char32_t id[10];
		XY pos{}, movementDirection{};
		// ...
		int32_t actionsLen{};
		uint64_t actionFlags{};
		Action actions[2];		// careful: safe cap need more test

		XX_INLINE bool ActionExists(ActionTypes bt) {
			return (actionFlags & (1llu << (int32_t)bt)) > 0;
		}
		XX_INLINE void ActionSetFlag(ActionTypes bt) {
			assert(!ActionExists(bt));
			actionFlags |= (1llu << (int32_t)bt);
		}
		XX_INLINE void ActionClearFlag(ActionTypes bt) {
			assert(ActionExists(bt));
			actionFlags &= ~(1llu << (int32_t)bt);
		}

		XX_INLINE bool ActionRemove(ActionTypes bt) {
			if (!ActionExists(bt)) return false;
			actionFlags &= ~(1llu << (int32_t)bt);
			for (int32_t i = actionsLen - 1; i >= 0; --i) {
				if (actions[i].type == bt) {
					actions[i] = actions[--actionsLen];
				}
			}
			return true;
		}

		void Init(Scene* scene_);
		int32_t Update();

		void TryAddBaseActions();

		bool ActionAdd_Move(float speed);
		bool ActionAdd_Stun(int32_t numFrames);
		// ...

		void ActionHandle_Move(Action_Move& b, int32_t frameNumber, int32_t index);
		void ActionHandle_Stun(Action_Stun& b, int32_t frameNumber, int32_t index);
	};

	/*********************************************************************************************/

	struct Scene {
		int32_t frameNumber{};
		int32_t autoId{};
		XY screenAreaMin{}, screenAreaMax{};
		xx::SpaceGrid<Monster> monsters;
		xx::Rnd rnd;
		// todo: wall ?

		void Init() {
			monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
		}

		void BeforeUpdate() {
			screenAreaMin.x = gLooper.camera.safeMinX;
			screenAreaMin.y = gLooper.camera.safeMinY;
			screenAreaMax.x = gLooper.camera.safeMaxX;
			screenAreaMax.y = gLooper.camera.safeMaxY;
		}

		int32_t Update() {
			++frameNumber;

			// simulate stun event every ?? frames
			if (frameNumber % 20 == 0) {
				monsters.Foreach([](Monster& o)->void {
					o.ActionAdd_Stun(60);
				});
			}

			// update all monsters
			monsters.Foreach([&](Monster& o)->xx::ForeachResult {
				auto r = o.Update();
				if (r == -1) return xx::ForeachResult::RemoveAndContinue;
				else if (r == 1) {
					monsters.Update(o);
				}
				return xx::ForeachResult::Continue;
			});

			// make some monsters
			for (int32_t i = 0; i < 10; i++) {
				monsters.EmplaceInit(this);
			}

			return 0;
		}

		void Draw() {
			auto& c = gLooper.camera;
			xx::LineStrip ls;
			ls.FillCirclePoints({}, 32);

			monsters.Foreach([&](Monster& o)->void {
				ls.SetPosition(c.ToGLPos(o.pos))
				.SetScale(c.scale)
				.Draw();
			});


			std::u32string str;
			monsters.Foreach([&](Monster& o)->void {
				auto len = xx::IntToStringTo(str, o.id);
				gLooper.ctcDefault.Draw(c.ToGLPos(o.pos), { 0.5f, 0.5f }, xx::RGBA8_Blue, str);
			});

			//std::u32string str = U"m";
			//monsters.Foreach([&](Monster& o)->void {
			//	auto len = xx::IntToStringTo<false>(str, o.id);
			//	gLooper.ctcDefault.Draw(c.ToGLPos(o.pos), { 0.5f, 0.5f }, xx::RGBA8_Blue, str);
			//	str.resize(1);
			//});

			//monsters.Foreach([&c](Monster& o)->void {
			//	gLooper.ctcDefault.Draw(c.ToGLPos(o.pos), { 0.5f, 0.5f }, xx::RGBA8_Blue, o.id);
			//});
		}
	};

	/*********************************************************************************************/

	inline void Monster::Init(Scene* scene_) {
		scene = scene_;
		auto radians = scene->rnd.Next<float>(-gPI, gPI);
		pos = gLooper.mapSize_2;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);
		auto n = ++scene->autoId;
		//id[0] = 'm';
		//id[1 + xx::ToStringEN(n, id + 1)] = '0';
		id = n;
		TryAddBaseActions();
	}

	/*********************************************************************************************/

#define CONCAT_NAME( a, b ) a##b
#define CASE_ACTION(NAME) case ActionTypes::NAME: CONCAT_NAME(ActionHandle_, NAME)((CONCAT_NAME(Action_, NAME)&)b, frameNumber, i); break;

	inline int32_t Monster::Update() {
		auto frameNumber = scene->frameNumber;
		auto posBak = pos;

		// execute all actions
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			auto& b = actions[i];
			switch (b.type) {
				CASE_ACTION(Move);
				CASE_ACTION(Stun);
				// ... more case
			}
		}

		// moved out of the screen? suside
		if (pos.x < scene->screenAreaMin.x || pos.x > scene->screenAreaMax.x ||
			pos.y < scene->screenAreaMin.y || pos.y > scene->screenAreaMax.y) {
			return -1;
		}

		// 
		TryAddBaseActions();

		// update space grid?
		if (posBak == pos) return 0;
		else return 1;
	}

	XX_INLINE void Monster::ActionHandle_Move(Action_Move& o, int32_t frameNumber, int32_t index) {
		pos += movementDirection * o.speed;
	}

	XX_INLINE void Monster::ActionHandle_Stun(Action_Stun& o, int32_t frameNumber, int32_t index) {
		if (o.timeoutFrameNumber < frameNumber) {
			actions[index] = actions[--actionsLen];
			ActionClearFlag(ActionTypes::Stun);
		}
	}

	/*********************************************************************************************/

	inline bool Monster::ActionAdd_Move(float speed) {
		if (ActionExists(ActionTypes::Move) || ActionExists(ActionTypes::Stun)) return false;
		ActionSetFlag(ActionTypes::Move);
		auto& o = (Action_Move&)actions[actionsLen++];
		o.type = ActionTypes::Move;
		o.speed = speed;
		return true;
	}

	inline bool Monster::ActionAdd_Stun(int32_t numFrames) {
		if (ActionExists(ActionTypes::Stun)) return false;
		ActionRemove(ActionTypes::Move);
		ActionSetFlag(ActionTypes::Stun);
		auto& o = (Action_Stun&)actions[actionsLen++];
		o.type = ActionTypes::Stun;
		o.timeoutFrameNumber = scene->frameNumber + numFrames;
		return true;
	}

	inline void Monster::TryAddBaseActions() {
		ActionAdd_Move(2);
	}

};
