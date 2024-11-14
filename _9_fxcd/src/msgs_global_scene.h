#pragma once

namespace Msgs {
	namespace Global {	// id = 1 ~ 99

		struct Scene : xx::SerdeBase {
			static constexpr uint16_t cTypeId{ 3 };	/////////////////////////////////////////////////////////////////////////////
			static constexpr uint16_t cParentTypeId{ xx::SerdeBase::cTypeId };
			/* S */ void WriteTo(xx::Data& d) const override;
			/* C */ int32_t ReadFrom(xx::Data_r& dr) override;

			// int max value == 0x7FFF FFFF. sqrt == 46340. / 96 ~= 482		/ 64 = 724
			static constexpr FX64 maxDistance{ 46340 };
			static constexpr int32_t cellSize{ 64 };
			static constexpr int32_t numRows{ (int32_t)Looper::height / cellSize };
			static constexpr int32_t numCols{ (int32_t)Looper::width / cellSize };
			static constexpr int32_t cellSize_2{ cellSize / 2 };
			static constexpr XYi mapSize{ numCols * cellSize, numRows * cellSize };
			static constexpr XYi mapSize_2{ mapSize / 2 };
			static constexpr XY mapSize_2f{ mapSize_2 };
			static constexpr XYp mapSize_2p{ mapSize_2 };
			static constexpr int32_t itemSize{ 64 };
			static constexpr XYi mapEdgeMin{ itemSize * 2 - 1 };
			static constexpr XYi mapEdgeMax{ mapSize - mapEdgeMin };
			static constexpr XYp mapSizep{ mapSize };

			static constexpr FX64 fps{ (int)Looper::fps };
			static constexpr FX64 fps60ratio{ fps / FX64{60} };
			static constexpr FX64 _1_fps60ratio{ FX64{1} / fps60ratio };

			int64_t frameNumber{};
			xx::Rnd rnd;
			xx::Spacei32<Item> itemSpace;
			xx::Spacei32<Monster> monsterSpace;
			xx::Shared<Config_Monster> monsterDefaultConfig;
			xx::Listi32<xx::Shared<Monster>> monsters;
			xx::Listi32<xx::Shared<Player>> players;
			xx::SpaceABi32<Block> blockSpace;
			xx::Listi32<xx::Shared<Block>> blocks;
			xx::Listi32<xx::Shared<Bullet_Base>> bullets;
			xx::Listi32<xx::Shared<Item>> items;
			xx::Queue<EffectText> effectTexts;
			/* T */ bool disposing{};	// ~Scene() == true

			void Init(int32_t sid);
			~Scene();
			/* C */ void InitForDraw();	// recursive call all childs
			void Update();
			/* C */ void Draw();
			xx::Shared<Player> const& RefPlayer(int32_t clientId);
			void RemovePlayer(int32_t clientId);
			void PosLimitByMapSize(XYp& p);
			void MakeEffectText(xx::XY const& pos_, xx::XY const& dist_, xx::RGBA8 color_, int32_t value_);

			template<typename T>
			static xx::Shared<Item> RemoveFromItem(T& items, int32_t index);	// todo
		};

	}
}
