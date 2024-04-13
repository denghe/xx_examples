#include "pch.h"
#include "looper.h"

Looper gLooper;
ResTpFrames& gRes(gLooper.res);

#ifdef __EMSCRIPTEN__
int32_t main() {
	gLooper.EngineInit();
	emscripten_request_animation_frame_loop([](double ms, void*)->EM_BOOL {
		return gLooper.JsLoopCallback(ms);
		}, nullptr);
}
#else
int32_t main() {
	gLooper.showFps = true;
	gLooper.title = "xx_examples_2_plane";
	gLooper.Init();
	gLooper.Run<false>();
}
#endif


xx::Task<> Looper::MainTask() {

	// res init
	co_await res.AsyncLoad("res/");

	// tiled map init
#ifdef __EMSCRIPTEN__
	tiledMap = co_await AsyncLoadTiledMapFromUrl<true>("res/tm.bmx");
#else
	tiledMap = LoadTiledMap<true>("res/tm.bmx");
#endif

	// locate layer
	xx_assert(tiledMap);
	layerBG = tiledMap->FindLayer<xx::TMX::Layer_Tile>("bg");
	xx_assert(layerBG);
	layerTrees = tiledMap->FindLayer<xx::TMX::Layer_Tile>("trees");
	xx_assert(layerTrees);

	// get map info
	auto mw = tiledMap->width;
	auto mh = tiledMap->height;
	auto tw = tiledMap->tileWidth;
	auto th = tiledMap->tileHeight;
	xx_assert(tw == th && tw == unitSizei);


	// fill fields
	mapNumRowsCols = { mh, mw };
	mapSize = { mw * tw, mh * th };
	mapSize_2 = mapSize / 2;

	trees.Init(mh, mw, { tw, th });

	// fill map frame, grid trees
	for (int y = 0, ye = mh; y < ye; ++y) {
		for (int x = 0, xe = mw; x < xe; ++x) {
			if (auto info = tiledMap->GetGidInfo(layerBG, y, x)) {
				assert(info->image);
				if (info->image->source == "pics/tile_1.png"sv) {
					info->frame = res.tile_1;
				} else if (info->image->source == "pics/tile_2.png"sv) {
					info->frame = res.tile_2;
				}
			}
			if (auto info = tiledMap->GetGidInfo(layerTrees, y, x)) {
				trees.EmplaceInit(xx::XY{ x * tw, y * th });
			}
		}
	}

	monsters.Init(mh, mw, tw);
	srdd.Init(128, unitSizei);

	plane.Emplace()->Init(mapSize_2);

	// camera init
	camera.SetMaxFrameSize(maxItemSize);
	camera.SetOriginal(mapSize_2);
	camera.SetScale(0.1f);

	ok = true;
}


void Looper::BeforeUpdate() {
	if (!ok) return;

	// scale control
	if (gLooper.KeyDownDelay(xx::KeyboardKeys::Z, 0.02f)) {
		camera.IncreaseScale(0.1f, 5);
	} else if (gLooper.KeyDownDelay(xx::KeyboardKeys::X, 0.02f)) {
		camera.DecreaseScale(0.1f, 0.1f);
	}

}


void Looper::Update() {
	if (!ok) return;

	if (plane) {
		if (plane->Update()) {
			plane->pos = mapSize_2;	// todo: reborn
		}
		camera.SetOriginal(plane->pos);
		camera.Calc();
	}

	monsters.Foreach([](Monster& o)->xx::ForeachResult {
		return o.Update() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
	});

	bullets.ForeachFlags([](Bullet& o)->xx::ForeachResult {
		return o.Update() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
	});

	explosions.ForeachFlags([](Explosion& o)->xx::ForeachResult {
		return o.Update() ? xx::ForeachResult::RemoveAndContinue : xx::ForeachResult::Continue;
	});

	enm.Update();

}


void Looper::Draw() {
	if (!ok) return;

	{
		int32_t rowFrom, rowTo, colFrom, colTo;
		camera.FillRowColIdxRange(mapNumRowsCols.y, mapNumRowsCols.x, unitSizei,
			rowFrom, rowTo, colFrom, colTo);

		for (int32_t rowIdx = rowFrom; rowIdx < rowTo; ++rowIdx) {
			for (int32_t colIdx = colFrom; colIdx < colTo; ++colIdx) {
				xx::XY pos{ colIdx * unitSize, rowIdx * unitSize };
				if (auto info = tiledMap->GetGidInfo(layerBG, rowIdx, colIdx)) {
					auto& q = xx::Quad::DrawOnce(info->frame);
					q.pos = camera.ToGLPos(pos);
					q.anchor = { 0, 1 };
					q.scale = camera.scale;
					q.radians = 0;
					q.colorplus = 1;
					q.color = { 255, 255, 255, 255 };
				}
				
				if (auto info = tiledMap->GetGidInfo(layerTrees, rowIdx, colIdx)) {
					trees.ForeachCell({ colIdx, rowIdx }, [this](Tree& tree) {
						yDraws.Emplace(tree.pos.y, [](void* self) { ((Tree*)self)->Draw(); }, &tree);
						});
				}
			}
		}
	}

	// todo: draw all order by y

	bullets.ForeachFlags([this](Bullet& o) {
		yDraws.Emplace(o.pos.y, [](void* self) { ((Bullet*)self)->Draw(); }, &o);
	});

	if (plane) {
		yDraws.Emplace(plane->pos.y, [](void* self) { ((Plane*)self)->Draw(); }, plane.pointer);
	}

	std::sort(yDraws.buf, yDraws.buf + yDraws.len, [](auto const& a, auto const& b) {
		return std::get<0>(a) < std::get<0>(b);
	});

	for (auto& yd : yDraws) {
		std::get<1>(yd)(std::get<2>(yd));
	}

	yDraws.Clear();


	enm.Draw();

	// draw tips
	gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "zoom: ZX  move: ASDW  fire: MOUSE", xx::RGBA8_Green, { 0.5f, 1 });

	//auto str = xx::ToString("total item count = ", grid.Count());
	//gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 50 }, str, xx::RGBA8_Green, { 0.5f, 1 });
}

Monster* Looper::FindNeighborMonster(xx::XY const& pos, float radius) {
	Monster* r{};
	monsters.Foreach9(pos, [&](Monster& m)->bool {
		if (xx::Calc::Intersects::CircleCircle<float>(pos.x, pos.y, radius, m.pos.x, m.pos.y, m.radius)) {
			r = &m;
			return true;
		}
		return false;
	});
	return r;
}
