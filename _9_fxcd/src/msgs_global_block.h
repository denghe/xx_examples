#pragma once

namespace Msgs {
	namespace Global {	// id = 1 ~ 99

		struct Block : xx::SerdeBase, xx::SpaceABi32Item<Block> {
			static constexpr uint16_t cTypeId{ 4 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Scene> scene;
			XYi pos{}, halfSize{};
			xx::Math::BlockWayout wayout;
			bool isMapCorner{};

			virtual ~Block();
			Block& Init(Scene* scene_, int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);
			Block& Init(Scene* scene_, XYi const& pos_, XYi const& siz_);

			void FillWayout();
			void AuthWayout(xx::Math::BlockWayout bw);

			bool IntersectCircle(XYi const& p, int32_t radius);
			bool PushOut(Monster* m, XYi& mp);

			void Draw();
		};

	}
}
