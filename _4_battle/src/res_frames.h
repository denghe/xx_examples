#include "pch.h"

struct ResFrames {
	xx::Task<> AsyncLoad(std::vector<std::pair<std::string, xx::Ref<xx::Frame>*>>& ffs, std::string picRoot, int32_t texSiz = 2048);
	xx::Task<> AsyncLoad(std::string picRoot, int32_t texSiz = 2048);

	xx::Ref<xx::Frame> button;
};
