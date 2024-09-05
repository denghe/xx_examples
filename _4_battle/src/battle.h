#pragma once
#include <looper.h>

namespace Battle {

	// simulate 1 monster walk through, random stun

	/*********************************************************************************************/

	// buff == skill == behavior
	enum class BuffTypes : int32_t {
		Move,
		Stun,
		// ...
		//Hurt,
		//RestoreHealth,
		//// ...
		MaxValue
	};

	/*********************************************************************************************/

	// data struct
	struct Buff {
		BuffTypes type;
		int32_t _0, _1, _2;
	};

	// skins ...
	struct Buff_Move {
		BuffTypes type;
		float speed, _1, _2;
	};

	struct Buff_Stun {
		BuffTypes type;
		int32_t timeoutFrameNumber, _1, _2;
	};

	// ...

	/*********************************************************************************************/

	struct Scene;
	struct Monster {
		Scene* scene{};
		int32_t id{};
		XY pos{}, movementDirection{};
		// ...

		uint64_t buffsFlags{};
		static_assert((int32_t)BuffTypes::MaxValue <= sizeof(buffsFlags) * 8);
		xx::Listi32<Buff> buffs;	// todo: inline ?

		XX_INLINE bool BuffsExists(BuffTypes bt) {
			return (buffsFlags & (1llu << (int32_t)bt)) > 0;
		}
		XX_INLINE void BuffsSetFlag(BuffTypes bt) {
			assert(!BuffsExists(bt));
			buffsFlags |= (1llu << (int32_t)bt);
		}
		XX_INLINE void BuffsClearFlag(BuffTypes bt) {
			assert(BuffsExists(bt));
			buffsFlags &= ~(1llu << (int32_t)bt);
		}

		XX_INLINE bool BuffsRemove(BuffTypes bt) {
			if (!BuffsExists(bt)) return false;
			buffsFlags &= ~(1llu << (int32_t)bt);
			for (int32_t i = buffs.len - 1; i >= 0; --i) {
				if (buffs[i].type == bt) {
					buffs.SwapRemoveAt(i);
				}
			}
			return true;
		}

		void Init(Scene* scene_);
		int32_t Update();

		void TryAddBaseBuffs();

		bool AddBuff_Move(float speed);
		bool AddBuff_Stun(int32_t numFrames);
		// ...

		void HandleBuff_Move(Buff_Move& b, int32_t frameNumber, int32_t index);
		void HandleBuff_Stun(Buff_Stun& b, int32_t frameNumber, int32_t index);
	};

	/*********************************************************************************************/

	struct Scene {
		int32_t frameNumber{};
		int32_t autoId{};
		xx::SpaceGrid<Monster> monsters;
		xx::Rnd rnd;
		// todo: wall ?

		void Init() {
			monsters.Init(gLooper.physNumRows, gLooper.physNumCols, gLooper.physCellSize);
		}

		void BeforeUpdate() {}

		int32_t Update() {
			++frameNumber;

			// simulate stun event every ?? frames
			if (frameNumber % 20 == 0) {
				monsters.Foreach([](Monster& o)->void {
					o.AddBuff_Stun(60);
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
			monsters.EmplaceInit(this);

			return 0;
		}

		void Draw(xx::Camera& camera) {
			xx::LineStrip ls;
			ls.FillCirclePoints({}, 32);

			monsters.Foreach([&](Monster& o)->void {
				ls.SetPosition(camera.ToGLPos(o.pos)).Draw();
			});

			monsters.Foreach([&](Monster& o)->void {
				auto str = xx::ToString("m", o.id);	// todo: show id
				gLooper.ctcDefault.Draw(camera.ToGLPos(o.pos), str, xx::RGBA8_Green, { 0.5f, 0.5f });
			});
		}
	};

	/*********************************************************************************************/

	inline void Monster::Init(Scene* scene_) {
		scene = scene_;
		buffs.Reserve(6);	// avoid reallocating memory due to adding behavior
		auto radians = scene->rnd.Next<float>(-gPI, gPI);
		pos = gLooper.mapSize_2;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);
		id = ++scene->autoId;
		TryAddBaseBuffs();
	}

	/*********************************************************************************************/

#define CONCAT_NAME( a, b ) a##b
#define CASE_BUFF(NAME) case BuffTypes::NAME: CONCAT_NAME(HandleBuff_, NAME)((CONCAT_NAME(Buff_, NAME)&)b, frameNumber, i); break;

	inline int32_t Monster::Update() {
		auto frameNumber = scene->frameNumber;
		auto posBak = pos;
		for (int32_t i = buffs.len - 1; i >= 0; --i) {
			auto& b = buffs[i];
			switch (b.type) {
				CASE_BUFF(Move);
				CASE_BUFF(Stun);
				// ... more case
			}
		}
		TryAddBaseBuffs();
		// todo: is dead: return -1 when moved out of the screen?
		if (posBak == pos) return 0;
		else return 1;
	}

	XX_INLINE void Monster::HandleBuff_Move(Buff_Move& o, int32_t frameNumber, int32_t index) {
		pos += movementDirection * o.speed;
	}

	XX_INLINE void Monster::HandleBuff_Stun(Buff_Stun& o, int32_t frameNumber, int32_t index) {
		if (o.timeoutFrameNumber < frameNumber) {
			buffs.SwapRemoveAt(index);
			BuffsClearFlag(BuffTypes::Stun);
		}
	}

	/*********************************************************************************************/

	inline bool Monster::AddBuff_Move(float speed) {
		if (BuffsExists(BuffTypes::Move) || BuffsExists(BuffTypes::Stun)) return false;
		BuffsSetFlag(BuffTypes::Move);
		auto& o = (Buff_Move&)buffs.Emplace();
		o.type = BuffTypes::Move;
		o.speed = speed;
		return true;
	}

	inline bool Monster::AddBuff_Stun(int32_t numFrames) {
		if (BuffsExists(BuffTypes::Stun)) return false;
		BuffsRemove(BuffTypes::Move);
		BuffsSetFlag(BuffTypes::Stun);
		auto& o = (Buff_Stun&)buffs.Emplace();
		o.type = BuffTypes::Stun;
		o.timeoutFrameNumber = scene->frameNumber + numFrames;
		return true;
	}

	inline void Monster::TryAddBaseBuffs() {
		AddBuff_Move(1);
	}

};
