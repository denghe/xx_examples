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
		bool posChanged{}, hasCross{};
		for (auto& o : scene->blocks) {				// todo: optimize
			if (o.IsCross(*this)) {
				hasCross = true;
				auto newPos = o.PushOut(*this);
				if (pos != newPos) {
					pos = newPos;
					posChanged = true;
					break;
				}
			}
		}
		if (hasCross && !posChanged) {
			// todo: 1 vs 3+
			// scan all cross path + distance & combine
		}
	}

	inline bool Character::HasCross(XYi const& tarPos_) const {
		for (auto& o : scene->blocks) {				// todo: optimize
			if (o.IsCross(tarPos_, size)) return true;
		}
		return false;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Block& Block::Init(Scene* scene_, XYi const& pos_, XYi const& size_) {
		Item::Init(scene_, pos_, size_);
		color = xx::RGBA8_Blue;
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

	inline XYi Block::PushOut(Character const& c) const {
		// calculate 4 way distance & choose min val
		auto bPosRB = pos + size;	// RB: right bottom
		auto cPosRB = c.pos + c.size;
		int32_t dLeft, dRight, dUp, dDown;
		if (c.pos.x >= pos.x) {
			dLeft = c.pos.x - pos.x + c.size.x;
			dRight = bPosRB.x - c.pos.x;
			if (c.pos.y >= pos.y) {
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
			dRight = pos.x - c.pos.x + c.size.x;
			if (c.pos.y >= pos.y) {
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
			if (!c.HasCross(newPos)) return newPos;
			if (dLeft <= dUp && dLeft <= dDown) {
				newPos = { c.pos.x - dLeft, c.pos.y };
				if (!c.HasCross(newPos)) return newPos;
				if (dUp <= dDown) {
					newPos = { c.pos.x, c.pos.y - dUp };
					if (!c.HasCross(newPos)) return newPos;
				}
				else {
					newPos = { c.pos.x, c.pos.y + dDown };
					if (!c.HasCross(newPos)) return newPos;
				}
			}
			else if (dUp <= dDown) {
				newPos = { c.pos.x, c.pos.y - dUp };
				if (!c.HasCross(newPos)) return newPos;
			}
			else {
				newPos = { c.pos.x, c.pos.y + dDown };
				if (!c.HasCross(newPos)) return newPos;
			}
		}
		else if (dLeft <= dRight && dLeft <= dUp && dLeft <= dDown) {
			newPos = { c.pos.x - dLeft, c.pos.y };
			if (!c.HasCross(newPos)) return newPos;
			if (dRight <= dUp && dRight <= dDown) {
				newPos = { c.pos.x + dRight, c.pos.y };
				if (!c.HasCross(newPos)) return newPos;
				if (dUp <= dDown) {
					newPos = { c.pos.x, c.pos.y - dUp };
					if (!c.HasCross(newPos)) return newPos;
				}
				else {
					newPos = { c.pos.x, c.pos.y + dDown };
					if (!c.HasCross(newPos)) return newPos;
				}
			}
			else if (dUp <= dDown) {
				newPos = { c.pos.x, c.pos.y - dUp };
				if (!c.HasCross(newPos)) return newPos;
			}
			else {
				newPos = { c.pos.x, c.pos.y + dDown };
				if (!c.HasCross(newPos)) return newPos;
			}
		}
		else if (dUp <= dLeft && dUp <= dRight && dUp <= dDown) {
			newPos = { c.pos.x, c.pos.y - dUp };
			if (!c.HasCross(newPos)) return newPos;
			if (dDown <= dLeft && dDown <= dRight) {
				newPos = { c.pos.x, c.pos.y + dDown };
				if (!c.HasCross(newPos)) return newPos;
				if (dLeft <= dRight) {
					newPos = { c.pos.x - dLeft, c.pos.y };
					if (!c.HasCross(newPos)) return newPos;
				}
				else {
					newPos = { c.pos.x + dRight, c.pos.y };
					if (!c.HasCross(newPos)) return newPos;
				}
			}
			else if (dLeft <= dRight) {
				newPos = { c.pos.x - dLeft, c.pos.y };
				if (!c.HasCross(newPos)) return newPos;
			}
			else {
				newPos = { c.pos.x + dRight, c.pos.y };
				if (!c.HasCross(newPos)) return newPos;
			}
		}
		else if (dDown <= dLeft && dDown <= dRight && dDown <= dUp) {
			newPos = { c.pos.x, c.pos.y + dDown };
			if (!c.HasCross(newPos)) return newPos;
			if (dUp <= dLeft && dUp <= dRight) {
				newPos = { c.pos.x, c.pos.y - dUp };
				if (!c.HasCross(newPos)) return newPos;
				if (dLeft <= dRight) {
					newPos = { c.pos.x - dLeft, c.pos.y };
					if (!c.HasCross(newPos)) return newPos;
				}
				else {
					newPos = { c.pos.x + dRight, c.pos.y };
					if (!c.HasCross(newPos)) return newPos;
				}
			}
			else if (dLeft <= dRight) {
				newPos = { c.pos.x - dLeft, c.pos.y };
				if (!c.HasCross(newPos)) return newPos;
			}
			else {
				newPos = { c.pos.x + dRight, c.pos.y };
				if (!c.HasCross(newPos)) return newPos;
			}
		}
		return c.pos;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline void Scene::Init() {
		character.Emplace()->Init(this, { -128, -128 }, { 128, 128 });
		blocks.Emplace().Init(this, { 0, 0 }, { 128, 128 });
		blocks.Emplace().Init(this, { -180, 0 }, { 128, 128 });
		blocks.Emplace().Init(this, { -180, 180 }, { 128, 128 });
	}

	inline void Scene::Update() {
		character->Update();
	}

	inline void Scene::Draw() {
		for (auto& o : blocks) o.Draw();
		character->Draw();
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(collision)  move: mouse drag( white box )", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
