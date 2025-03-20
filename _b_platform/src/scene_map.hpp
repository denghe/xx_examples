#pragma once

namespace Map {

	inline void Scene::Init() {

		// fill
		map.InitByText(UR"(
ＢＢＢＢＢＢＢＢＢＢＢＢ
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
)"
, [&](auto x, auto y) { beginPos = { x, y }; }
, [&](auto x, auto y) { endPos = { x, y }; }
);

		// dump
		{
			auto s = map.Dump();
			s[beginPos.y * (map.size.x + 1) + beginPos.x] = U'ｃ';
			s[endPos.y * (map.size.x + 1) + endPos.x] = U'ｅ';
			xx::CoutN(s);
		}

		// navigation test
		{
			auto beginSpace = map.At(beginPos);
			assert(beginSpace);
			auto endSpace = map.At(endPos);
			assert(endSpace);
			auto gid = beginSpace->groupId;
			xx::Cout(gid);
			while (gid != endSpace->groupId) {
				gid = map.spaceGroups[gid].navTips[endSpace->groupId];
				xx::Cout(" -- ", gid);
			}
			xx::CoutN();
		}

		// load frames & frame colors
		for (int32_t i = 0; i < cNumAnimFrames; ++i) {
			auto fn = xx::ToStringFormat("pics/b_redstar_idle_{0}.png", i);
			auto& f = frames.Emplace(gLooper.LoadFrame(fn));
			auto width = f->tex->Width();
			auto height = f->tex->Height();
			auto bytesLen = width * height * 4;
			fn = xx::ToStringFormat("pics/c_redstar_idle_{0}.png", i);
			auto [fileData, fullPath] = gLooper.LoadFileData(fn);
			auto data = std::make_unique_for_overwrite<xx::RGBA8[]>(bytesLen);
			{
				int w, h, comp;
				if (auto buf = stbi_load_from_memory((stbi_uc*)fileData.buf, (int)fileData.len, &w, &h, &comp, 0)) {
					xx_assert(width == w);
					xx_assert(height == h);
					xx_assert(comp == 4);	// RGBA
					memcpy(data.get(), buf, bytesLen);
					stbi_image_free(buf);
				}
				else {
					xx::CoutN("failed to load texture. fn = ", fullPath);
					xx_assert(false);
				}
			}
			frameColorss.Emplace(std::move(data));

			if (i) {
				xx_assert(width == frameSize.x);
				xx_assert(height == frameSize.y);
			}
			else {
				frameSize.x = width;
				frameSize.y = height;
			}
		}
	}

	inline void Scene::Update() {
		frameIndex += cAnimFrameDelay;
		while (frameIndex >= cNumAnimFrames) {
			frameIndex -= cNumAnimFrames;
		}

		if (gLooper.mouse.PressedMBLeft() && !gLooper.mouseEventHandler) {
			auto mp = gLooper.camera.ToLogicPos(gLooper.mouse.pos);

			// calculate pos in frame area & print color
			auto scaledFrameSize = frameSize * frameScale;
			auto anchorPos = scaledFrameSize * XY{ frameAnchor.x, 1.f - frameAnchor.y };
			xx::FromTo<XY> aabb;
			aabb.from = frameLogicPos - anchorPos;
			aabb.to = aabb.from + scaledFrameSize;
			
			if (mp.x < aabb.from.x || mp.y < aabb.from.y
				|| mp.x >= aabb.to.x || mp.y >= aabb.to.y) {
				// out of range
				xx::CoutN("out of range");
			}
			else {
				auto localPos = ((mp - aabb.from) / frameScale).As<int32_t>();
				assert(localPos.x >= 0 && localPos.x < frameSize.x);
				assert(localPos.y >= 0 && localPos.y < frameSize.y);
				auto frameColors = frameColorss[(int32_t)frameIndex].get();
				auto color = frameColors[localPos.y * frameSize.x + localPos.x];
				xx::CoutN("localPos = ", localPos, " color = ", color);
			}
		}
	}

	inline void Scene::Draw() {
		xx::Quad().SetFrame(frames[(int32_t)frameIndex])
			.SetPosition(gLooper.camera.ToGLPos(frameLogicPos))
			.SetAnchor(frameAnchor)
			.SetScale(frameScale)
			.Draw();

		gLooper.ctcDefault.Draw({ 0, gLooper.windowSize_2.y - 5 }, "(map) show console. mouse left button for hit check", xx::RGBA8_Green, { 0.5f, 1 });
	}

}
