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


		// code runtime context
		int32_t codesCursor{};
		std::vector<std::unique_ptr<Code_Base>> codes;
		void Init();	// simulate load code from player config
		bool Update();	// return true: end / finished
		void Run();		// while(!Update()){}

		template<std::derived_from<Code_Base> T>
		T& MakeCode() {
			return (T&)*codes.emplace_back(std::make_unique<T>());
		}

		template<typename T>
		T& GetMember(size_t memberOffset) {
			return *(T*)((char*)this + memberOffset);
		}
	};

	struct Code_Base {
		Context* cd{};
		int32_t nextsCount{};
		std::array<int32_t, 4> nexts{};

		Code_Base() = default;
		Code_Base(Code_Base const&) = delete;
		Code_Base& operator=(Code_Base const&) = delete;
		void BaseInit(Context* cd_, int32_t nextsCount
			, int32_t next0, int32_t next1 = -1, int32_t next2 = -1, int32_t next3 = -1);
		virtual int32_t Update() = 0;	// return -2( end ), -1(not finish) or index( nexts[] )
		virtual ~Code_Base() {};
	};


	struct Code_Begin : Code_Base {
		void Init(Context* cd_, int32_t next);
		int32_t Update() override;
	};

	template<typename T>
	struct Code_Compare : Code_Base {
		T v{};
		size_t memberOffset{};

		void Init(Context* cd_, size_t memberOffset_, T v_, int32_t nextLess, int32_t nextEqual, int32_t nextGreat) {
			this->BaseInit(cd_, 3, nextLess, nextEqual, nextGreat);
			memberOffset = memberOffset_;
			v = v_;
		}

		int32_t Update() override {
			auto& mv = cd->GetMember<T>(memberOffset);
			if (mv < v) return 0;
			else if (mv == v) return 1;
			return 2;
		}
	};

	template<typename T>
	struct Code_Add1 : Code_Base {
		size_t memberOffset{};

		void Init(Context* cd_, size_t memberOffset_, int32_t next) {
			this->BaseInit(cd_, 1, next);
			memberOffset = memberOffset_;
		}

		int32_t Update() override {
			auto& mv = cd->GetMember<T>(memberOffset);
			++mv;
			return 0;
		}
	};

	struct Code_End : Code_Base {
		void Init(Context* cd_);
		int32_t Update() override;
	};

};
