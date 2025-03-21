#pragma once
#include "xx_map.h"

namespace Map {

	struct Scene : xx::SceneBase {
		xx::Map map;
		XYi beginPos{}, endPos{};

		static constexpr float cAnimFPS{ 60.f };
		static constexpr int32_t cNumAnimFrames{ 60 };
		static constexpr float cAnimFrameDelay{ 1.f * cAnimFPS / Cfg::fps };

		XY frameLogicPos{ 0, 400 }, frameScale{ 4, 4 }, frameAnchor{ 0.5, 0 };
		XYi frameSize{};
		xx::Listi32<xx::Ref<xx::Frame>> frames;
		xx::Listi32<std::unique_ptr<xx::RGBA8[]>> frameColorss;
		xx::Data frameInfo;					// store bin
		uint8_t const* frameColorsData{};	// point to first address
		int32_t frameColorDataLength{};		// for address calculate
		float frameIndex{};

		static constexpr float cAimScale{ 0.25f };
		xx::Ref<xx::Frame> frame_aimLine;
		xx::Ref<xx::Frame> frame_aimArc;


		void Init() override;
		void Update() override;
		void Draw() override;
	};

}
