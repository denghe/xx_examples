#pragma once

namespace Msgs {
	namespace Global {	// id = 1 ~ 99

		struct Monster : xx::SerdeBase, xx::Spacei32Item<Monster> {
			static constexpr uint16_t cTypeId{ 6 }; /////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			static constexpr FX64 cResRadius{ 32 };
			static constexpr FX64 cFrameIndexStep{ FX64{0.1} / Scene::fps60ratio };
			static constexpr FX64 cFrameIndexMax{ ResTpFrames::_countof_monster_ };
			static constexpr FX64 cMovementSpeed{ FX64{5} / Scene::fps60ratio };
			static constexpr FX64 c1_MovementSpeed{ FX64_1 / cMovementSpeed };
			static constexpr FX64 cMovementSpeedPow2{ cMovementSpeed * cMovementSpeed };

			static constexpr float cColorPlusDefault{ 1 };
			static constexpr float cColorPlusWhite{ 100000 };
			static constexpr int32_t cColorPlusChangeDuration{ int32_t(0.1 * Looper::fps) };

			static constexpr int32_t cMaxHP{ 100 };

			xx::Weak<Scene> scene;
			xx::Weak<Player> owner;

			XYp pos, tarPos;
			FX64 radius{}, radians{}, frameIndex{};
			int64_t changeColorToWhiteElapsedTime{};
			/* T */ XYp inc{}, newPos{};

			xx::Shared<Config_Monster> cfg;
			xx::TinyList<xx::Shared<Item>> items;
			Stat_t level{}, experience{};
			Stat_t life{}, energy{};
			StatPanel sp;
			void spSetDirty();
			void spClearDirty();
			bool spIsDirty() const;

			virtual ~Monster();
			Monster* Init(Scene* scene_, xx::Shared<Player> const& owner_, xx::XYi const& bornPos);
			virtual void Update1();	// concurrent support
			virtual int32_t Update2();	// non zero: kill
			/* C */ void Draw();
			/* C */ void DrawBars();
			bool FillCrossInc(XYp const& pos_);
			int32_t BlocksLimit(XYp& pos_);
			void Kill(/* todo: killer */);	// remove from scene.monsters & destroy
			bool Hurt(Bullet_Base* bullet_);
			void UpdateSP();
		};

	}
}
