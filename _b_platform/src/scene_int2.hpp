#pragma once

namespace IntVersion2 {

	XX_INLINE void Item::Init(Scene* scene_, XYi const& pos_, XYi const& size_, xx::RGBA8 color_) {
		scene = scene_;
		_pos = pos = pos_;
		size = size_;
		color = color_;
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

	inline Item::~Item() {
	}

	inline bool Item::Update() {
		return false;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Character& Character::Init(Scene* scene_, XYi const& pos_, XYi const& size_) {
		Item::Init(scene_, pos_, size_, xx::RGBA8_White);
		return *this;
	}

	inline bool Character::Update() {
		auto bak = pos;
		auto bakRB = bak + size;

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

		if (!attachedPlatform) {
			// handle gravity
			ySpeed += cGravity;
			if (ySpeed > cYSpeedMax) {
				ySpeed = cYSpeedMax;
			}
			_pos.y += ySpeed;
		}
		pos = _pos.As<int32_t>();

		if (ySpeed > FX64_0) {
			++fallingFrameCount;
		}

		// watch full jump max height for easy config gravity & speed
		//xx::CoutN(pos.y);

		if (attachedPlatform) {
			auto a = attachedPlatform.pointer();
			_pos.y = pos.y = a->pos.y - size.y;		// force sync pos
			AttachPlatform({});						// detach
			goto LabCheckPlatform;
		}
		if (ySpeed > FX64_0) {
		LabCheckPlatform:							// handle platforms( maybe cross 2 platforms )
			auto posRB = pos + size;
			std::array<xx::Weak<Platform>, 2> ps;
			int32_t psi{};
			for (auto& o : scene->platforms) {
				if (bakRB.y <= o->pos.y && o->pos.y <= posRB.y) {						// y cross
					if (!(posRB.x <= o->pos.x || pos.x >= o->pos.x + o->size.x)) {		// stand on the platform
						ps[psi++] = o;
					}
				}
			}
			if (psi) {
				longJumpStoped = doubleJumped = jumping = false;
				fallingFrameCount = bigJumpFrameCount = 0;
				ySpeed = 0;
				_pos.y = pos.y = ps[0]->pos.y - size.y;
			}
			if (psi == 1) {
				AttachPlatform(std::move(ps[0]));
			} else if (psi == 2) {
				auto a = ps[0].pointer();
				auto b = ps[1].pointer();
				assert(a->pos.x < b->pos.x);
				auto ad = a->pos.x + a->size.x - pos.x;
				auto bd = pos.x + size.x - b->pos.x;
				assert(ad >= 0 && bd >= 0);
				if (ad >= bd) {									// compare intersect len
					AttachPlatform(std::move(ps[0]));
				} else {
					AttachPlatform(std::move(ps[1]));
				}
			} else {
				assert(psi == 0);
				AttachPlatform({});
			}
		}


		// handle blocks
		PushOutWays pushOutWays{};
		auto& bs = scene->blocks;
		//if (pos.x < 0 || pos.x >= bs.gridSize.x || pos.y < 0 || pos.y >= bs.gridSize.y) return;
		auto posBR = pos + size;
		//if (posBR.x < 0 || posBR.x >= bs.gridSize.x || posBR.y < 0 || posBR.y >= bs.gridSize.y) return;
		auto criFrom = scene->blocks.PosToColRowIndex(pos);
		auto criTo = scene->blocks.PosToColRowIndex(posBR);
		assert(criFrom.x - criTo.x <= 1 && criFrom.y - criTo.y <= 1);
		// bb
		// bf
		if (criFrom == criTo) {
			if (auto bc = bs.At(criFrom - 1); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
			if (auto bc = bs.At({ criFrom.x - 1, criFrom.y }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
			if (auto bc = bs.At({ criFrom.x, criFrom.y - 1 }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
			if (auto bc = bs.At(criFrom); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
		}
		// fc
		// ct
		else {
			if (auto bc = bs.At(criFrom); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
			if (auto bc = bs.At({ criTo.x, criFrom.y }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
			if (auto bc = bs.At({ criFrom.x, criTo.y }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
			if (auto bc = bs.At(criTo); bc && (bc->IsCross(pos, size))) {
				auto [newPos, pushOutWay] = bc->PushOut(pos, size);
				if (pushOutWay != PushOutWays::Unknown) {
					pos = newPos;
					(uint32_t&)pushOutWays |= (uint32_t&)pushOutWay;
				}
			}
		}
		if ((uint32_t)pushOutWays) {

			// todo: ensure attach platform

			_pos = pos;
			if (fallingFrameCount && ((uint32_t&)pushOutWays & (uint32_t)PushOutWays::Up) > 0) {
				longJumpStoped = doubleJumped = jumping = false;
				fallingFrameCount = bigJumpFrameCount = 0;
				ySpeed = 0;
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
				AttachPlatform({});
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

		return false;
	}

	inline void Character::AttachPlatform(xx::Weak<Platform> platform) {
		if (attachedPlatform == platform) return;
		if (attachedPlatform) {
			auto& acs = attachedPlatform->attachedCharacters;
			acs.Back()->attachedPlatformIndex = attachedPlatformIndex;
			acs.SwapRemoveAt(attachedPlatformIndex);
			attachedPlatformIndex = -1;
			attachedPlatform.Reset();
		}
		if (platform) {
			auto w = xx::WeakFromThis(this);
			assert(platform->attachedCharacters.Find(w) == -1);
			attachedPlatformIndex = platform->attachedCharacters.len;
			platform->attachedCharacters.Add(std::move(w));
			attachedPlatform = std::move(platform);
		}
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Block& Block::Init(Scene* scene_, XYi const& pos_, XYi const& size_) {
		Item::Init(scene_, pos_, size_, xx::RGBA8_Red);
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

	inline void Block::FillWayout() {
		// search neighbor & set wayout bit
		auto& bs = scene->blocks;
		auto cri = bs.PosToColRowIndex(pos);
		bool atEdge{};

		if (cri.y == 0) {
			wayout.up = false; atEdge = true;
		}
		else wayout.up = !bs.At({ cri.x, cri.y - 1 });

		if (cri.y + 1 == bs.numRows) {
			wayout.down = false; atEdge = true;
		}
		else wayout.down = !bs.At({ cri.x, cri.y + 1 });

		if (cri.x == 0) {
			wayout.left = false; atEdge = true;
		}
		else wayout.left = !bs.At({ cri.x - 1, cri.y });

		if (cri.x + 1 == bs.numCols) {
			wayout.right = false;
		}
		else wayout.right = !bs.At({ cri.x + 1, cri.y });

		if (atEdge && !(uint8_t&)wayout) {
			if (cri.y != 0) wayout.up = true;
			if (cri.y + 1 != bs.numRows) wayout.down = true;
			if (cri.x != 0) wayout.left = true;
			if (cri.x + 1 != bs.numCols) wayout.right = true;
		}
	}

	inline std::pair<XYi, PushOutWays> Block::PushOut(XYi const& cPos, XYi const& cSize) const {
		// calculate 4 way distance & choose min val
		auto bPosRB = pos + size;	// RB: right bottom
		auto bCenter = pos + XYi{ size.x >> 1, size.y >> 1 };
		auto cPosRB = cPos + cSize;
		auto cCenter = cPos + XYi{ cSize.x >> 1, cSize.y >> 1 };
		int32_t dLeft, dRight, dUp, dDown;
		if (cCenter.x >= bCenter.x) {
			if (wayout.left) dLeft = cPos.x - pos.x + cSize.x;
			else dLeft = 0x7FFFFFFF;
			if (wayout.right) dRight = bPosRB.x - cPos.x;
			else dRight = 0x7FFFFFFF;
			if (cCenter.y >= bCenter.y) {
				if (wayout.up) dUp = cPos.y - pos.y + cSize.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = bPosRB.y - cPos.y;
				else dDown = 0x7FFFFFFF;
			} else {
				if (wayout.up) dUp = cPosRB.y - pos.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = pos.y - cPos.y + cSize.y;
				else dDown = 0x7FFFFFFF;
			}
		} else {
			if (wayout.left) dLeft = cPosRB.x - pos.x;
			else dLeft = 0x7FFFFFFF;
			if (wayout.right) dRight = bPosRB.x - cPos.x;
			else dRight = 0x7FFFFFFF;
			if (cCenter.y >= bCenter.y) {
				if (wayout.up) dUp = cPos.y - pos.y + cSize.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = bPosRB.y - cPos.y;
				else dDown = 0x7FFFFFFF;
			} else {
				if (wayout.up) dUp = cPosRB.y - pos.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = pos.y - cPos.y + cSize.y;
				else dDown = 0x7FFFFFFF;
			}
		}
		if (dLeft == 0x7FFFFFFF && dRight == 0x7FFFFFFF && dUp == 0x7FFFFFFF && dDown == 0x7FFFFFFF)
			return { cPos, PushOutWays::Unknown };
		if (dRight <= dLeft && dRight <= dUp && dRight <= dDown)
			return { { cPos.x + dRight, cPos.y }, PushOutWays::Right };
		else if (dLeft <= dRight && dLeft <= dUp && dLeft <= dDown)
			return { { cPos.x - dLeft, cPos.y }, PushOutWays::Left };
		else if (dUp <= dLeft && dUp <= dRight && dUp <= dDown)
			return { { cPos.x, cPos.y - dUp }, PushOutWays::Up };
		else
			return { { cPos.x, cPos.y + dDown }, PushOutWays::Down };
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Platform& Platform::Init(Scene* scene_, XYi const& pos_, int32_t len_) {
		Item::Init(scene_, pos_, { len_, 1 }, xx::RGBA8_Yellow);
		return *this;
	}

	XX_INLINE void Platform::AssignOffset(XYp const& offset) {
		_pos += offset;
		pos = _pos.As<int32_t>();
		for (auto& c : attachedCharacters) {
			c->_pos += offset;
			c->pos = c->_pos.As<int32_t>();
		}
	}

	inline Platform_Slide& Platform_Slide::Init(Scene* scene_, XYi const& posFrom_, XYi const& posTo_
		, int32_t len_, int32_t moveDurationMS_, int32_t idleDurationMS_) {
		Item::Init(scene_, posFrom_, { len_, 1 }, xx::RGBA8_Green);
		posFrom = posFrom_;
		posTo = posTo_;
		moveFrames = (FX64{ moveDurationMS_ } / FX64{ 1000 } / FX64{ Cfg::frameDelay }).ToInt();
		posInc = XYp{ posTo - posFrom } / moveFrames;
		idleFrames = (FX64{ idleDurationMS_ } / FX64{ 1000 } / FX64{ Cfg::frameDelay }).ToInt();
		return *this;
	}

	inline bool Platform_Slide::Update() {
		XX_BEGIN;
		while (true) {
			for (i = 0; i < moveFrames; ++i) {
				AssignOffset(posInc);
				XX_YIELD_F;
			}
			for (i = 0; i < idleFrames; ++i) {
				XX_YIELD_F;
			}
			posInc = -posInc;
		}
		XX_YIELD_F_TO_BEGIN
		XX_END;
		return false;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline void Scene::Init() {

		static constexpr std::string_view mapText{ R"(
####################
#        O         #
#      //vv\\      #
#                  #
##                 #
# #   >>>    <<<   #
#  #            #  #
#   #----------### #
#    #        #    #
####################
)" };
		// detect map max size
		int32_t maxX{}, x{}, y{};
		for (int i = 0; i < mapText.size(); ++i) {
			auto c = mapText[i];
			if (c == '\n') {
				if (maxX < x) maxX = x;
				x = 0;
				++y;
			} else {
				++x;
			}
		}
		blocks.Init(y - 1, maxX, { 64, 64 });

		// fill map contents
		x = 0;
		y = -1;
		for (int i = 0; i < mapText.size(); ++i) {
			switch (auto c = mapText[i]) {
			case '\n':
				x = 0;
				++y;
				continue;
			case 'O':
				character.Emplace()->Init(this, { 64 * x, 64 * y }, {32, 48});
				break;
			case '#': {
				auto block = xx::MakeShared<Block>();
				block->Init(this, { 64 * x, 64 * y }, { 64, 64 });
				blocks.Add(std::move(block));
				break;
			}
			case '-': {
				// todo: combine
				platforms.Emplace().Emplace()->Init(this, { 64 * x, 64 * y }, 64);
				break;
			}
			case '>': {
				// todo: combine
				platforms.Emplace().Emplace<Platform_Slide>()->Init(this
					, { 64 * x, 64 * y }
					, { 64 * x + 64 * 2, 64 * y }
					, 64
					, 500
					, 1000);
				break;
			}
			case '<': {
				// todo: combine
				platforms.Emplace().Emplace<Platform_Slide>()->Init(this
					, { 64 * x, 64 * y }
					, { 64 * x - 64 * 2, 64 * y }
					, 64
					, 500
					, 1000);
				break;
			}
			case 'v': {
				// todo: combine
				platforms.Emplace().Emplace<Platform_Slide>()->Init(this
					, { 64 * x, 64 * y }
					, { 64 * x, 64 * y + 64 * 2 }
					, 64
					, 240
					, 10);
				break;
			}
			case '/': {
				// todo: combine
				platforms.Emplace().Emplace<Platform_Slide>()->Init(this
					, { 64 * x, 64 * y }
					, { 64 * x - 64 * 2, 64 * y + 64 * 2 }
					, 64
					, 60
					, 10);
				break;
			}
			case '\\': {
				// todo: combine
				platforms.Emplace().Emplace<Platform_Slide>()->Init(this
					, { 64 * x, 64 * y }
					, { 64 * x + 64 * 2, 64 * y + 64 * 2 }
					, 64
					, 120
					, 10);
				break;
			}
			}
			++x;
		}

		for (auto& o : blocks.items) o->FillWayout();


		gLooper.camera.SetOriginal({ 64 * maxX / 2, 64 * y / 2 });

	}

	inline void Scene::Update() {
		for (auto& o : blocks.items) o->Update();
		for (auto& o : platforms) o->Update();
		character->Update();

		//gLooper.camera.SetOriginal(character->pos);

		//// performance test
		//auto secs = xx::NowEpochSeconds();
		//for (int i = 0; i < 100000000; ++i) {
		//	platforms[0].Update();
		//}
		//xx::CoutN(xx::NowEpochSeconds(secs));
	}

	inline void Scene::Draw() {
		for (auto& o : blocks.items) o->Draw();
		for (auto& o : platforms) o->Draw();
		character->Draw();
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(play)  move: A / D     jump: SPACE      down jump: S+SPACE", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
