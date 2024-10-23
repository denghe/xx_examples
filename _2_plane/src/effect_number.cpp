#include "pch.h"
#include "looper.h"

XX_INLINE void EffectNumber::FillBuf(int i) {
	len = {};
	while (i >= 10) {
		int a = i / 10;
		buf[len++] = i - a * 10;
		i = a;
	}
	buf[len++] = i;
}

// pos: original position,  dist: determine move direction
void EffectNumber::Init(xx::XY const& pos_, xx::XY const& dist_, xx::RGBA8 color_, int value_) {
	texId = gRes.number_outlined_[0]->tex->GetValue();
	FillBuf(value_);
	pos = { pos_.x - cCharPixelWidth * cScale * len / 2, pos_.y };	// calculate center point
	auto _1_mag = 1 / std::sqrt(dist_.x * dist_.x + dist_.y * dist_.y);
	inc = dist_ * _1_mag * gLooper.rnd.Next<float>(cMoveSpeedMin, cMoveSpeedMax);	// calculate move frame inc with random speed
	color = color_;
}

int EffectNumber::UpdateCore() {
	COR_BEGIN
	// move away
	for (f = 0; f < cMoveDurationSeconds; f += gCfg.frameDelay) {
		pos += inc;
		COR_YIELD
	}
	// fade out
	for (f = 255.f; f >= 0; f -= cFadeOutInc) {
		color.a = (uint8_t)f;
		COR_YIELD
	}
	color.a = 0;
	COR_END
}

XX_INLINE bool EffectNumber::Update() {
	lineNumber = UpdateCore();
	return lineNumber == 0;		// will remove by caller
}

void EffectNumber::Draw() {
	auto qs = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(texId, len);
	auto basePos = gLooper.camera.ToGLPos(pos);
	auto scale = gLooper.camera.scale * cScale;
	auto widthInc = cCharPixelWidth * scale;
	auto& fs = gRes.number_outlined_;
	for (int i = 0; i < len; ++i) {
		auto& q = qs[i];
		q.anchor = { 0, 0.5f };
		q.color = color;
		q.colorplus = 1;
		q.pos = basePos;
		q.radians = 0;
		q.scale = { scale, scale };
		q.texRect.data = fs[buf[len - i - 1]]->textureRect.data;
		basePos.x += widthInc;
	}
}


void EffectNumberManager::Init(size_t cap) {
	ens.Reserve(cap);
}

void EffectNumberManager::Add(xx::XY const& pos_, xx::XY const& dist_, xx::RGBA8 color_, int value_) {
	ens.Emplace().Init(pos_, dist_, color_, value_);
}

bool EffectNumberManager::Update() {
	size_t n{};
	for (size_t i = 0, e = ens.Count(); i < e; ++i) {
		if (ens[i].Update()) {
			++n;
		}
	}
	ens.PopMulti(n);
	return ens.Empty();
}

void EffectNumberManager::Draw() {
	for (size_t i = 0, e = ens.Count(); i < e; ++i) {
		ens[i].Draw();
	}
}
