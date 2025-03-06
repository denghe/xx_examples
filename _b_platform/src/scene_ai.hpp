#pragma once

namespace AI {

	inline void Item::Init(Scene* scene_, XYi const& pos_) {
		scene = scene_;
		pos = pos_;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	XX_INLINE int32_t Character::CIndexToPosX(int32_t const& cIndex) {
		return MapCfg::cellSize.x * cIndex + (MapCfg::cellSize.x >> 1);
	}

	XX_INLINE int32_t Character::RIndexToPosY(int32_t const& rIndex) {
		return MapCfg::cellSize.y * rIndex + MapCfg::cellSize.y - 1;
	}

	XX_INLINE XYi Character::CRIndexToPos(XYi const& crIndex) {
		return { CIndexToPosX(crIndex.x), RIndexToPosY(crIndex.y) };
	}

	XX_INLINE XYi Character::GetCRIndex() const {
		return scene->blocks.PosToColRowIndex(pos);
	}

	inline Character& Character::Init(Scene* scene_, XYi const& crIndex) {
		auto p = CRIndexToPos(crIndex);
		Item::Init(scene_, p);
		_pos = p;	// sync float version
		plan = PlanTypes::Idle;
		return *this;
	}

	inline bool Character::Update() {

	LabAction:
		if (currentActionIndex >= 0) {
			auto& currAction = actions[currentActionIndex];
			switch (currAction.type) {
			case ActionTypes::None: {
				currentActionIndex = -1;
				goto LabAction;
			}
			case ActionTypes::Move: {
				auto& a = (Action_Move&)currAction;
				if (Move(a.tarPos)) {
					++currentActionIndex;
				}
				break;
			}
			case ActionTypes::Jump: {
				auto& a = (Action_Jump&)currAction;
				if (Jump(a.tarPos)) {
					++currentActionIndex;
				}
				break;
			}
			case ActionTypes::Fall: {
				xx::CoutN("todo ", __LINE__);
				break;
			}
			}

			return false;
		}

	//LabPlan:
		switch (plan) {
		case PlanTypes::Idle:
			break;
		case PlanTypes::MoveTo: {
			// pos to cri
			auto crIndex = GetCRIndex();

			// try get current block
			auto block = scene->blocks.TryAt(crIndex + XYi{ 0, 1 });

			// flying?
			if (!block)
				break;

			// calculate max y in current cell
			auto tarPosY = RIndexToPosY(crIndex.y);

			// flying?
			if (pos.y != tarPosY)
				break;
			
			// get current block group
			auto blockGroup = block->blockGroup.GetPointer();
			assert(blockGroup);

			// get target block
			auto tarBlock = scene->blocks.TryAt(moveToCRIndex + XYi{ 0, 1 });
			assert(tarBlock);

			// get target block group
			auto tarBlockGroup = tarBlock->blockGroup.GetPointer();
			assert(tarBlockGroup);

			// same block group?
			if (blockGroup == tarBlockGroup) {
				auto tarPosX = CIndexToPosX(moveToCRIndex.x);
				if (Move({ tarPosX, tarPosY })) {
					plan = PlanTypes::Idle;
				}
				break;
			}

			// fill actions
			auto nextBlockGroupIndex = blockGroup->navigationTips[tarBlockGroup->index];
			auto nextBlockGroup = scene->blockGroups[nextBlockGroupIndex].pointer;

			if (blockGroup->rIndex == nextBlockGroup->rIndex) {
				// far jump
				// 1. move to edge ( need calc )  2. jump   3. next step
				// #################
				// 1--- 2---
				// #################
				// 2--- 1---

				xx::CoutN("todo ", __LINE__);
			}
			else if (blockGroup->rIndex < nextBlockGroup->rIndex) {
				// fall
				// 1. move to edge ( need calc )  2. fall   3. next step
				// #################
				//     1---
				//   2-------
				// #################
				// 1---
				//   2---
				// #################
				//   1---
				// 2---

				xx::CoutN("todo ", __LINE__);
			}
			else {
				// jump / climb
				// 1. move to tar edge +-1 ( need calc )  2. jump   3. next step

				if (blockGroup->cIndexRange.from < nextBlockGroup->cIndexRange.from) {
					//       2--...
					// 1...--
					currentActionIndex = 0;
					{
						// move to g1's right edge
						auto& a = (Action_Move&)actions[0];
						a.type = a.cType;
						auto tx = CIndexToPosX(blockGroup->cIndexRange.to);
						a.tarPos = { tx, tarPosY };
					}
					{
						// jump to g2's left edge
						auto& a = (Action_Jump&)actions[1];
						a.type = a.cType;
						auto tx = CIndexToPosX(nextBlockGroup->cIndexRange.from);
						auto ty = RIndexToPosY(nextBlockGroup->rIndex - 1);
						a.tarPos = { tx, ty };
					}
					actions[2].type = ActionTypes::None;
				}
				else {
					// 2...--
					//       1--...
					currentActionIndex = 0;
					{
						// move to g1's left edge
						auto& a = (Action_Move&)actions[0];
						a.type = a.cType;
						auto tx = CIndexToPosX(blockGroup->cIndexRange.from);
						a.tarPos = { tx, tarPosY };
					}
					{
						// jump to g2's right edge
						auto& a = (Action_Jump&)actions[1];
						a.type = a.cType;
						auto tx = CIndexToPosX(nextBlockGroup->cIndexRange.to);
						auto ty = RIndexToPosY(nextBlockGroup->rIndex - 1);
						a.tarPos = { tx, ty };
					}
					actions[2].type = ActionTypes::None;
				}

			}

			break;
		}
		}

		return false;
	}

	XX_INLINE bool Character::Move(XYi const& tarPos) {
		assert(pos.y == _pos.y);
		assert(pos.y == tarPos.y);
		auto tp = tarPos.As<float>();
		auto dx = tp.x - _pos.x;
		if (dx * dx <= cSpeed * cSpeed) {
			pos.x = tarPos.x;
			_pos.x = tp.x;
			return true;
		}
		else {
			_pos.x += cSpeed;
			pos.x = (int32_t)_pos.x;
			return false;
		}
	}

	XX_INLINE bool Character::Jump(XYi const& tarPos) {
		auto tp = tarPos.As<float>();
		if (_pos.y > tp.y) {
			assert(pos.x == _pos.x);
			auto dy = tp.y - _pos.y;
			if (dy * dy <= cSpeed * cSpeed) {
				pos.y = tarPos.y;
				_pos.y = tp.y;
			}
			else {
				_pos.y -= cSpeed;
				pos.y = (int32_t)_pos.y;
			}
			return false;
		}
	//LabMove:
		assert(pos.y == tarPos.y);
		auto dx = tp.x - _pos.x;
		if (dx * dx <= cSpeed * cSpeed) {
			pos.x = tarPos.x;
			_pos.x = tp.x;
			assert(pos.y == _pos.y);
			return true;
		}
		else {
			_pos.x += cSpeed;
			pos.x = (int32_t)_pos.x;
			return false;
		}
	}

	inline void Character::Draw() {
		auto& frame = *gRes.monster_3;
		auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame.tex->GetValue(), 1);
		q.pos = gLooper.camera.ToGLPos(_pos);
		q.anchor = cAnchor;
		q.scale = cScale * gLooper.camera.scale;
		q.radians = cRadians;
		q.colorplus = 1;
		q.color = xx::RGBA8_White;
		q.texRect.data = frame.textureRect.data;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline xx::Shared<Block> Block::Init(Scene* scene_, XYi const& crIndex_) {
		auto p = MapCfg::cellSize * crIndex_;	// calculate top left point pos
		Item::Init(scene_, p);
		size = MapCfg::cellSize;
		color = { 128,64,0,255 };

		crIndex = crIndex_;

		return xx::SharedFromThis(this);
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

	inline int32_t Block::CalcCrossLenX(int32_t cPosX, int32_t cSizeX) const {
		auto xLeft = std::min(pos.x, cPosX);
		auto xRight = std::max(pos.x + size.x, cPosX + cSizeX);
		return size.x + cSizeX - (xRight - xLeft);
	}

	inline void Block::Draw() {
		auto frame = gRes.quad;
		auto p = pos.As<float>();
		auto s = size.As<float>();
		auto q = gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(frame->tex->GetValue(), 2);
		q[0].pos = gLooper.camera.ToGLPos(p);
		q[0].anchor = cAnchor;
		q[0].scale = s / cResSize * gLooper.camera.scale;
		q[0].radians = cRadians;
		q[0].colorplus = 1;
		q[0].color = color;
		q[0].texRect.data = frame->textureRect.data;

		q[1].pos = gLooper.camera.ToGLPos(p + 2.f);
		q[1].anchor = cAnchor;
		q[1].scale = s / cResSize * gLooper.camera.scale * ((s - 4.f) / s);
		q[1].radians = cRadians;
		q[1].colorplus = 1;
		q[1].color = xx::RGBA8_Black;
		q[1].texRect.data = frame->textureRect.data;
	}

	inline void Block::FillWayout() {
		// search neighbor & set wayout bit
		auto& bs = scene->blocks;
		auto cri = bs.PosToColRowIndex(pos);
		bool atEdge{};

		if (cri.y == 0) {
			wayout.up = false; atEdge = true;
		}
		else wayout.up = !bs.TryAt({ cri.x, cri.y - 1 });

		if (cri.y + 1 == bs.numRows) {
			wayout.down = false; atEdge = true;
		}
		else wayout.down = !bs.TryAt({ cri.x, cri.y + 1 });

		if (cri.x == 0) {
			wayout.left = false; atEdge = true;
		}
		else wayout.left = !bs.TryAt({ cri.x - 1, cri.y });

		if (cri.x + 1 == bs.numCols) {
			wayout.right = false;
		}
		else wayout.right = !bs.TryAt({ cri.x + 1, cri.y });

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
			}
			else {
				if (wayout.up) dUp = cPosRB.y - pos.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = pos.y - cPos.y + cSize.y;
				else dDown = 0x7FFFFFFF;
			}
		}
		else {
			if (wayout.left) dLeft = cPosRB.x - pos.x;
			else dLeft = 0x7FFFFFFF;
			if (wayout.right) dRight = bPosRB.x - cPos.x;
			else dRight = 0x7FFFFFFF;
			if (cCenter.y >= bCenter.y) {
				if (wayout.up) dUp = cPos.y - pos.y + cSize.y;
				else dUp = 0x7FFFFFFF;
				if (wayout.down) dDown = bPosRB.y - cPos.y;
				else dDown = 0x7FFFFFFF;
			}
			else {
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
		// Ｂ					block
		// ｃ					character
		// ｅ					end pos

		static std::u32string_view mapText{ UR"(
　　　　　　　　　　　　
Ｂ　　　　　　　　　　Ｂ
Ｂ　　　Ｂ　Ｂ　　　　Ｂ
Ｂｃ　ＢＢＢＢ　　　　Ｂ
ＢＢＢＢＢＢＢＢ　　　Ｂ
ＢＢＢＢＢＢＢＢＢＢ　Ｂ
Ｂ　　　　　　　　　　Ｂ
Ｂ　　　　　ＢＢＢＢＢＢ
Ｂ　　　　　　ＢＢＢＢＢ
Ｂ　ｅ　　　　　　　　Ｂ
ＢＢＢＢＢＢＢＢＢＢＢＢ
)" };

		mapText = mapText.substr(1, mapText.size() - 2);	// skip first & last new line

		// detect map max size
		int32_t maxX{}, x{}, y{};
		for (int32_t i = 0; i < mapText.size(); ++i) {
			auto c = mapText[i];
			switch (auto c = mapText[i]) {
			case U'\r': continue;
			case U'\n':
				if (maxX < x) maxX = x;
				x = 0;
				++y;
				continue;
			}
			++x;
		}
		blocks.Init(y + 1, maxX, MapCfg::cellSize);

		XYi beginCRIndex{}, endCRIndex{};

		// fill map contents
		x = 0;
		y = 0;
		for (int32_t i = 0; i < mapText.size(); ++i) {
			switch (auto c = mapText[i]) {
			case U'\r': continue;
			case U'\n':
				x = 0;
				++y;
				continue;
			case U'ｃ':
				beginCRIndex = { x, y };
				break;
			case U'ｅ':
				endCRIndex = { x, y };
				break;
			case U'Ｂ': {
				blocks.Add(xx::MakeShared<Block>()->Init(this, { x, y }));
				break;
			}
			}
			++x;
		}
		for (auto& o : blocks.items) o->FillWayout();

		character.Emplace()->Init(this, beginCRIndex);

		gLooper.camera.SetOriginal({ character->_pos.x, blocks.gridSize.y / 2 });

		// astar begin
		auto width = blocks.numCols;
		auto height = blocks.numRows;

		xx::AStarGrid asg;
		asg.Init(width, height);

		// generate cells
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				asg.InitCell(x, y, !blocks.At({ x, y }));
			}
		}

		// for search falling block
		auto FindFallingOffsetY = [&](int32_t x, int32_t y)->std::optional<int32_t> {
			std::optional<int32_t> r;
			if (x < 0 || y < 0 || x >= width || y >= height || blocks.At({ x, y })) return r;
			for (auto i = y + 1; i < height; ++i) {
				if (blocks.At({ x, i })) {
					r = i - y - 1;
					break;
				}
			}
			return r;
		};

		// fill neighbors		// todo: custom neighbors support
		xx::Listi32<XYi> nos;
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				// keyboard number area style
				// 7 8 9
				// 4 5 6
				// 1 2 3

				// [???]
				// [?.?]
				// [???]
				auto c5 = asg.At(x, y);
				if (c5->walkable) {
					auto c1 = asg.TryAt(x - 1, y + 1)->walkable;
					auto c2 = asg.TryAt(x, y + 1)->walkable;
					auto c3 = asg.TryAt(x + 1, y + 1)->walkable;
					auto c4 = asg.TryAt(x - 1, y)->walkable;
					auto c6 = asg.TryAt(x + 1, y)->walkable;
					auto c7 = asg.TryAt(x - 1, y - 1)->walkable;
					auto c8 = asg.TryAt(x, y - 1)->walkable;
					auto c9 = asg.TryAt(x + 1, y - 1)->walkable;

					// file	neighbors
					{
						xx::CoutN(x, ", ", y, '\n'
							, c7 ? '.' : 'B', c8 ? '.' : 'B', c9 ? '.' : 'B', '\n'
							, c4 ? '.' : 'B', 'c', c6 ? '.' : 'B', '\n'
							, c1 ? '.' : 'B', c2 ? '.' : 'B', c3 ? '.' : 'B'
						);
						if (c2) continue;

						// handle 7 and 9
						if (c8)
						{
							// can to 7
							// [..?]
							// [Bc?]
							// [?B?]
							if (!c4 && c7) {
								nos.Emplace(-1, -1);
							}

							// can to 9
							// [?..]
							// [?cB]
							// [?B?]
							if (!c6 && c9) {
								nos.Emplace(1, -1);
							}
						}

						// handle 4
						// [???]
						// [.c?]
						// [BB?]
						if (c4 && !c1) {
							nos.Emplace(-1, 0);
						}

						// handle 6
						// [???]
						// [?c.]
						// [?BB]
						if (c6 && !c3) {
							nos.Emplace(1, 0);
						}

						// handle falling 1
						// [???]
						// [.c?]
						// [.B?]
						if (c4 && c1) {
							if (auto fy = FindFallingOffsetY(x - 1, y); fy.has_value()) {
								nos.Emplace(-1, *fy);
							}
						}

						// handle falling 3
						// [???]
						// [?c.]
						// [?B.]
						if (c6 && c3) {
							if (auto fy = FindFallingOffsetY(x + 1, y); fy.has_value()) {
								//relations[c5].Emplace(asg.At(x + 1, y + *fy));
								nos.Emplace(1, *fy);
							}
						}
						asg.InitCellNeighbors(c5, nos.buf, nos.len);
						nos.Clear();
					}
				}
			}
		}

		// performance test
		//auto secs = xx::NowEpochSeconds();
		//for (int i = 0; i < 10000000; ++i) {
		//	auto b = asg.Search(beginCRIndex, endCRIndex);
		//	//auto s = asg.Dump(beginCRIndex, endCRIndex);
		//	asg.Cleanup();
		//}
		//xx::CoutN(xx::NowEpochSeconds(secs));

		// show search result
		{
			auto b = asg.Search(beginCRIndex, endCRIndex);
			xx::CoutN(asg.Dump(beginCRIndex, endCRIndex));
			asg.Cleanup();
		}

		// fill block groups
		for (int32_t y = 0; y < height; ++y) {
			for (int32_t x = 0; x < width; ++x) {
				auto by = y + 1;
				if (asg.TryAt(x, y)->walkable && !asg.TryAt(x, by)->walkable) {
					auto block = blocks.TryAt({ x, by });
					if (asg.TryAt(x - 1, y)->walkable && !asg.TryAt(x - 1, by)->walkable) {
						block->blockGroup = blocks.TryAt({ x - 1, by })->blockGroup;
						block->blockGroup->cIndexRange.to = x;
					}
					else {
						auto index = blockGroups.len;
						auto& g = blockGroups.Emplace().Emplace<BlockGroup>();
						block->blockGroup = g;
						g->index = index;
						g->rIndex = by;
						g->cIndexRange.from = g->cIndexRange.to = x;
					}
					block->blockGroup->blocks.Add(xx::WeakFromThis(block));
				}
			}
		}

		// fill navigationTips
		for (auto numGroups = blockGroups.len, i = 0; i < numGroups; ++i) {

			auto beginGroup = blockGroups[i].pointer;
			auto beginBlock = beginGroup->blocks[0].GetPointer();
			auto beginPos = blocks.CellIndexToColRowIndex(beginBlock->indexAtCells) + XY{ 0, -1 };
			beginGroup->navigationTips.Resize(numGroups);

			for (int32_t j = 0; j < numGroups; ++j) {
				if (i == j) continue;
				auto endGroup = blockGroups[j].pointer;
				auto endBlock = endGroup->blocks[0].GetPointer();
				auto endPos = blocks.CellIndexToColRowIndex(endBlock->indexAtCells) + XY{ 0, -1 };
				assert(beginPos != endPos);

				if (asg.Search(beginPos, endPos)) {
					auto& path = asg.path;
					assert(path.len);
					// find first diff group from asg.path & fill to navigationTips
					for (int32_t k = path.len - 1; k >= 0; --k) {
						auto c = path[k];
						auto b = blocks.At({ c->x, c->y + 1 });
						auto g = b->blockGroup.GetPointer();
						if (beginGroup != g) {
							beginGroup->navigationTips[j] = g->index;
							break;
						}
					}
				}
				else {
					beginGroup->navigationTips[j] = -1;	// -1: unreachable
				}
				asg.Cleanup();
			}
		}

		// test navigation
		{
			auto beginBlock = blocks.At(beginCRIndex + XYi{ 0, 1 });
			assert(beginBlock);
			auto beginGroup = beginBlock->blockGroup.TryGetPointer();
			assert(beginGroup);

			auto endBlock = blocks.At(endCRIndex + XYi{ 0, 1 });
			assert(endBlock);
			auto endGroup = endBlock->blockGroup.TryGetPointer();
			assert(endGroup);

			auto gIndex = beginGroup->index;
			xx::Cout(gIndex);
			while (gIndex != endGroup->index) {
				gIndex = blockGroups[gIndex]->navigationTips[endGroup->index];
				xx::Cout(" -- ", gIndex);
			}
			xx::CoutN();
		}

		// set character todo
		{
			character->plan = PlanTypes::MoveTo;
			character->moveToCRIndex = endCRIndex;
		}
	}

	inline void Scene::Update() {
		character->Update();
	}

	inline void Scene::Draw() {
		for (auto& o : blocks.items) o->Draw();
		character->Draw();
		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(ai) mouse click for move char", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
