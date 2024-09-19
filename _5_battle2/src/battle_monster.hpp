#pragma once

namespace Battle {

	inline void Monster::Init(XY const& pos_) {
		auto radians = gScene->rnd.Next<float>(-gPI, gPI);
		pos = pos_;
		radius = cRadius;
		scale = radius / cRadius;
		radians = 0;
		frame = gRes.monster_1;
		movementDirection.x = std::cos(radians);
		movementDirection.y = std::sin(radians);

		// init stat
		statInfoMax.health = 20;
		statInfoMax.mana = 50;
		statInfo.health = statInfoMax.health = gScene->rnd.Next<int32_t>(1, statInfoMax.health + 1);
		statInfo.mana = 50;
		// todo: more init

		// add some skills
		skills.Emplace().Emplace<Item_Sword1>()->Init(this);

		TryRestoreBornAbility();
		BlocksLimit();
	}

	/*********************************************************************************************/

	inline int32_t Monster::Update() {
		auto posBak = pos;

		// execute all actions
		for (int32_t i = actionsLen - 1; i >= 0; --i) {
			auto& b = actions[i];
			switch (b.type) {
			case Action_Stun::cType: Case_((Action_Stun&)b); break;
			case Action_SearchTarget::cType: Case_((Action_SearchTarget&)b); break;
			case Action_MoveToTarget::cType: Case_((Action_MoveToTarget&)b); break;
			case Action_HitTarget::cType: Case_((Action_HitTarget&)b); break;
			case Action_SetColor::cType: Case_((Action_SetColor&)b); break;
			// ...
			}
		}

		// suicide check
		if (isDead) return -1;

		// try restore something
		TryRestoreBornAbility();

		// update space grid?
		if (posBak == pos) return 0;
		else return 1;
	}

	inline void Monster::DrawBars() {
		// health( bg + fg ), mana( bg + fg )
		auto& f = *gRes.quad;
		auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(f.tex->GetValue(), 4);
		auto hpBgPos = gLooper.camera.ToGLPos( pos + XY{ -32, -39 } );
		auto hpFgPos = hpBgPos + XY{ 1, 0 };
		auto bgScale = gLooper.camera.scale * XY{ 1, 0.1 };
		auto hpFgScale = gLooper.camera.scale * XY{ (float)statInfo.health / statInfoMax.health * (62.f / 64.f), 0.1f * (4.4f / 6.4f) };
		auto manaBgPos = hpBgPos + XY{ 0, -7 };
		auto manaFgPos = manaBgPos + XY{ 1, 0 };
		auto manaFgScale = gLooper.camera.scale * XY{ (float)statInfo.mana / statInfoMax.mana * (62.f / 64.f), 0.1f * (4.4f / 6.4f) };
		{
			auto& q = qs[0];
			q.pos = hpBgPos;
			q.anchor = { 0, 0.5f };
			q.scale = bgScale;
			q.radians = 0;
			q.colorplus = colorplus;
			q.color = xx::RGBA8_Black;
			q.texRect.data = f.textureRect.data;
		}
		{
			auto& q = qs[1];
			q.pos = hpFgPos;
			q.anchor = { 0, 0.5f };
			q.scale = hpFgScale;
			q.radians = 0;
			q.colorplus = colorplus;
			q.color = xx::RGBA8_Red;
			q.texRect.data = f.textureRect.data;
		}
		{
			auto& q = qs[2];
			q.pos = manaBgPos;
			q.anchor = { 0, 0.5f };
			q.scale = bgScale;
			q.radians = 0;
			q.colorplus = colorplus;
			q.color = xx::RGBA8_Black;
			q.texRect.data = f.textureRect.data;
		}
		{
			auto& q = qs[3];
			q.pos = manaFgPos;
			q.anchor = { 0, 0.5f };
			q.scale = manaFgScale;
			q.radians = 0;
			q.colorplus = colorplus;
			q.color = xx::RGBA8_Blue;
			q.texRect.data = f.textureRect.data;
		}

	}

};
