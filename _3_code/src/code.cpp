#include "pch.h"
#include "code.h"

namespace Code {

	void Code_Base::BaseInit(Context* cd_, int32_t nextsCount_, int32_t next0, int32_t next1, int32_t next2, int32_t next3) {
		cd = cd_;
		nextsCount = nextsCount_;
		nexts[0] = next0;
		nexts[1] = next1;
		nexts[2] = next2;
		nexts[3] = next3;
	}

	void Code_Begin::Init(Context* cd_, int32_t next) {
		this->BaseInit(cd, 1, next);
	}

	int32_t Code_Begin::Update() {
		return 0;
	}

	void Code_End::Init(Context* cd_) {
		this->BaseInit(cd, 0, -1);
	}

	int32_t Code_End::Update() {
		return -2;
	}


	void Context::Init(int32_t count) {
		/* 0 */	MakeCode<Code_Begin>().Init(this, 1);
		/* 1 */	MakeCode<Code_Compare<int32_t>>().Init(this
			, offsetof(Context, a), count, 2, 2, 3);
		/* 2 */	MakeCode<Code_Add1<int32_t>>().Init(this
			, offsetof(Context, a), 1);
		/* 3 */	MakeCode<Code_End>().Init(this);
	}

	bool Context::Update() {
		assert(!codes.empty());
		assert(codesCursor < codes.size());

		auto& c = codes[codesCursor];
		auto r = c->Update();
		switch (r) {
		case -2: return true;
		case -1: return false;
		default:
			codesCursor = c->nexts[r];
		}
		// todo: error check?
		return false;
	}

	void Context::Run() {
		while (!Update()) {}
	}

}
