#pragma once
#include <cfg.h>

namespace Code {

	struct Code_Base;

	struct Context {
		// simulate logic members
		XY pos{};
		int32_t a{}, b{};
		float c{}, d{};
		double e{}, f{};

		template<typename T>
		T& GetMember(int32_t memberOffset) {
			return *(T*)((char*)this + memberOffset);
		}
	};

	struct Code_Base {
		std::array<int32_t, 3> nexts{};
		//int32_t Update(Context* c);	// return -2( end ), -1(not finish) or index( nexts[] )
	};

	struct Code_Begin : Code_Base {
		void Init(int32_t next) {
			this->nexts[0] = next;
		}
		int32_t Update(Context* c) {
			return 0;
		}
	};

	template<typename T>
	struct Code_Compare : Code_Base {
		int32_t memberOffset{};
		T v{};

		void Init(int32_t memberOffset_, T v_, int32_t nextLess, int32_t nextEqual, int32_t nextGreat) {
			this->nexts[0] = nextLess;
			this->nexts[1] = nextEqual;
			this->nexts[2] = nextGreat;
			memberOffset = memberOffset_;
			v = v_;
		}

		int32_t Update(Context* c) {
			auto& mv = c->GetMember<T>(memberOffset);
			if (mv < v) return 0;
			else if (mv == v) return 1;
			return 2;
		}
	};

	template<typename T>
	struct Code_Add1 : Code_Base {
		int32_t memberOffset{};

		void Init(int32_t memberOffset_, int32_t next) {
			this->nexts[0] = next;
			memberOffset = memberOffset_;
		}

		int32_t Update(Context* c) {
			auto& mv = c->GetMember<T>(memberOffset);
			++mv;
			return 0;
		}
	};

	struct Code_End : Code_Base {
		int32_t Update(Context* c) {
			return -2;
		}
	};


	struct Context2 : Context {
		typedef int32_t(*UpdateFunc)(void*, Context*);
		static constexpr int32_t maxSiz = xx::MaxSizeof_v< Code_Begin, Code_Compare<int32_t>, Code_Add1<int32_t>, Code_End >;
		using CodeItem = std::pair<UpdateFunc, std::aligned_storage_t<maxSiz>>;

		int32_t codesCursor{};

		xx::Listi32<CodeItem> codes;

		template<std::derived_from<Code_Base> T>
		T& MakeCode() {
			auto& kv = codes.Emplace();
			kv.first = [](void* p, Context* c) { return ((T*)p)->Update(c); };
			return (T&)kv.second;
		}

		void Init(int32_t count) {
			codes.Reserve(4);			// important

			/* 0 */	MakeCode<Code_Begin>().Init(1);
			/* 1 */	MakeCode<Code_Compare<int32_t>>().Init(offsetof(Context, a), count, 2, 2, 3);
			/* 2 */	MakeCode<Code_Add1<int32_t>>().Init(offsetof(Context, a), 1);
			/* 3 */	MakeCode<Code_End>();
		}

		XX_FORCE_INLINE bool Update() {
			assert(!codes.Empty());
			assert(codesCursor < codes.len);

			auto& kv = codes[codesCursor];
			auto c = (Code_Base*)&kv.second;	// dangerous ?
			auto r = kv.first(c, this);
			assert(r >= 0 && r < 3);
			switch (r) {
			case -2: return true;
			case -1: return false;
			default:
				codesCursor = c->nexts[r];
			}
			assert(codesCursor >= 0 && codesCursor < codes.len);
			return false;
		}

		void Run() {
			while (!Update()) {}
		}


		XX_NOINLINE void Run2(int32_t n) {
		Lab0:
			if (a < n) goto Lab1;
			else if (a == n) goto Lab1;
			else goto Lab2;

		Lab1:
			++a;
			goto Lab0;
		Lab2:;
		}
	};
};
