#pragma once

namespace AI {

	struct AStarCell {
		int32_t x{}, y{}, walkable{};
		AStarCell* parent{};

		// memory packed to 4 * 4 = 16 bytes
		float heuristicStartToEndLen{}, startToCurLen{}, heuristicCurToEndLen{};
		char heuristicCurToEndLen_hasValue{}, opened{}, closed{};
	};

	struct AStarGrid {
		int32_t width{}, height{};							// map size
		AStarCell* startCell{}, * endCell{};				// tmp
		xx::Listi32<AStarCell> cells;						// map data
		xx::Listi32<AStarCell*> openList;					// tmp
		xx::Listi32<AStarCell*> path;						// search result

		static constexpr float sqrt_2 = 1.414213562373095;
		static constexpr std::array<XYi, 8> neighborOffsets = { XYi
			{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}
		};

		XX_INLINE AStarCell& At(int32_t x, int32_t y) {
			assert(x >= 0 && y >= 0 && x < width && y < height);
			return cells[(size_t)y * width + x];
		}

		XX_INLINE float Heuristic(int32_t x, int32_t y) {
			return sqrtf(float((x - endCell->x) * (x - endCell->x) + (y - endCell->y) * (y - endCell->y)));
		}

		XX_INLINE void OpenListAdd(AStarCell* c) {
			c->opened = 1;
			openList.Emplace(c);
			std::push_heap(openList.buf, openList.buf + openList.len, [](auto a, auto b) {
				return a->heuristicStartToEndLen > b->heuristicStartToEndLen;
			});
		}

		XX_INLINE AStarCell* OpenListPop() {
			auto first = openList[0];
			first->opened = 0;
			first->closed = 1;
			std::pop_heap(openList.buf, openList.buf + openList.len, [](auto a, auto b) {
				return a->heuristicStartToEndLen > b->heuristicStartToEndLen;
			});
			openList.PopBack();
			return first;
		}

		bool FindPath() {
			assert(width && height);
			assert(!startCell && !endCell);
			OpenListAdd(startCell);
			while (!openList.Empty()) {
				auto c = OpenListPop();
				if (c == endCell) {
					path.Clear();
					do {
						path.Emplace(c);
					} while ((c = c->parent));
					return true;
				}
				auto cx = c->x;
				auto cy = c->y;
				for (auto&& o : neighborOffsets) {
					auto&& n = At(o.x + cx, o.y + cy);
					if (!n.walkable || n.closed) continue;
					auto len = c->startToCurLen + ((n.x == cx || n.y == cy) ? 1.0f : sqrt_2);
					if (!n.opened || len < n.startToCurLen) {
						n.startToCurLen = len;
						if (!n.heuristicCurToEndLen_hasValue) {
							n.heuristicCurToEndLen = Heuristic(n.x, n.y);
							n.heuristicCurToEndLen_hasValue = 1;
						}
						n.heuristicStartToEndLen = n.startToCurLen + n.heuristicCurToEndLen;
						n.parent = c;
						if (!n.opened) {
							OpenListAdd(&n);
						}
					}
				}
			}
			return false;
		}

		XX_INLINE void Cleanup() {
			openList.Clear();
			for (auto&& c : cells) {
				memset(&c.heuristicStartToEndLen, 0, 4 * 4);	// heuristicStartToEndLen ~ closed
			}
		}

		//void LoadByFile(char const* fileName) {
		//	std::ifstream f(fileName);
		//	std::string tmp;
		//	std::vector<std::string> ss;
		//	while (getline(f, tmp)) {
		//		if (tmp[tmp.size() - 1] < 36) {
		//			tmp.resize(tmp.size() - 1);
		//		}
		//		ss.push_back(tmp);
		//	}
		//	width = (int32_t)ss[0].size();
		//	height = (int32_t)ss.size();
		//	cells.Resize((size_t)width * height);
		//	path.Clear();
		//	startCell = nullptr;
		//	endCell = nullptr;
		//	for (int32_t y = 0; y < height; ++y) {
		//		auto& s = ss[y];
		//		for (int32_t x = 0; x < width; ++x) {
		//			switch (s[x]) {
		//			case '@':
		//				startCell = &At(x, y);
		//				At(x, y).walkable = 1;
		//				break;
		//			case '*':
		//				endCell = &At(x, y);
		//			case ' ':
		//				At(x, y).walkable = 1;
		//				break;
		//			default:
		//				At(x, y).walkable = 0;
		//			}
		//		}
		//	}
		//	for (int32_t y = 0; y < height; ++y) {
		//		for (int32_t x = 0; x < width; ++x) {
		//			At(x, y).x = x;
		//			At(x, y).y = y;
		//		}
		//	}
		//}

		//void Dump() {
		//	for (int32_t y = 0; y < height; ++y) {
		//		for (int32_t x = 0; x < width; ++x) {
		//			auto o = &At(x, y);
		//			if (o == startCell) {
		//				std::cout << "o";
		//			}
		//			else if (o == endCell) {
		//				std::cout << "*";
		//			}
		//			else if (path.len && std::find(path.buf, path.buf + path.len, o) != nullptr) {
		//				std::cout << "+";
		//			}
		//			else if (o->walkable) {
		//				std::cout << " ";
		//			}
		//			else {
		//				std::cout << "#";
		//			}
		//		}
		//		std::cout << std::endl;
		//	}
		//	std::cout << std::endl;
		//}
	};

	/***************************************************************************************/
	/***************************************************************************************/

	inline void Item::Init(Scene* scene_, XYi const& pos_) {
		scene = scene_;
		pos = pos_;
	}

	/***************************************************************************************/
	/***************************************************************************************/

	inline Character& Character::Init(Scene* scene_, XYi const& crIndex) {

		auto p = MapCfg::cellSize * crIndex;	// calculate top left point pos

		// calculate bottom center point pos
		p.x += MapCfg::cellSize.x >> 1;
		p.y += MapCfg::cellSize.y;

		Item::Init(scene_, p);

		// sync float version
		_pos = p;

		return *this;
	}

	inline bool Character::Update() {
		// todo: AI search path logic
		return false;
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

	inline xx::Shared<Block> Block::Init(Scene* scene_, XYi const& crIndex) {

		auto p = MapCfg::cellSize * crIndex;	// calculate top left point pos

		Item::Init(scene_, p);

		size = MapCfg::cellSize;
		color = { 128,64,0,255 };

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
		static std::u32string_view mapText{ UR"(
　　Ｂ　Ｂ　　ｃ　
　Ｂ　Ｂ　Ｂ　Ｂ　
Ｂ　　　Ｂ　Ｂ　Ｂ
)" };	// last new line is required
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
		blocks.Init(y + 1, maxX, { 64, 64 });

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
				character.Emplace()->Init(this, { x, y });
				break;
			case U'Ｂ': {
				blocks.Add(xx::MakeShared<Block>()->Init(this, { x, y }));
				break;
			}
			}
			++x;
		}

		for (auto& o : blocks.items) o->FillWayout();

		gLooper.camera.SetOriginal({ character->_pos.x, blocks.gridSize.y / 2 });
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
