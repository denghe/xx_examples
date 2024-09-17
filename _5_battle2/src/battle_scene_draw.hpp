#pragma once

namespace Battle {

	inline void Scene::Draw() {
		auto& c = gLooper.camera;

		// blocks
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.quad).SetAnchor({ 0, 1 }).SetColor({55,55,55,255});
			blocks.Foreach([&](Block& o)->void {
				q.SetPosition(c.ToGLPos(o.aabb.from))
					.SetScale((o.aabb.to - o.aabb.from) / 64 * c.scale)
					.Draw();
				});
		}

		// monster body
		if constexpr (true) {
			xx::Quad q;
			monsters.Foreach([&](Monster& o)->void {
				q.SetFrame(gRes.monster_[o.frameIndex])
					.SetPosition(c.ToGLPos(o.pos))
					.SetColor(o.color)
					.SetScale(c.scale * (o.radius / 32))
					.Draw();
			});
		}

		// monster body( line )
		if constexpr (false) {
			xx::LineStrip ls;
			ls.FillCirclePoints({}, 32, {}, 10);
			monsters.Foreach([&](Monster& o)->void {
				ls.SetPosition(c.ToGLPos(o.pos))
					.SetColor(o.color)
					.SetScale(c.scale * (o.radius / 32))
					.Draw();
			});
		}

		// monster id text
		if constexpr (false) {
			std::u32string str;
			monsters.Foreach([&](Monster& o)->void {
				auto len = xx::IntToStringTo(str, o.id);
				gLooper.ctcDefault.Draw(c.ToGLPos(o.pos), { 0.5f, 0.5f }, xx::RGBA8_Blue, str);
			});
		}

		// blade light
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.blade_light);
			bladeLights.ForeachFlags([&](BladeLight& o)->void {
				q.SetPosition(c.ToGLPos(o.pos))
					.SetScale(c.scale * o.scale)
					.SetRotate(o.radians)
					.SetColorA(int8_t(255 * o.alpha))
					.Draw();
			});
		}

		// explosions
		if constexpr (true) {
			xx::Quad q;
			q.SetColorA(123);
			explosions.ForeachFlags([&](Explosion& o)->void {
				q.SetFrame(gRes.explosion_13_[(size_t)o.frameIndex])
					.SetPosition(c.ToGLPos(o.pos))
					.SetScale(c.scale * o.scale)
					.SetRotate(o.radians)
					.Draw();
			});
		}

		// health bar
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.quad).SetAnchor({ 0, 0.5f });
			monsters.Foreach([&](Monster& o)->void {
				auto healthBarBaseScaleX = (float)o.statInfo.health / o.statInfoMax.health;
				q.SetPosition(c.ToGLPos(o.pos + XY{ -32, -39 }))
					.SetScale(c.scale * XY{ 1, 0.1})
					.SetColor(xx::RGBA8_Black)
					.Draw()
					.SetPositionX(q.pos.x + 1)
					.SetScale(c.scale * XY{ healthBarBaseScaleX * (62.f / 64.f), 0.1f * (4.4f / 6.4f) })
					.SetColor(xx::RGBA8_Red)
					.Draw();
			});
		}

		// mana bar
		if constexpr (true) {
			xx::Quad q;
			q.SetFrame(gRes.quad).SetAnchor({ 0, 0.5f });
			monsters.Foreach([&](Monster& o)->void {
				auto healthBarBaseScaleX = (float)o.statInfo.mana / o.statInfoMax.mana;
				q.SetPosition(c.ToGLPos(o.pos + XY{ -32, -32 }))
					.SetScale(c.scale * XY{ 1, 0.1})
					.SetColor(xx::RGBA8_Black)
					.Draw()
					.SetPositionX(q.pos.x + 1)
					.SetScale(c.scale * XY{ healthBarBaseScaleX * (62.f / 64.f), 0.1f * (4.4f / 6.4f) })
					.SetColor(xx::RGBA8_Blue)
					.Draw();
			});
		}

		// effect text
		if constexpr (true) {
			effectTextManager.Draw();
		}

	}

};
