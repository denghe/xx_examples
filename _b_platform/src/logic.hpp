#pragma once

inline void Item::Update() {
	// todo: phys sim ?
}

inline void Item::Draw() {
	auto& frame = *gRes.quad;
	auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
	q.pos = gLooper.camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0.5f };
	q.scale = size * gLooper.camera.scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = xx::RGBA8_White;
	q.texRect.data = frame.textureRect.data;
}

/***************************************************************************************/
/***************************************************************************************/

inline void Character::Init() {
}

inline void Character::Update() {
	int32_t moveDir;
	if (gLooper.KeyDown(xx::KeyboardKeys::A) && gLooper.KeyDown(xx::KeyboardKeys::D)) {
		if (lastKey == xx::KeyboardKeys::A) {
			moveDir = 1;
		} else if (lastKey == xx::KeyboardKeys::D) {
			moveDir = -1;
		}
	} else if (gLooper.KeyDown(xx::KeyboardKeys::A)) {
		lastKey = xx::KeyboardKeys::A;
		moveDir = -1;
	} else if (gLooper.KeyDown(xx::KeyboardKeys::D)) {
		lastKey = xx::KeyboardKeys::D;
		moveDir = 1;
	} else {
		lastKey = xx::KeyboardKeys::Null;
		moveDir = 0;
	}

	if (moveDir) {
		pos.x += cGravity * moveDir;
	}
}

/***************************************************************************************/
/***************************************************************************************/

inline void Block::Draw() {
	auto& frame = *gRes.quad;
	auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
	q.pos = gLooper.camera.ToGLPos(aabb.from);
	q.anchor = { 0, 1 };
	q.scale = XY{ (aabb.to.x - aabb.from.x) / 32.f, (aabb.to.y - aabb.from.y) / 32.f } * gLooper.camera.scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = xx::RGBA8_Blue;
	q.texRect.data = frame.textureRect.data;
}

/***************************************************************************************/
/***************************************************************************************/

inline void Platform::Draw() {
	auto& frame = *gRes.quad;
	auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
	q.pos = gLooper.camera.ToGLPos(x.from, y);
	q.anchor = { 0, 1 };
	q.scale = XY{ (x.to - x.from) / 32.f, 1.f / 32.f } *gLooper.camera.scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = xx::RGBA8_Green;
	q.texRect.data = frame.textureRect.data;
}

/***************************************************************************************/
/***************************************************************************************/

inline void Logic::Init() {
	character.Emplace()->Init();
	// todo: make some blocks & platforms
}

inline void Logic::Update() {
	character->Update();
}

inline void Logic::Draw() {
	for (auto& o : blocks) o.Draw();
	for (auto& o : platforms) o.Draw();
	character->Draw();
}
