#pragma once
#include "xx_map.h"

namespace Map {

	struct Scene : xx::SceneBase {
		xx::Map map;
		XYi beginPos{}, endPos{};

		static constexpr float cAnimFPS{ 60.f };
		static constexpr int32_t cNumAnimFrames{ 60 };
		static constexpr float cAnimFrameDelay{ 1.f * cAnimFPS / Cfg::fps };

		XY framePos{}, frameScale{ 1,1 }, frameAnchor{ 0.5, 0 };
		XYi frameSize{};
		xx::Listi32<xx::Ref<xx::Frame>> frames;
		xx::Listi32<std::unique_ptr<xx::RGBA8[]>> frameColorss;
		float frameIndex{};

		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
