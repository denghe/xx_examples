#pragma once
#include <cfg.h>

namespace Code3 {

	// simulate 1 monster walk through, random stun

	enum class BuffTypes : int32_t {
		Move,
		Stun,
		// ...
		MaxValue
	};

	struct Buff {
		BuffTypes type;
		union {
			float fs[3];
			int32_t is[3];
			struct {
				union {
					float f1;
					int32_t i1;
				};
				union {
					float f2;
					int32_t i2;
				};
				union {
					float f3;
					int32_t i3;
				};
			};
		};
	};

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
	};

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

	inline void Monster::TryAddBaseBuffs() {
		AddBuff_Move(1);
	}

	inline void Monster::Init(Scene* scene_) {
		scene = scene_;
		buffs.Reserve(2);
		TryAddBaseBuffs();
	}

	inline int32_t Monster::Update() {
		auto frameNumber = scene->frameNumber;
		for (int32_t i = buffs.len - 1; i >= 0; --i) {
			auto& b = buffs[i];
			switch (b.type) {
			case BuffTypes::Move:
				pos += b.i1;
				break;
			case BuffTypes::Stun:
				if (b.i1 < frameNumber) {
					buffs.SwapRemoveAt(i);
					BuffsClearFlag(BuffTypes::Stun);
				}
				break;
			}
		}

		TryAddBaseBuffs();
		return 0;
	}

	inline bool Monster::AddBuff_Move(int32_t speed) {
		if (BuffsExists(BuffTypes::Move) || BuffsExists(BuffTypes::Stun)) return false;
		BuffsSetFlag(BuffTypes::Move);
		auto& buff = buffs.Emplace();
		buff.type = BuffTypes::Move;
		buff.i1 = speed;
		return true;
	}

	inline bool Monster::AddBuff_Stun(int32_t numFrames) {
		if (BuffsExists(BuffTypes::Stun)) return false;
		BuffsRemove(BuffTypes::Move);
		BuffsSetFlag(BuffTypes::Stun);
		auto& buff = buffs.Emplace();
		buff.type = BuffTypes::Stun;
		buff.i1 = scene->frameNumber + numFrames;
		return true;
	}

	inline void Test() {
		Scene scene;
		scene.Init();
		for (int32_t i = 0; i < 20; i++) {
			//xx::CoutTN("frameNumber = ", scene.frameNumber, "\tscene.Update() == ", scene.Update());
			scene.Update();
			xx::CoutN(scene.frameNumber, "\tmonster.pos = ", scene.monster.pos);
		}
	}
};
