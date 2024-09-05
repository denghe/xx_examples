#include "pch.h"
#include "battle.h"

namespace Battle {

	void Scene::Init() {
		monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
	}

	void Scene::BeforeUpdate() {
		screenAreaMin.x = gLooper.camera.safeMinX;
		screenAreaMin.y = gLooper.camera.safeMinY;
		screenAreaMax.x = gLooper.camera.safeMaxX;
		screenAreaMax.y = gLooper.camera.safeMaxY;
	}

	int32_t Scene::Update() {
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

	void Scene::Draw() {
		auto& c = gLooper.camera;
		xx::LineStrip ls;
		ls.FillCirclePoints({}, 32);

#if 1
		monsters.Foreach([&](Monster& o)->void {
			ls.SetPosition(c.ToGLPos(o.pos))
				.SetScale(c.scale)
				.Draw();
			});
#endif

#if 1
		std::u32string str;
		monsters.Foreach([&](Monster& o)->void {
			auto len = xx::IntToStringTo(str, o.id);
			gLooper.ctcDefault.Draw(c.ToGLPos(o.pos), { 0.5f, 0.5f }, xx::RGBA8_Blue, str);
			});
#endif

	}

	/*********************************************************************************************/

	XX_INLINE bool Monster::ActionExists(ActionTypes bt) {
		return (actionFlags & (1llu << (int32_t)bt)) > 0;
	}

	XX_INLINE void Monster::ActionSetFlag(ActionTypes bt) {
		assert(!ActionExists(bt));
		actionFlags |= (1llu << (int32_t)bt);
	}

	XX_INLINE void Monster::ActionClearFlag(ActionTypes bt) {
		assert(ActionExists(bt));
		actionFlags &= ~(1llu << (int32_t)bt);
	}

	// return -1 mean not found
	XX_INLINE int32_t Monster::ActionFind(ActionTypes bt) {
		if (!ActionExists(bt)) return -1;
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			if (actions[i].type == bt) return i;
		}
		assert(false);
		return -1;
	}

	// call after ActionFind
	XX_INLINE void Monster::ActionRemove(ActionTypes bt, int32_t index) {
		assert(ActionExists(bt));
		assert(index != -1);
		assert(index < _countof(actions));
		assert(actions[index].type == bt);
		actionFlags &= ~(1llu << (int32_t)bt);
		actions[index] = actions[--actionsLen];
	}

	// return -1 mean not found
	XX_INLINE bool Monster::ActionRemove(ActionTypes bt) {
		if (!ActionExists(bt)) return false;
		for (int32_t index = actionsLen - 1; index >= 0; --index) {
			if (actions[index].type == bt) {
				ActionRemove(bt, index);
				return true;
			}
		}
		assert(false);
		return false;
	}


	/*********************************************************************************************/

	void Monster::Init(Scene* scene_) {
		scene = scene_;
		auto radians = scene->rnd.Next<float>(-gPI, gPI);
		pos = gLooper.mapSize_2;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);
		auto n = ++scene->autoId;
		id = n;
		TryAddBaseActions();
	}

	/*********************************************************************************************/

#define CONCAT_NAME( a, b ) a##b
#define CASE_ACTION_CALL(NAME) case ActionTypes::NAME: CONCAT_NAME(ActionCall_, NAME)((CONCAT_NAME(Action_, NAME)&)b, frameNumber, i); break;

	int32_t Monster::Update() {
		auto frameNumber = scene->frameNumber;
		auto posBak = pos;

		// execute all actions
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			auto& b = actions[i];
			switch (b.type) {
				CASE_ACTION_CALL(Move);
				CASE_ACTION_CALL(Stun);
				// ... more case
			}
		}

		// moved out of the screen? suicide
		if (pos.x < scene->screenAreaMin.x || pos.x > scene->screenAreaMax.x ||
			pos.y < scene->screenAreaMin.y || pos.y > scene->screenAreaMax.y) {
			return -1;
		}

		// try restore something
		TryAddBaseActions();

		// update space grid?
		if (posBak == pos) return 0;
		else return 1;
	}

	/*********************************************************************************************/

	XX_INLINE void Monster::ActionCall_Move(Action_Move& o, int32_t frameNumber, int32_t index) {
		pos += movementDirection * o.speed;
	}

	XX_INLINE void Monster::ActionCall_Stun(Action_Stun& o, int32_t frameNumber, int32_t index) {
		if (o.timeoutFrameNumber < frameNumber) {
			actions[index] = actions[--actionsLen];
			ActionClearFlag(ActionTypes::Stun);
		}
	}

	/*********************************************************************************************/

	bool Monster::ActionAdd_Move(float speed) {
		if (ActionExists(ActionTypes::Move) || ActionExists(ActionTypes::Stun)) return false;
		ActionSetFlag(ActionTypes::Move);
		auto& o = (Action_Move&)actions[actionsLen++];
		o.type = ActionTypes::Move;
		o.speed = speed;
		return true;
	}

	bool Monster::ActionAdd_Stun(int32_t numFrames) {
		if (ActionExists(ActionTypes::Stun)) return false;
		ActionRemove(ActionTypes::Move);
		ActionSetFlag(ActionTypes::Stun);
		auto& o = (Action_Stun&)actions[actionsLen++];
		o.type = ActionTypes::Stun;
		o.timeoutFrameNumber = scene->frameNumber + numFrames;
		return true;
	}

	/*********************************************************************************************/

	void Monster::TryAddBaseActions() {
		ActionAdd_Move(2);
	}

};
