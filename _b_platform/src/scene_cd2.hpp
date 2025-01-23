#pragma once

namespace CollisionDetection2 {

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
		auto mp = gLooper.camera.ToLogicPos(gLooper.mouse.pos);	// get mouse logic pos
		auto mb = gLooper.mouse.PressedMBLeft();	// get mouse left button state

		if (!gLooper.mouseEventHandler) {			// mouse not in ui area
			if (lastMBState != mb) {				// state changed
				lastMBState = mb;
				if (mb) {							// mouse down
					if (!(mp.x < pos.x || mp.y < pos.y || mp.x >= pos.x + size.x || mp.y >= pos.y + size.y)) {
						lastPos = pos;
						mouseOffset = mp - pos;		// in area: store diff
						dragging = true;			// set flag
					}
				}
				else {								// mouse up
					if (dragging) {
						dragging = false;			// clear flag
						color = xx::RGBA8_White;
						HandleCollision();
					}
				}
			}
		}

		if (dragging) {
			pos = mp - mouseOffset;					// sync pos from mouse
			color = { 255, 255, 255, 127 };
		}
	}

	inline void Character::HandleCollision() {
		auto& bs = scene->blocks;
		if (pos.x < 0 || pos.x >= bs.gridSize.x || pos.y < 0 || pos.y >= bs.gridSize.y) return;
		auto posBR = pos + size;
		if (posBR.x < 0 || posBR.x >= bs.gridSize.x || posBR.y < 0 || posBR.y >= bs.gridSize.y) return;
		auto criFrom = scene->blocks.PosToColRowIndex(pos);
		auto criTo = scene->blocks.PosToColRowIndex(posBR);
		assert(criFrom.x - criTo.x <= 1 && criFrom.y - criTo.y <= 1);
		// bb
		// bf
		if (criFrom == criTo) {
			if (auto bc = bs.At(criFrom - 1); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
			if (auto bc = bs.At({ criFrom.x - 1, criFrom.y }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
			if (auto bc = bs.At({ criFrom.x, criFrom.y - 1 }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
			if (auto bc = bs.At(criFrom); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
		}
		// fc
		// ct
		else {
			if (auto bc = bs.At(criFrom); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
			if (auto bc = bs.At({ criTo.x, criFrom.y }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
			if (auto bc = bs.At({ criFrom.x, criTo.y }); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
			if (auto bc = bs.At(criTo); bc && (bc->IsCross(pos, size))) {
				auto [newPos, way] = bc->PushOut(pos, size);
				pos = newPos;
			}
		}
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Block& Block::Init(Scene* scene_, XYi const& pos_, XYi const& size_) {
		Item::Init(scene_, pos_, size_);
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

	inline void Block::FillWayout() {
		// search neighbor & set wayout bit
		auto& bs = scene->blocks;
		auto cri = bs.PosToColRowIndex(pos);
		if (cri.y == 0) wayout.up = false;
		else wayout.up = !bs.At({ cri.x, cri.y - 1 });
		if (cri.y + 1 == bs.numRows) wayout.down = false;
		else wayout.down = !bs.At({ cri.x, cri.y + 1 });
		if (cri.x == 0) wayout.left = false;
		else wayout.left = !bs.At({ cri.x - 1, cri.y });
		if (cri.x + 1 == bs.numCols) wayout.right = false;
		else wayout.right = !bs.At({ cri.x + 1, cri.y });
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

	inline void Scene::Init() {
		static constexpr std::string_view mapText{ R"(
####################
#                  #
#        O         #
#                  #
##                 #
# #  ---   ---     #
#  #               #
#   #              #
#    #             #
####################
)" };
		int32_t maxX{}, x{}, y{};
		for (int i = 0; i < mapText.size(); ++i) {
			auto c = mapText[i];
			if (c == '\n') {
				if (maxX < x) maxX = x;
				x = 0;
				++y;
				continue;
			} else {
				++x;
				continue;
			}
		}

		blocks.Init(y, maxX, { 64, 64 });

		x = 0;
		y = 0;
		int32_t cx{}, cy{};
		for (int i = 0; i < mapText.size(); ++i) {
			auto c = mapText[i];
			if (c == '\n') {
				x = 0;
				++y;
				continue;
			}
			if (c == 'O') {
				cx = x;
				cy = y;
			}
			else if (c == '#' || c == '-') {
				auto block = xx::MakeShared<Block>();
				block->Init(this, { 64 * x, 64 * y }, { 64, 64 });
				blocks.Add(std::move(block));
			}
			++x;
		}

		for (auto& o : blocks.items) o->FillWayout();

		character.Emplace()->Init(this, { 64 * cx, 64 * cy }, { 32, 48 });

		gLooper.camera.SetOriginal({ 64 * maxX / 2, 64 * y / 2 });
	}

	inline void Scene::Update() {
		character->Update();
	}

	inline void Scene::Draw() {
		for (auto& o : blocks.items) o->Draw();
		character->Draw();
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(pushout test)  move: mouse drag( white box )", xx::RGBA8_Green, { 0.5f, 1 });
	}
}
