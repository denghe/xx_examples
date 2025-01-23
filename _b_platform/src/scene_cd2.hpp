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
		//for (auto& o : scene->blocks.items) {
		//	if (o->IsCross(pos, size)) {
		//		auto [x, dx] = o->PushOut<true>({ pos.x, lastPos.y }, size);
		//		auto [y, dy] = o->PushOut<false>({ lastPos.x, pos.y }, size);
		//		pos.x = x;
		//		pos.y = y;
		//		break;
		//	}
		//}
	}

	inline bool Character::HasCross(XYi const& newPos_) const {
		for (auto& o : scene->blocks.items) {
			if (o->IsCross(newPos_, size)) return true;
		}
		return false;
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

	//template<int32_t isPushX>
	//inline std::pair<int32_t, PushOutWays> Block::PushOut(XYi const& cPos, XYi const& cSize) const {
	//	// calculate 4 way distance & choose min val
	//	auto bPosRB = pos + size;	// RB: right bottom
	//	auto bCenter = pos + XYi{ size.x >> 1, size.y >> 1 };
	//	auto cPosRB = cPos + cSize;
	//	auto cCenter = cPos + XYi{ cSize.x >> 1, cSize.y >> 1 };
	//	int32_t dLeft, dRight, dUp, dDown;
	//	if constexpr (isPushX) {
	//		if (cCenter.x >= bCenter.x) {
	//			dLeft = cPos.x - pos.x + cSize.x;
	//			dRight = bPosRB.x - cPos.x;
	//		} else {
	//			dLeft = cPosRB.x - pos.x;
	//			dRight = bPosRB.x - cPos.x;
	//		}
	//		if (dRight <= dLeft) {
	//			return { cPos.x + dRight, PushOutWays::Right };
	//		} else {
	//			return { cPos.x - dLeft, PushOutWays::Left };
	//		}
	//	}
	//	else {
	//		if (cCenter.y >= bCenter.y) {
	//			dUp = cPos.y - pos.y + cSize.y;
	//			dDown = bPosRB.y - cPos.y;
	//		}
	//		else {
	//			dUp = cPosRB.y - pos.y;
	//			dDown = pos.y - cPos.y + cSize.y;
	//		}
	//		if (dUp <= dDown) {
	//			return { cPos.y - dUp, PushOutWays::Up };
	//		} else {
	//			return { cPos.y + dDown, PushOutWays::Down };
	//		}
	//	}
	//}

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
				blocks.Add(xx::MakeShared<Block>())->Init(this, { 64 * x, 64 * y }, { 64, 64 });
			}
			++x;
		}

		// todo: fill wayout?

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
