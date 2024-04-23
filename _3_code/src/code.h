#pragma once
#include <cfg.h>

namespace Code {

	struct Code_Base;

	struct Context {
		// simulate logic members
		int32_t a{}, b{};
		float c{}, d{};
		double e{}, f{};

		template<typename T>
		XX_FORCE_INLINE T& GetMember(int32_t memberOffset) {
			return *(T*)((char*)this + memberOffset);
		}
	};

	struct Code_Base {
		int32_t typeId{};
		std::array<int32_t, 3> nexts{};	// todo: remove this
		//int32_t Update(Context* c);	// return -2( end ), -1(not finish) or index( nexts[] )
	};

	struct Code_Begin : Code_Base {
		void Init(int32_t next) {
			this->typeId = 0;
			this->nexts[0] = next;
		}
		XX_FORCE_INLINE int32_t Update(Context* c) {
			return 0;
		}
	};

	template<typename T>
	struct Code_Compare : Code_Base {
		int32_t memberOffset{};
		T v{};

		void Init(int32_t memberOffset_, T v_, int32_t nextLess, int32_t nextEqual, int32_t nextGreat) {
			this->typeId = 1;
			this->nexts[0] = nextLess;
			this->nexts[1] = nextEqual;
			this->nexts[2] = nextGreat;
			memberOffset = memberOffset_;
			v = v_;
		}

		XX_FORCE_INLINE int32_t Update(Context* c) {
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
			this->typeId = 2;
			this->nexts[0] = next;
			memberOffset = memberOffset_;
		}

		XX_FORCE_INLINE int32_t Update(Context* c) {
			auto& mv = c->GetMember<T>(memberOffset);
			++mv;
			return 0;
		}
	};

	struct Code_End : Code_Base {
		void Init() {
			this->typeId = 3;
		}
		XX_FORCE_INLINE int32_t Update(Context* c) {
			return -2;
		}
	};

	// test results:
	// call virtual function. secs = 0.69
	// call member function pointer. secs == 0.51
	// switch case call inline member function. secs == 0.41
	// switch case inline code. secs == 0.33

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



	struct Context3 : Context {
		static constexpr int32_t maxSiz = xx::MaxSizeof_v< Code_Begin, Code_Compare<int32_t>, Code_Add1<int32_t>, Code_End >;
		using CodeItem = std::aligned_storage_t<maxSiz>;

		int32_t codesCursor{};

		xx::Listi32<CodeItem> codes;

		template<std::derived_from<Code_Base> T>
		T& MakeCode() {
			return (T&)codes.Emplace();
		}

		void Init(int32_t count) {
			codes.Reserve(4);			// important

			/* 0 */	MakeCode<Code_Begin>().Init(1);
			/* 1 */	MakeCode<Code_Compare<int32_t>>().Init(offsetof(Context, a), count, 2, 2, 3);
			/* 2 */	MakeCode<Code_Add1<int32_t>>().Init(offsetof(Context, a), 1);
			/* 3 */	MakeCode<Code_End>().Init();
		}

		XX_FORCE_INLINE bool Update() {
			assert(!codes.Empty());
			assert(codesCursor < codes.len);

			auto& o = codes[codesCursor];
			auto c = (Code_Base*)&o;
			int32_t r;
			switch (c->typeId) {
			case 0:
				r = ((Code_Begin*)c)->Update(this);
				break;
			case 1:
				r = ((Code_Compare<int32_t>*)c)->Update(this);
				break;
			case 2:
				r = ((Code_Add1<int32_t>*)c)->Update(this);
				break;
			case 3:
				r = ((Code_End*)c)->Update(this);
				break;
			}
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
	};



	struct Context4 : Context {
		static constexpr int32_t maxSiz = xx::MaxSizeof_v< Code_Begin, Code_Compare<int32_t>, Code_Add1<int32_t>, Code_End >;
		using CodeItem = std::aligned_storage_t<maxSiz>;

		int32_t codesCursor{};

		xx::Listi32<CodeItem> codes;

		template<std::derived_from<Code_Base> T>
		T& MakeCode() {
			return (T&)codes.Emplace();
		}

		void Init(int32_t count) {
			codes.Reserve(4);			// important
			codesCursor = 0;

			/* 0 */	MakeCode<Code_Begin>().Init(1);
			/* 1 */	MakeCode<Code_Compare<int32_t>>().Init(offsetof(Context, a), count, 2, 2, 3);
			/* 2 */	MakeCode<Code_Add1<int32_t>>().Init(offsetof(Context, a), 1);
			/* 3 */	MakeCode<Code_End>().Init();
		}

		XX_FORCE_INLINE bool Update() {
			assert(!codes.Empty());

			auto& c = (Code_Base&)codes[codesCursor];
			switch (c.typeId) {
			case 0: {
				codesCursor = c.nexts[0];
				return false;
			}
			case 1: {
				auto& self = (Code_Compare<int32_t>&)c;
				auto& mv = GetMember<int32_t>(self.memberOffset);
				auto& v = self.v;
				if (mv < v) {
					codesCursor = c.nexts[0];
				} else if (mv == v) {
					codesCursor = c.nexts[1];
				} else {
					codesCursor = c.nexts[2];
				}
				return false;
			}
			case 2: {
				auto& self = (Code_Add1<int32_t>&)c;
				auto& mv = GetMember<int32_t>(self.memberOffset);
				++mv;
				codesCursor = c.nexts[0];
				return false;
			}
			case 3: return true;
			}
			return false;
		}

		void Run() {
			assert(codesCursor == 0);
			while (!Update()) {};
		}
	};

};
