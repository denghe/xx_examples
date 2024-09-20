#pragma once

namespace Battle {

	inline void Monster::DrawBars() {
		// health( bg + fg ), mana( bg + fg )
		auto& f = *gRes.quad;
		auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(f.tex->GetValue(), 4);
		auto hpBgPos = gLooper.camera.ToGLPos( pos + XY{ -32, -39 } );
		auto hpFgPos = gLooper.camera.ToGLPos(pos + XY{ -31, -39 });
		auto bgScale = gLooper.camera.scale * XY{ 1, 0.1 };
		auto hpFgScale = gLooper.camera.scale * XY{ (float)statInfo.health / statInfoMax.health * (62.f / 64.f), 0.1f * (4.4f / 6.4f) };
		auto manaBgPos = gLooper.camera.ToGLPos(pos + XY{ -32, -32 });
		auto manaFgPos = gLooper.camera.ToGLPos(pos + XY{ -31, -32 });
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
