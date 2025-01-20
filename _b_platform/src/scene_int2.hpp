#pragma once

namespace IntVersion2 {

	XX_INLINE void Item::Init(Scene* scene_, XYi const& pos_, XYi const& size_) {
		scene = scene_;
		pos = pos_;
		size = size_;
	}

	inline void Item::Draw() {
		auto& frame = *gRes.quad;
		auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
		q.pos = gLooper.camera.ToGLPos(pos.As<float>());
		q.anchor = { 0.f, 1.f };
		q.scale = size.As<float>() / cResSize * gLooper.camera.scale;
		q.radians = 0;
		q.colorplus = 1;
		q.color = color;
		q.texRect.data = frame.textureRect.data;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Character& Character::Init(Scene* scene_, XYi const& pos_, XYi const& size_) {
		Item::Init(scene_, pos_, size_);
		color = xx::RGBA8_White;
		return *this;
	}

	inline void Character::Update() {
		auto bak = pos;

		// left right move command check
		int32_t moveDir;
		if (gLooper.KeyDown(xx::KeyboardKeys::A) && gLooper.KeyDown(xx::KeyboardKeys::D)) {
			if (lastXMoveDirection == -1) {
				moveDir = 1;
			}
			else if (lastXMoveDirection == 1) {
				moveDir = -1;
			}
			else {
				moveDir = 0;
			}
		}
		else if (gLooper.KeyDown(xx::KeyboardKeys::A)) {
			lastXMoveDirection = moveDir = -1;
		}
		else if (gLooper.KeyDown(xx::KeyboardKeys::D)) {
			lastXMoveDirection = moveDir = 1;
		}
		else {
			lastXMoveDirection = moveDir = 0;
		}

		// left right move
		if (moveDir == 1) {
			_pos.x += cXSpeed;
		}
		else if (moveDir == -1) {
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
			auto bakRB = bak + size;
			auto posRB = pos + size;
			for (auto& o : scene->platforms) {							// todo: space index optimize
				if (bakRB.y <= o.pos.y && o.pos.y <= posRB.y) {
					if (!(posRB.x <= o.pos.x || pos.x >= o.pos.x + o.size.x)) {
						longJumpStoped = doubleJumped = jumping = false;
						fallingFrameCount = bigJumpFrameCount = 0;
						ySpeed = 0;
						_pos.y = pos.y = o.pos.y - size.y;
						break;
					}
				}
			}
		}

		// handle blocks
		bool hasCross{};
		int32_t count{};
	LabBegin:
		for (auto& o : scene->blocks) {				// todo: optimize, sort by nearly
			if (o.IsCross(*this)) {
				hasCross = true;
				auto [newPos, isForceOpen] = o.PushOut(*this);
				if (pos != newPos) {
					pos = newPos;
					_pos = pos;
					if (isForceOpen && ++count < 4) goto LabBegin;
					break;
				}
			}
		}
		// if (hasCross && bak == pos) // todo: log?

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
		}
		else {
			if (firstJumpPressed && !doubleJumped) {
				doubleJumped = true;
				longJumpStoped = false;
				bigJumpFrameCount = 0;
				ySpeed = cYSpeedInit;
			}
			else if (longJumpPressed && !longJumpStoped) {
				++bigJumpFrameCount;
				if (bigJumpFrameCount < cBigJumpNumFrames) {
					ySpeed = cYSpeedInit;
				}
			}
			else {
				if constexpr (cEnableStrictJumpMode) {
					longJumpStoped = true;
				}
			}
		}
		lastJumpPressed = jumpPressed;
	}

	inline bool Character::HasCross(XYi const& newPos_) const {
		for (auto& o : scene->blocks) {				// todo: optimize
			if (o.IsCross(newPos_, size)) return true;
		}
		return false;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Block& Block::Init(Scene* scene_, XYi const& pos_, XYi const& size_, xx::Math::BlockWayout blockWayout_) {
		Item::Init(scene_, pos_, size_);
		blockWayout = blockWayout_;
		color = xx::RGBA8_Red;
		return *this;
	}

	XX_INLINE bool Block::IsCross(XYi const& cPos, XYi const& cSize) const {
		if (cPos.x >= pos.x) {
			if (cPos.y >= pos.y) {
				if (cPos.x < pos.x + size.x) return cPos.y < pos.y + size.y;
			}
			else /* cPos.y < pos.y */ {
				if (cPos.x < pos.x + size.x) return cPos.y + cSize.y > pos.y;
			}
		}
		else /* cPos.x < pos.x */ {
			if (cPos.y >= pos.y) {
				if (cPos.x + cSize.x > pos.x) return cPos.y < pos.y + size.y;
			}
			else /* cPos.y < pos.y */ {
				if (cPos.x + cSize.x > pos.x) return cPos.y + cSize.y > pos.y;
			}
		}
		return false;
	}

	XX_INLINE bool Block::IsCross(Character const& c) const {
		return IsCross(c.pos, c.size);
	}

	inline std::pair<XYi, bool> Block::PushOut(Character const& c) const {
		// calculate 4 way distance & choose min val
		auto bPosRB = pos + size;	// RB: right bottom
		auto bCenter = pos + XYi{ size.x >> 1, size.y >> 1 };
		auto cPosRB = c.pos + c.size;
		auto cCenter = c.pos + XYi{ c.size.x >> 1, c.size.y >> 1 };
		int32_t dLeft, dRight, dUp, dDown;
		if (cCenter.x >= bCenter.x) {
			dLeft = c.pos.x - pos.x + c.size.x;
			dRight = bPosRB.x - c.pos.x;
			if (cCenter.y >= bCenter.y) {
				dUp = c.pos.y - pos.y + c.size.y;
				dDown = bPosRB.y - c.pos.y;
			} 
			else {
				dUp = cPosRB.y - pos.y;
				dDown = pos.y - c.pos.y + c.size.y;
			}
		}
		else {
			dLeft = cPosRB.x - pos.x;
			dRight = bPosRB.x - c.pos.x;
			if (cCenter.y >= bCenter.y) {
				dUp = c.pos.y - pos.y + c.size.y;
				dDown = bPosRB.y - c.pos.y;
			}
			else {
				dUp = cPosRB.y - pos.y;
				dDown = pos.y - c.pos.y + c.size.y;
			}
		}
		XYi newPos;
		if (dRight <= dLeft && dRight <= dUp && dRight <= dDown) {
			newPos = { c.pos.x + dRight, c.pos.y };
			if (blockWayout.right || !c.HasCross(newPos)) return { newPos, blockWayout.right };
			if (dLeft <= dUp && dLeft <= dDown) {
				newPos = { c.pos.x - dLeft, c.pos.y };
				if (blockWayout.left || !c.HasCross(newPos)) return { newPos, blockWayout.left };
				if (dUp <= dDown) {
					newPos = { c.pos.x, c.pos.y - dUp };
					if (blockWayout.up || !c.HasCross(newPos)) return { newPos, blockWayout.up };
				}
				else {
					newPos = { c.pos.x, c.pos.y + dDown };
					if (blockWayout.down || !c.HasCross(newPos)) return { newPos, blockWayout.down };
				}
			}
			else if (dUp <= dDown) {
				newPos = { c.pos.x, c.pos.y - dUp };
				if (blockWayout.up || !c.HasCross(newPos)) return { newPos, blockWayout.up };
			}
			else {
				newPos = { c.pos.x, c.pos.y + dDown };
				if (blockWayout.down || !c.HasCross(newPos)) return { newPos, blockWayout.down };
			}
		}
		else if (dLeft <= dRight && dLeft <= dUp && dLeft <= dDown) {
			newPos = { c.pos.x - dLeft, c.pos.y };
			if (blockWayout.left || !c.HasCross(newPos)) return { newPos, blockWayout.left };
			if (dRight <= dUp && dRight <= dDown) {
				newPos = { c.pos.x + dRight, c.pos.y };
				if (blockWayout.right || !c.HasCross(newPos)) return { newPos, blockWayout.right };
				if (dUp <= dDown) {
					newPos = { c.pos.x, c.pos.y - dUp };
					if (blockWayout.up || !c.HasCross(newPos)) return { newPos, blockWayout.up };
				}
				else {
					newPos = { c.pos.x, c.pos.y + dDown };
					if (blockWayout.down || !c.HasCross(newPos)) return { newPos, blockWayout.down };
				}
			}
			else if (dUp <= dDown) {
				newPos = { c.pos.x, c.pos.y - dUp };
				if (blockWayout.up || !c.HasCross(newPos)) return { newPos, blockWayout.up };
			}
			else {
				newPos = { c.pos.x, c.pos.y + dDown };
				if (blockWayout.down || !c.HasCross(newPos)) return { newPos, blockWayout.down };
			}
		}
		else if (dUp <= dLeft && dUp <= dRight && dUp <= dDown) {
			newPos = { c.pos.x, c.pos.y - dUp };
			if (blockWayout.up || !c.HasCross(newPos)) return { newPos, blockWayout.up };
			if (dDown <= dLeft && dDown <= dRight) {
				newPos = { c.pos.x, c.pos.y + dDown };
				if (blockWayout.down || !c.HasCross(newPos)) return { newPos, blockWayout.down };
				if (dLeft <= dRight) {
					newPos = { c.pos.x - dLeft, c.pos.y };
					if (blockWayout.left || !c.HasCross(newPos)) return { newPos, blockWayout.left };
				}
				else {
					newPos = { c.pos.x + dRight, c.pos.y };
					if (blockWayout.right || !c.HasCross(newPos)) return { newPos, blockWayout.right };
				}
			}
			else if (dLeft <= dRight) {
				newPos = { c.pos.x - dLeft, c.pos.y };
				if (blockWayout.left || !c.HasCross(newPos)) return { newPos, blockWayout.left };
			}
			else {
				newPos = { c.pos.x + dRight, c.pos.y };
				if (blockWayout.right || !c.HasCross(newPos)) return { newPos, blockWayout.right };
			}
		}
		else if (dDown <= dLeft && dDown <= dRight && dDown <= dUp) {
			newPos = { c.pos.x, c.pos.y + dDown };
			if (blockWayout.down || !c.HasCross(newPos)) return { newPos, blockWayout.down };
			if (dUp <= dLeft && dUp <= dRight) {
				newPos = { c.pos.x, c.pos.y - dUp };
				if (blockWayout.up || !c.HasCross(newPos)) return { newPos, blockWayout.up };
				if (dLeft <= dRight) {
					newPos = { c.pos.x - dLeft, c.pos.y };
					if (blockWayout.left || !c.HasCross(newPos)) return { newPos, blockWayout.left };
				}
				else {
					newPos = { c.pos.x + dRight, c.pos.y };
					if (blockWayout.right || !c.HasCross(newPos)) return { newPos, blockWayout.right };
				}
			}
			else if (dLeft <= dRight) {
				newPos = { c.pos.x - dLeft, c.pos.y };
				if (blockWayout.left || !c.HasCross(newPos)) return { newPos, blockWayout.left };
			}
			else {
				newPos = { c.pos.x + dRight, c.pos.y };
				if (blockWayout.right || !c.HasCross(newPos)) return { newPos, blockWayout.right };
			}
		}
		return { c.pos, false };
	}

	inline void Block::Update() {
		// todo
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Platform& Platform::Init(Scene* scene_, XYi const& pos_, int32_t len_) {
		Item::Init(scene_, pos_, {len_, 1});
		color = xx::RGBA8_Yellow;
		return *this;
	}

	inline void Platform::Update() {
		static constexpr FX64 moveDistance{ 50 };
		static constexpr FX64 moveSpeed{ 0.2 };

		switch (lineNumber) { case 0:
			xOriginal = pos.x;
			for (xOffset = FX64_0; xOffset <= moveDistance; xOffset += moveSpeed) {
				pos.x = (xOriginal + xOffset).ToInt();
				lineNumber = 1; return; case 1:;
			}

			lineNumber = 2; return; case 2:;

			for (xOffset = moveDistance; xOffset >= FX64_0; xOffset -= moveSpeed) {
				pos.x = (xOriginal + xOffset).ToInt();
				lineNumber = 3; return; case 3:;
			}

			lineNumber = 4; return; case 4:;
			lineNumber = 0; return;
		}
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline void Scene::Init() {
		character.Emplace()->Init(this);

		platforms.Emplace().Init(this, { -64 * 5, -64 * 2 }, 64 * 2);
		platforms.Emplace().Init(this, { 64, 0 }, 64 * 3);
		platforms.Emplace().Init(this, { -64, 64 * 2 }, 64 * 1);
		platforms.Emplace().Init(this, { -64 * 10, 64 * 4 }, 64 * 20);

		blocks.Emplace().Init(this, { -64 * 11, 64 }, { 64, 64 * 3 });
		blocks.Emplace().Init(this, { 64 * 10, 64 }, { 64, 64 * 3 });

		blocks.Emplace().Init(this, { -64 * 5, 64 + 32 }, { 64 * 4, 64 });
	}

	inline void Scene::Update() {
		for (auto& o : blocks) o.Update();
		for (auto& o : platforms) o.Update();
		character->Update();
	}

	inline void Scene::Draw() {
		for (auto& o : blocks) o.Draw();
		for (auto& o : platforms) o.Draw();
		character->Draw();
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "move: A / D     jump: SPACE      down jump: S+SPACE", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
