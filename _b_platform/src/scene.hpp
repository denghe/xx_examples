#pragma once

inline Character& Character::Init(Scene* owner_) {
	owner = owner_;
	// todo: init pos, lastY = pos.y
	return *this;
}

inline void Character::Update() {

	// left right move command check
	int32_t moveDir;
	if (gLooper.KeyDown(xx::KeyboardKeys::A) && gLooper.KeyDown(xx::KeyboardKeys::D)) {
		if (lastKey == xx::KeyboardKeys::A) {
			moveDir = 1;
		} else if (lastKey == xx::KeyboardKeys::D) {
			moveDir = -1;
		} else {
			moveDir = 0;
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

	// left right move
	if (moveDir) {
		pos.x += cMoveSpeed * moveDir;
	}

	// handle jump
	lastY = pos.y;
	if (!jumping && gLooper.KeyDown(xx::KeyboardKeys::Space)) {
		ySpeed = -20;
		jumping = true;
	} else {
		ySpeed += cGravity;
		pos.y += ySpeed;

		// handle block & platform
		if (pos.y > lastY) {
			auto maxX = pos.x + halfWidth;
			auto minX = pos.x - halfWidth;
			
			for (auto& o : owner->platforms) {
				if (lastY <= o.y && o.y <= pos.y) {
					if (!(maxX <= o.x.from || minX >= o.x.to)) {
						jumping = false;
						ySpeed = 0;
						pos.y = o.y;
					}
				}
			}
		}
	}
}

inline void Character::Draw() {
	auto& frame = *gRes.quad;
	auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
	q.pos = gLooper.camera.ToGLPos(pos);
	q.anchor = { 0.5f, 0 };
	q.scale = gLooper.camera.scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = xx::RGBA8_White;
	q.texRect.data = frame.textureRect.data;
}

/***************************************************************************************/
/***************************************************************************************/

inline void Block::Draw() {
	auto& frame = *gRes.quad;
	auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
	q.pos = gLooper.camera.ToGLPos(aabb.from);
	q.anchor = { 0.5f, 0.5f };
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
	auto w = (x.to - x.from) * 0.5f;
	q.pos = gLooper.camera.ToGLPos(x.from + w, y);
	q.anchor = { 0.5f, 1 };
	q.scale = XY{ w * 0.03125f, 0.03125f } * gLooper.camera.scale;
	q.radians = 0;
	q.colorplus = 1;
	q.color = xx::RGBA8_Green;
	q.texRect.data = frame.textureRect.data;
}

/***************************************************************************************/
/***************************************************************************************/

inline void Scene::Init() {
	{
		auto& o = character.Emplace()->Init(this);
		// o.x = xxx
	}
	// todo: make some blocks & platforms
	{
		auto& o = platforms.Emplace();
		o.x.from = -64;
		o.x.to = 64;
		o.y = 64*2;
	}
	{
		auto& o = platforms.Emplace();
		o.x.from = -64*5;
		o.x.to = 64*5;
		o.y = 64*4;
	}
}

inline void Scene::Update() {
	character->Update();
}

inline void Scene::Draw() {
	for (auto& o : blocks) o.Draw();
	for (auto& o : platforms) o.Draw();
	character->Draw();
}
