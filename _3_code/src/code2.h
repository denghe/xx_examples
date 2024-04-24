#pragma once
#include <cfg.h>

namespace Code2 {

	struct CBase {
		int32_t typeId{};
	};

	struct CBegin : CBase {
		constexpr static int32_t cTypeId{ 0 };
		int32_t next;

		void Init(int32_t next_) {
			this->typeId = cTypeId;
			next = next_;
		}
	};

	template<typename T>
	struct CCompare : CBase {
		constexpr static int32_t cTypeId{ 1 };
		using ValueType = T;
		int32_t memberOffset;
		int32_t nextLess, nextEqual, nextGreat;
		T v;

		void Init(int32_t memberOffset_, T v_, int32_t nextLess_, int32_t nextEqual_, int32_t nextGreat_) {
			this->typeId = cTypeId;
			memberOffset = memberOffset_;
			v = v_;
			nextLess = nextLess_;
			nextEqual = nextEqual_;
			nextGreat = nextGreat_;
		}
	};

	template<typename T>
	struct CAdd1 : CBase {
		constexpr static int32_t cTypeId{ 2 };
		using ValueType = T;
		int32_t memberOffset;
		int32_t next;

		void Init(int32_t memberOffset_, int32_t next_) {
			this->typeId = cTypeId;
			memberOffset = memberOffset_;
			next = next_;
		}
	};

	struct CEnd : CBase {
		constexpr static int32_t cTypeId{ 3 };

		void Init() {
			this->typeId = cTypeId;
		}
	};

	struct Context {

		// simulate logic members
		int32_t a{}, b{};
		float c{}, d{};
		double e{}, f{};

		template<typename T>
		XX_FORCE_INLINE T& GetMember(int32_t memberOffset) {
			return *(T*)((char*)this + memberOffset);
		}
		// ((ptr - 1) / alignment + 1) * alignment
		static constexpr int32_t maxSiz = xx::MaxSizeof_v< CBegin, CCompare<int32_t>, CAdd1<int32_t>, CEnd >;
		using CodeItem = std::aligned_storage_t<maxSiz>;

		int32_t codesCursor{};
		xx::Listi32<CodeItem> codes;

		template<std::derived_from<CBase> T>
		T& MakeCode() {
			return (T&)codes.Emplace();
		}

		void Init(int32_t count) {
			codes.Reserve(4);			// important
			codesCursor = 0;

			/* 0 */	MakeCode<CBegin>().Init(1);
			/* 1 */	MakeCode<CCompare<int32_t>>().Init(offsetof(Context, a), count, 2, 2, 3);
			/* 2 */	MakeCode<CAdd1<int32_t>>().Init(offsetof(Context, a), 1);
			/* 3 */	MakeCode<CEnd>().Init();
		}

		XX_FORCE_INLINE bool Update() {
			assert(!codes.Empty());

			auto& base = (CBase&)codes[codesCursor];
			switch (base.typeId) {
			case 0: {
				auto& c = (CBegin&)base;
				codesCursor = c.next;
				return false;
			}
			case 1: {
				auto& c = (CCompare<int32_t>&)base;
				auto& mv = GetMember<int32_t>(c.memberOffset);
				auto& v = c.v;
				if (mv < v) {
					codesCursor = c.nextLess;
				} else if (mv == v) {
					codesCursor = c.nextEqual;
				} else {
					codesCursor = c.nextGreat;
				}
				return false;
			}
			case 2: {
				auto& c = (CAdd1<int32_t>&)base;
				auto& mv = GetMember<int32_t>(c.memberOffset);
				++mv;
				codesCursor = c.next;
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
