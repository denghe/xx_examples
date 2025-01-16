#pragma once

namespace IntVersion {

	inline Character& Character::Init(Scene* owner_) {
		owner = owner_;
		// todo: init pos, lastY = pos.y
		return *this;
	}

	inline void Character::Update() {
		auto bak = pos;

		// left right move command check
		int32_t moveDir;
		if (gLooper.KeyDown(xx::KeyboardKeys::A) && gLooper.KeyDown(xx::KeyboardKeys::D)) {
			if (lastXMoveDirection == -1) {
				moveDir = 1;
			} else if (lastXMoveDirection == 1) {
				moveDir = -1;
			} else {
				moveDir = 0;
			}
		} else if (gLooper.KeyDown(xx::KeyboardKeys::A)) {
			lastXMoveDirection = moveDir = -1;
		} else if (gLooper.KeyDown(xx::KeyboardKeys::D)) {
			lastXMoveDirection = moveDir = 1;
		} else {
			lastXMoveDirection = moveDir = 0;
		}

		// left right move
		if (moveDir == 1) {
			_pos.x += cXSpeed;
		} else if (moveDir == -1) {
			_pos.x -= cXSpeed;
		}

		// handle gravity
		ySpeed += cGravity;
		if (ySpeed > cYSpeedMax) {
			ySpeed = cYSpeedMax;
		}
		_pos.y += ySpeed;
		if (ySpeed > FX64_0) {
			++fallingFrameCount;
		}

		// watch full jump max height for easy config gravity & speed
		//xx::CoutN(pos.y);

		// store pos int version
		pos = _pos.As<int32_t>();

		// handle platforms
		if (pos.y > bak.y) {
			auto maxX = pos.x + halfWidth;
			auto minX = pos.x - halfWidth;
			// todo: get platforms from space index find
			for (auto& o : owner->platforms) {
				if (bak.y <= o.y && o.y <= pos.y) {
					if (!(maxX <= o.x.from || minX >= o.x.to)) {
						longJumpStoped = doubleJumped = jumping = false;
						fallingFrameCount = bigJumpFrameCount = 0;
						ySpeed = 0;
						_pos.y = o.y;
						pos.y = o.y;
						break;
					}
				}
			}
		}

		// handle jump
		auto jumpPressed = gLooper.KeyDown(xx::KeyboardKeys::Space);
		auto downPressed = gLooper.KeyDown(xx::KeyboardKeys::S);
		auto downJumpPressed = jumpPressed && downPressed;
		auto longJumpPressed = jumpPressed && !downPressed && lastJumpPressed;
		auto firstJumpPressed = jumpPressed && !downPressed && !lastJumpPressed;
		if (!jumping) {
			if (firstJumpPressed && fallingFrameCount < cCoyoteNumFrames) {
				ySpeed = cYSpeedInit;
				jumping = true;
			}
			if (downJumpPressed && ySpeed == 0) {
				_pos.y += cDownJumpYOffset;
				pos.y = _pos.y.ToInt();
			}
		} else {
			if (firstJumpPressed && !doubleJumped) {
				doubleJumped = true;
				longJumpStoped = false;
				bigJumpFrameCount = 0;
				ySpeed = cYSpeedInit;
			} else if (longJumpPressed && !longJumpStoped) {
				++bigJumpFrameCount;
				if (bigJumpFrameCount < cBigJumpNumFrames) {
					ySpeed = cYSpeedInit;
				}
			} else {
				if constexpr (cEnableStrictJumpMode) {
					longJumpStoped = true;
				}
			}
		}
		lastJumpPressed = jumpPressed;

		// todo: handle blocks
	}

	inline void Character::Draw() {
		auto& frame = *gRes.quad;
		auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
		q.pos = gLooper.camera.ToGLPos(pos.As<float>());
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
		q.scale = XY{ (aabb.to.x - aabb.from.x) / 32.f, (aabb.to.y - aabb.from.y) / 32.f } *gLooper.camera.scale;
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
		q.pos = gLooper.camera.ToGLPos(XY{ x.from + w, y });
		q.anchor = { 0.5f, 1 };
		q.scale = XY{ w * 0.03125f, 0.03125f } *gLooper.camera.scale;
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
			o.x.from = -64 * 5;
			o.x.to = -64 * 3;
			o.y = -64 * 2;
		}
		{
			auto& o = platforms.Emplace();
			o.x.from = 64;
			o.x.to = 64 * 3;
			o.y = 0;
		}
		{
			auto& o = platforms.Emplace();
			o.x.from = -64;
			o.x.to = 64;
			o.y = 64 * 2;
		}
		{
			auto& o = platforms.Emplace();
			o.x.from = -64 * 10;
			o.x.to = 64 * 10;
			o.y = 64 * 4;
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

}
