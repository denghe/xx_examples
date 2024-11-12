#pragma once

namespace Msgs {

	inline void InitSerdeInfo() {
		gSerdeInfo.Init();

		gSerdeInfo.Register<::Msgs::Global::Equipment>();
		gSerdeInfo.Register<::Msgs::Global::Config_Monster>();
		gSerdeInfo.Register<::Msgs::Global::Scene>();
		gSerdeInfo.Register<::Msgs::Global::Block>();
		gSerdeInfo.Register<::Msgs::Global::Player>();
		gSerdeInfo.Register<::Msgs::Global::Monster>();
		gSerdeInfo.Register<::Msgs::Global::Projectile>();
		gSerdeInfo.Register<::Msgs::Global::Bullet_Base>();
		gSerdeInfo.Register<::Msgs::Global::Bullet_Sector>();
		gSerdeInfo.Register<::Msgs::Global::Bullet_Box>();

		gSerdeInfo.Register<::Msgs::C2S::Join>();
		gSerdeInfo.Register<::Msgs::C2S::Summon>();

		gSerdeInfo.Register<::Msgs::S2C::Join_r>();
		gSerdeInfo.Register<::Msgs::S2C::PlayerJoin>();
		gSerdeInfo.Register<::Msgs::S2C::PlayerLeave>();
		gSerdeInfo.Register<::Msgs::S2C::Summon>();

		// ...
	}
}
