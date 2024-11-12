#pragma once

namespace Msgs {
	namespace Global {

		inline void EffectText::Init(Scene* scene_, xx::XY const& pos_, xx::XY const& direction_, xx::RGBA8 color_, int32_t value_) {
			scene = xx::WeakFromThis(scene_);
			// calculate center point
			pos = { pos_.x - cCharPixelWidth * cScale * len / 2, pos_.y };
			// calculate move frame inc with random speed
			auto _1_mag = 1 / std::sqrt(direction_.x * direction_.x + direction_.y * direction_.y);
			inc = direction_ * _1_mag * scene->rnd.Next<float>(cMoveSpeedMin, cMoveSpeedMax);
			color = color_;
			alpha = 1;
			len = xx::ToStringEN(value_, buf.data());
		}

		inline int32_t EffectText::UpdateCore() {
			COR_BEGIN
			// move away
			timeout = scene->frameNumber + cMoveDurationSecondsFrames;
			while (scene->frameNumber <= timeout) {
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

		inline bool EffectText::Update() {
			lineNumber = UpdateCore();
			return lineNumber == 0;		// will remove by caller
		}

		inline void EffectText::Draw() {
			auto& fs = gRes.font_outline_;
			auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(fs[0]->tex->GetValue(), len);
			auto basePos = gLooper.camera.ToGLPos(pos);
			auto scale = gLooper.camera.scale * cScale;
			auto widthInc = cCharPixelWidth * scale;
			basePos.x -= widthInc * len / 2;
			for (int32_t i = 0; i < len; ++i) {
				auto& q = qs[i];
				q.anchor = { 0, 0.5f };
				q.color = { color.r, color.g, color.b, (uint8_t)(color.a * alpha) };
				q.colorplus = 1;
				q.pos = basePos;
				q.radians = 0;
				q.scale = { scale, scale };
				q.texRect.data = fs[buf[i] - 32]->textureRect.data;
				basePos.x += widthInc;
			}
		}

	}
}
