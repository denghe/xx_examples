#pragma once

namespace Light {

	inline void Scene::Init() {
		tex_tiles = gLooper.LoadTexture("pics/tiles.png");
		static constexpr auto s = tileSize;
		rect_tiles_1x1[0] = { s * 0, s * 0, s * 1, s * 1 };
		rect_tiles_1x1[0] = { s * 1, s * 0, s * 1, s * 1 };
		rect_tiles_1x1[0] = { s * 0, s * 1, s * 1, s * 1 };
		rect_tiles_1x1[0] = { s * 1, s * 1, s * 1, s * 1 };
		rect_tiles_1x1[0] = { s * 0, s * 3, s * 1, s * 1 };
		rect_tiles_1x1[0] = { s * 1, s * 3, s * 1, s * 1 };

		rect_tiles_1x2[0] = { s * 2, s * 0, s * 1, s * 2 };
		rect_tiles_1x2[1] = { s * 3, s * 0, s * 1, s * 2 };

		rect_tiles_2x1[0] = { s * 0, s * 2, s * 2, s * 1 };
		rect_tiles_2x1[1] = { s * 2, s * 2, s * 2, s * 1 };
		rect_tiles_2x1[2] = { s * 0, s * 4, s * 2, s * 1 };

		rect_tiles_2x2[0] = { s * 2, s * 3, s * 2, s * 2 };
		rect_tiles_2x2[1] = { s * 0, s * 5, s * 2, s * 2 };
		rect_tiles_2x2[2] = { s * 2, s * 5, s * 2, s * 2 };

		tex_light = gLooper.LoadTexture("pics/light.png");
		fb.Init();

		shaderLightTex.Init(&gLooper);
	}

	inline void Scene::Update() {
		if (gLooper.mouse.PressedMBLeft() && !gLooper.mouseEventHandler) {
			auto mp = gLooper.camera.ToLogicPos(gLooper.mouse.pos);

			if (lightCreateAvaliableFrameNumber <= gLooper.frameNumber) {
				lightPoss.Emplace(mp);
				lightCreateAvaliableFrameNumber = gLooper.frameNumber + (int)(0.01 / Cfg::frameDelay);
			}
		}
	}

	inline void Scene::DrawTile(int32_t cx, int32_t cy, xx::UVRect const& uv) {
		auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(tex_tiles->GetValue(), 1);
		q.pos = gLooper.camera.ToGLPos({ cx * tileSize, cy * tileSize });
		q.anchor = { 0, 1 };
		q.scale = 1.f * gLooper.camera.scale;
		q.radians = 0.f;
		q.colorplus = 1;
		q.color = xx::RGBA8_White;
		q.texRect.data = uv.data;
	}

	inline void Scene::Draw() {

		auto t1 = fb.Draw({ Cfg::width, Cfg::height }, true, xx::RGBA8{ 0,0,0,0 }, [&]() {
			DrawTile(-2, -2, rect_tiles_2x2[1]);
			DrawTile(0, -2, rect_tiles_2x2[2]);
			DrawTile(-2, 0, rect_tiles_2x2[0]);
			DrawTile(0, 0, rect_tiles_1x2[0]);
			DrawTile(1, 0, rect_tiles_1x2[1]);
		});

		auto t2 = fb.Draw({ Cfg::width, Cfg::height }, true, xx::RGBA8{0,0,0,0}, [&]() {
			gLooper.GLBlendFunc({ GL_SRC_ALPHA, GL_DST_ALPHA, GL_FUNC_ADD });
			for (auto& p : lightPoss) {
				auto& q = *gLooper.ShaderBegin(gLooper.shaderQuadInstance).Draw(tex_light->GetValue(), 1);
				q.pos = gLooper.camera.ToGLPos(p);
				q.anchor = { 0.5, 0.5 };
				q.scale = 1.f * gLooper.camera.scale;
				q.radians = 0.f;
				q.colorplus = 1;
				q.color = { 1,1,1,255 };
				q.texRect.x = 0;
				q.texRect.y = 0;
				q.texRect.w = tex_light->Width();
				q.texRect.h = tex_light->Height();
			}
		});

		{
			auto& q = *gLooper.ShaderBegin(shaderLightTex).Draw(t1->GetValue(), t2->GetValue(), 1);
			q.pos = {};
			q.anchor = { 0.5, 0.5 };
			q.scale = 1.f * gLooper.camera.scale;
			q.radians = 0.f;
			q.colorplus = 10;
			q.color = xx::RGBA8_White;
			q.texRect.x = 0;
			q.texRect.y = 0;
			q.texRect.w = t1->Width();
			q.texRect.h = t1->Height();
		}

		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(light) mouse left button for create light", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
