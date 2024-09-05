#pragma once
#include <cfg.h>

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
		int32_t speed, _1, _2;
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
		int32_t pos{};
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

		bool AddBuff_Move(int32_t speed);
		bool AddBuff_Stun(int32_t numFrames);
		// ...

		void HandleBuff_Move(Buff_Move& b, int32_t frameNumber, int32_t index);
		void HandleBuff_Stun(Buff_Stun& b, int32_t frameNumber, int32_t index);
	};

	/*********************************************************************************************/

	struct Scene {
		int32_t frameNumber{};
		Monster monster;

		void Init() {
			monster.Init(this);
		}

		int32_t Update() {
			++frameNumber;
			if (frameNumber % 3 == 0) {
				monster.AddBuff_Stun(5);		// simulate stun event every 3 frames
			}
			return monster.Update();
		}
	};

	/*********************************************************************************************/

	inline void Monster::TryAddBaseBuffs() {
		AddBuff_Move(1);
	}

	inline void Monster::Init(Scene* scene_) {
		scene = scene_;
		buffs.Reserve(6);	// avoid reallocating memory due to adding behavior
		TryAddBaseBuffs();
	}

	/*********************************************************************************************/

#define CONCAT_NAME( a, b ) a##b
#define CASE_BUFF(NAME) case BuffTypes::NAME: CONCAT_NAME(HandleBuff_, NAME)((CONCAT_NAME(Buff_, NAME)&)b, frameNumber, i); break;

	inline int32_t Monster::Update() {
		auto frameNumber = scene->frameNumber;
		for (int32_t i = buffs.len - 1; i >= 0; --i) {
			auto& b = buffs[i];
			switch (b.type) {
				CASE_BUFF(Move);
				CASE_BUFF(Stun);
				// ... more case
			}
		}
		TryAddBaseBuffs();
		return 0;
	}

	XX_INLINE void Monster::HandleBuff_Move(Buff_Move& o, int32_t frameNumber, int32_t index) {
		pos += o.speed;
	}

	XX_INLINE void Monster::HandleBuff_Stun(Buff_Stun& o, int32_t frameNumber, int32_t index) {
		if (o.timeoutFrameNumber < frameNumber) {
			buffs.SwapRemoveAt(index);
			BuffsClearFlag(BuffTypes::Stun);
		}
	}

	/*********************************************************************************************/

	inline bool Monster::AddBuff_Move(int32_t speed) {
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

	/*********************************************************************************************/
	/*********************************************************************************************/

	inline void Test() {
		Scene scene;
		scene.Init();
#if 1
		for (int32_t i = 0; i < 20; i++) {
			scene.Update();
			xx::CoutN(scene.frameNumber, "\tmonster.pos = ", scene.monster.pos);
		}
#else
		auto secs = xx::NowEpochSeconds();
		for (int32_t i = 0; i < 100000000; i++) {
			scene.Update();
		}
		xx::CoutN("secs = ", xx::NowEpochSeconds(secs));
		xx::CoutN(scene.frameNumber, "\tmonster.pos = ", scene.monster.pos);
#endif
	}
};
