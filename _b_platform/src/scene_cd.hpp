#pragma once

namespace CollisionDetection {

	inline void Item::Init(Scene* owner_, XYi const& pos_, XYi const& size_) {
		owner = owner_;
		pos = pos_;
		size = size_;
		SyncAABB();
	}

	inline void Item::SyncAABB() {
		aabb.from = pos;
		aabb.to = pos + size;
	}

	inline void Item::Draw() {
		auto& frame = *gRes.quad;
		auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
		q.pos = gLooper.camera.ToGLPos(pos.As<float>());
		q.anchor = { 0.f, 1.f };
		q.scale = XY{ (aabb.to.x - aabb.from.x) / 64.f, (aabb.to.y - aabb.from.y) / 64.f } * gLooper.camera.scale;
		q.radians = 0;
		q.colorplus = 1;
		q.color = color;
		q.texRect.data = frame.textureRect.data;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Character& Character::Init(Scene* owner_, XYi const& pos_, XYi const& size_) {
		Item::Init(owner_, pos_, size_);
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
					if (!(mp.x < aabb.from.x || mp.y < aabb.from.y || mp.x >= aabb.to.x || mp.y >= aabb.to.y)) {
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
			SyncAABB();
			color = { 255, 255, 255, 127 };
		}
	}

	inline void Character::HandleCollision() {
		// do collision check & moveout
		// todo
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Block& Block::Init(Scene* owner_, XYi const& pos_, XYi const& size_) {
		Item::Init(owner_, pos_, size_);
		color = xx::RGBA8_Blue;
		return *this;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline void Scene::Init() {
		character.Emplace()->Init(this, { -128, -128 }, { 128, 128 });
		blocks.Emplace().Init(this, { 0, 0 }, { 128, 128 });
	}

	inline void Scene::Update() {
		character->Update();
	}

	inline void Scene::Draw() {
		for (auto& o : blocks) o.Draw();
		character->Draw();
	}

}
