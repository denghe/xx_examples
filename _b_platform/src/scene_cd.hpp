#pragma once

namespace CollisionDetection {

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
		auto mp = XYi{ 1, -1 } * (gLooper.mouse.pos / gLooper.camera.scale);	// get mouse logic pos
		auto mb = gLooper.mouse.PressedMBLeft();	// get mouse left button state

		if (!gLooper.mouseEventHandler) {			// mouse not in ui area
			if (lastMBState != mb) {				// state changed
				lastMBState = mb;
				if (mb) {							// mouse down
					if (!(mp.x < pos.x || mp.y < pos.y || mp.x >= pos.x + size.x || mp.y >= pos.y + size.y)) {
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
		auto bak = pos;
		bool hasCross{};
		int32_t count{};
	LabBegin:
		for (auto& o : scene->blocks) {				// todo: optimize, sort by nearly
			if (o.IsCross(*this)) {
				hasCross = true;
				auto [newPos, isForceOpen] = o.PushOut(*this);
				if (pos != newPos) {
					pos = newPos;
					if (isForceOpen && ++count < 4) goto LabBegin;
					break;
				}
			}
		}
		if (hasCross && bak == pos) {
			// todo: log?
		}
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

	
	// has bug when box1 & box2 is neighbor
	//XX_INLINE bool IsCross_BoxBox(XYi const& b1minXY, XYi const& b1maxXY, XYi const& b2minXY, XYi const& b2maxXY) {
	//	return !(b1maxXY.x < b2minXY.x || b2maxXY.x < b1minXY.x || b1maxXY.y < b2minXY.y || b2maxXY.y < b1minXY.y);
	//}

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

	/***************************************************************************************/
	/***************************************************************************************/

	inline void Scene::Init() {
		character.Emplace()->Init(this, { -128, -128 }, { 128, 128 });

		blocks.Emplace().Init(this, { 0, 0 }, { 128, 128 });
		blocks.Emplace().Init(this, { -180, 0 }, { 128, 128 }, { false, true, true, false });
		blocks.Emplace().Init(this, { -180, 180 }, { 128, 128 });

		blocks.Emplace().Init(this, { 300, 0 }, { 210, 10 }, { false, false, true, false });
		blocks.Emplace().Init(this, { 400, 10 }, { 10, 200 });
		blocks.Emplace().Init(this, { 300, 210 }, { 210, 10 }, { true, false, false, false });

		blocks.Emplace().Init(this, { 0, -400 }, { 10, 200 }, { false, true, false, false });
		blocks.Emplace().Init(this, { 30, -400 }, { 10, 200 }, { false, false, false, true });

		blocks.Emplace().Init(this, { -400, 300 }, { 22, 22 });

		blocks.Emplace().Init(this, { -700, -300 }, { 333, 333 });
	}

	inline void Scene::Update() {
		character->Update();
	}

	inline void Scene::Draw() {
		for (auto& o : blocks) o.Draw();
		character->Draw();
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(pushout test)  move: mouse drag( white box )", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
