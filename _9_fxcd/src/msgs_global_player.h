#pragma once

namespace Msgs {
	namespace Global {	// id = 1 ~ 99

		struct Player : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 5 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			xx::Weak<Scene> scene;
			int32_t clientId{};
			int32_t score{};

			/* S */ void Init(Scene* scene_, int32_t clientId_);	// auto add to scene.players
			void Update();
			/* C */ void Draw();
		};

	}
}
