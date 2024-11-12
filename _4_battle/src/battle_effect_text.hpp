#pragma once

namespace Battle {

	XX_INLINE void EffectText::Init(xx::XY const& pos_, xx::XY const& dist_, xx::RGBA8 color_, int32_t value_) {
		// calculate center point
		pos = { pos_.x - cCharPixelWidth * cScale * len / 2, pos_.y };
		// calculate move frame inc with random speed
		auto _1_mag = 1 / std::sqrt(dist_.x * dist_.x + dist_.y * dist_.y);
		inc = dist_ * _1_mag * gLooper.rnd.Next<float>(cMoveSpeedMin, cMoveSpeedMax);
		color = color_;
		alpha = 1;
		len = xx::ToStringEN(value_, buf.data());
	}

	XX_INLINE int32_t EffectText::UpdateCore() {
		COR_BEGIN
			// move away
			timeoutFrameNumber = gLooper.frameNumber + int32_t(cMoveDurationSeconds * Cfg::fps);
			while(gLooper.frameNumber <= timeoutFrameNumber) {
				pos += inc;
				COR_YIELD
			}
		// fade out
		for (; alpha >= 0; alpha -= cFadeOutStep) {
			COR_YIELD
		}
		alpha = 0;
		COR_END
	}

	XX_INLINE bool EffectText::Update() {
		lineNumber = UpdateCore();
		return lineNumber == 0;		// will remove by caller
	}

	XX_INLINE void EffectText::Draw() {
		auto& fs = gRes.font_outline_;
		auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(fs[0]->tex->GetValue(), len);
		auto basePos = gLooper.camera.ToGLPos(pos);
		auto scale = gLooper.camera.scale * cScale;
		auto widthInc = cCharPixelWidth * scale;
		basePos.x -= widthInc * len / 2;
		for (int32_t i = 0; i < len; ++i) {
			auto& q = qs[i];
			q.anchor = { 0, 0.5f };
			q.color = {color.r, color.g, color.b, (uint8_t)(color.a * alpha)};
			q.colorplus = 1;
			q.pos = basePos;
			q.radians = 0;
			q.scale = { scale, scale };
			q.texRect.data = fs[buf[i] - 32]->textureRect.data;
			basePos.x += widthInc;
		}
	}

	XX_INLINE void EffectTextManager::Init(int32_t cap) {
		ens.Reserve(cap);
	}

	XX_INLINE void EffectTextManager::Add(xx::XY const& pos_, xx::XY const& dist_, xx::RGBA8 color_, int32_t value_) {
		ens.Emplace().Init(pos_, dist_, color_, value_);
	}

	XX_INLINE bool EffectTextManager::Update() {
		int32_t n{};
		for (int32_t i = 0, e = ens.Count(); i < e; ++i) {
			if (ens[i].Update()) {
				++n;
			}
		}
		ens.PopMulti(n);
		return ens.Empty();
	}

	XX_INLINE void EffectTextManager::Draw() {
		for (int32_t i = 0, e = ens.Count(); i < e; ++i) {
			ens[i].Draw();
		}
	}

};
