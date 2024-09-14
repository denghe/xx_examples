#pragma once
#include "pch.h"
#include "res_tp_frames.h"

// this file is generated by tool: plist2blist

xx::Task<> ResTpFrames::AsyncLoad(std::string picRoot) {
	{
		auto& eg = xx::EngineBase3::Instance();
#ifdef __EMSCRIPTEN__
		auto tp = co_await eg.AsyncLoadTexturePackerFromUrl(picRoot + "pics.blist");
#else
		auto tp = eg.LoadTexturePacker<true>(picRoot + "pics.blist");
#endif
		xx_assert(tp);
		auto map = tp->GetMapSV();

		this->blade_light = map["blade_light"sv];
		this->button = map["button"sv];
		this->circle = map["circle"sv];
		this->cring = map["cring"sv];
		this->explosion_13_1 = map["explosion_13_1"sv];
		this->explosion_13_2 = map["explosion_13_2"sv];
		this->explosion_13_3 = map["explosion_13_3"sv];
		this->explosion_13_4 = map["explosion_13_4"sv];
		this->explosion_13_5 = map["explosion_13_5"sv];
		this->explosion_13_6 = map["explosion_13_6"sv];
		this->explosion_13_7 = map["explosion_13_7"sv];
		this->eye_fire_1 = map["eye_fire_1"sv];
		this->eye_fire_2 = map["eye_fire_2"sv];
		this->eye_fire_3 = map["eye_fire_3"sv];
		this->eye_fire_4 = map["eye_fire_4"sv];
		this->eye_fire_5 = map["eye_fire_5"sv];
		this->eye_fire_6 = map["eye_fire_6"sv];
		this->eye_fire_7 = map["eye_fire_7"sv];
		this->font_outline_32 = map["font_outline_32"sv];
		this->font_outline_33 = map["font_outline_33"sv];
		this->font_outline_34 = map["font_outline_34"sv];
		this->font_outline_35 = map["font_outline_35"sv];
		this->font_outline_36 = map["font_outline_36"sv];
		this->font_outline_37 = map["font_outline_37"sv];
		this->font_outline_38 = map["font_outline_38"sv];
		this->font_outline_39 = map["font_outline_39"sv];
		this->font_outline_40 = map["font_outline_40"sv];
		this->font_outline_41 = map["font_outline_41"sv];
		this->font_outline_42 = map["font_outline_42"sv];
		this->font_outline_43 = map["font_outline_43"sv];
		this->font_outline_44 = map["font_outline_44"sv];
		this->font_outline_45 = map["font_outline_45"sv];
		this->font_outline_46 = map["font_outline_46"sv];
		this->font_outline_47 = map["font_outline_47"sv];
		this->font_outline_48 = map["font_outline_48"sv];
		this->font_outline_49 = map["font_outline_49"sv];
		this->font_outline_50 = map["font_outline_50"sv];
		this->font_outline_51 = map["font_outline_51"sv];
		this->font_outline_52 = map["font_outline_52"sv];
		this->font_outline_53 = map["font_outline_53"sv];
		this->font_outline_54 = map["font_outline_54"sv];
		this->font_outline_55 = map["font_outline_55"sv];
		this->font_outline_56 = map["font_outline_56"sv];
		this->font_outline_57 = map["font_outline_57"sv];
		this->font_outline_58 = map["font_outline_58"sv];
		this->font_outline_59 = map["font_outline_59"sv];
		this->font_outline_60 = map["font_outline_60"sv];
		this->font_outline_61 = map["font_outline_61"sv];
		this->font_outline_62 = map["font_outline_62"sv];
		this->font_outline_63 = map["font_outline_63"sv];
		this->font_outline_64 = map["font_outline_64"sv];
		this->font_outline_65 = map["font_outline_65"sv];
		this->font_outline_66 = map["font_outline_66"sv];
		this->font_outline_67 = map["font_outline_67"sv];
		this->font_outline_68 = map["font_outline_68"sv];
		this->font_outline_69 = map["font_outline_69"sv];
		this->font_outline_70 = map["font_outline_70"sv];
		this->font_outline_71 = map["font_outline_71"sv];
		this->font_outline_72 = map["font_outline_72"sv];
		this->font_outline_73 = map["font_outline_73"sv];
		this->font_outline_74 = map["font_outline_74"sv];
		this->font_outline_75 = map["font_outline_75"sv];
		this->font_outline_76 = map["font_outline_76"sv];
		this->font_outline_77 = map["font_outline_77"sv];
		this->font_outline_78 = map["font_outline_78"sv];
		this->font_outline_79 = map["font_outline_79"sv];
		this->font_outline_80 = map["font_outline_80"sv];
		this->font_outline_81 = map["font_outline_81"sv];
		this->font_outline_82 = map["font_outline_82"sv];
		this->font_outline_83 = map["font_outline_83"sv];
		this->font_outline_84 = map["font_outline_84"sv];
		this->font_outline_85 = map["font_outline_85"sv];
		this->font_outline_86 = map["font_outline_86"sv];
		this->font_outline_87 = map["font_outline_87"sv];
		this->font_outline_88 = map["font_outline_88"sv];
		this->font_outline_89 = map["font_outline_89"sv];
		this->font_outline_90 = map["font_outline_90"sv];
		this->font_outline_91 = map["font_outline_91"sv];
		this->font_outline_92 = map["font_outline_92"sv];
		this->font_outline_93 = map["font_outline_93"sv];
		this->font_outline_94 = map["font_outline_94"sv];
		this->font_outline_95 = map["font_outline_95"sv];
		this->font_outline_96 = map["font_outline_96"sv];
		this->font_outline_97 = map["font_outline_97"sv];
		this->font_outline_98 = map["font_outline_98"sv];
		this->font_outline_99 = map["font_outline_99"sv];
		this->font_outline_100 = map["font_outline_100"sv];
		this->font_outline_101 = map["font_outline_101"sv];
		this->font_outline_102 = map["font_outline_102"sv];
		this->font_outline_103 = map["font_outline_103"sv];
		this->font_outline_104 = map["font_outline_104"sv];
		this->font_outline_105 = map["font_outline_105"sv];
		this->font_outline_106 = map["font_outline_106"sv];
		this->font_outline_107 = map["font_outline_107"sv];
		this->font_outline_108 = map["font_outline_108"sv];
		this->font_outline_109 = map["font_outline_109"sv];
		this->font_outline_110 = map["font_outline_110"sv];
		this->font_outline_111 = map["font_outline_111"sv];
		this->font_outline_112 = map["font_outline_112"sv];
		this->font_outline_113 = map["font_outline_113"sv];
		this->font_outline_114 = map["font_outline_114"sv];
		this->font_outline_115 = map["font_outline_115"sv];
		this->font_outline_116 = map["font_outline_116"sv];
		this->font_outline_117 = map["font_outline_117"sv];
		this->font_outline_118 = map["font_outline_118"sv];
		this->font_outline_119 = map["font_outline_119"sv];
		this->font_outline_120 = map["font_outline_120"sv];
		this->font_outline_121 = map["font_outline_121"sv];
		this->font_outline_122 = map["font_outline_122"sv];
		this->font_outline_123 = map["font_outline_123"sv];
		this->font_outline_124 = map["font_outline_124"sv];
		this->font_outline_125 = map["font_outline_125"sv];
		this->font_outline_126 = map["font_outline_126"sv];
		this->monster_1 = map["monster_1"sv];
		this->monster_2 = map["monster_2"sv];
		this->monster_3 = map["monster_3"sv];
		this->monster_4 = map["monster_4"sv];
		this->quad = map["quad"sv];
		this->ring = map["ring"sv];
		this->staff_5 = map["staff_5"sv];
		this->sword_16 = map["sword_16"sv];
	}

	// fill groups

	explosion_13_.Add(explosion_13_1);
	explosion_13_.Add(explosion_13_2);
	explosion_13_.Add(explosion_13_3);
	explosion_13_.Add(explosion_13_4);
	explosion_13_.Add(explosion_13_5);
	explosion_13_.Add(explosion_13_6);
	explosion_13_.Add(explosion_13_7);
	eye_fire_.Add(eye_fire_1);
	eye_fire_.Add(eye_fire_2);
	eye_fire_.Add(eye_fire_3);
	eye_fire_.Add(eye_fire_4);
	eye_fire_.Add(eye_fire_5);
	eye_fire_.Add(eye_fire_6);
	eye_fire_.Add(eye_fire_7);
	font_outline_.Add(font_outline_32);
	font_outline_.Add(font_outline_33);
	font_outline_.Add(font_outline_34);
	font_outline_.Add(font_outline_35);
	font_outline_.Add(font_outline_36);
	font_outline_.Add(font_outline_37);
	font_outline_.Add(font_outline_38);
	font_outline_.Add(font_outline_39);
	font_outline_.Add(font_outline_40);
	font_outline_.Add(font_outline_41);
	font_outline_.Add(font_outline_42);
	font_outline_.Add(font_outline_43);
	font_outline_.Add(font_outline_44);
	font_outline_.Add(font_outline_45);
	font_outline_.Add(font_outline_46);
	font_outline_.Add(font_outline_47);
	font_outline_.Add(font_outline_48);
	font_outline_.Add(font_outline_49);
	font_outline_.Add(font_outline_50);
	font_outline_.Add(font_outline_51);
	font_outline_.Add(font_outline_52);
	font_outline_.Add(font_outline_53);
	font_outline_.Add(font_outline_54);
	font_outline_.Add(font_outline_55);
	font_outline_.Add(font_outline_56);
	font_outline_.Add(font_outline_57);
	font_outline_.Add(font_outline_58);
	font_outline_.Add(font_outline_59);
	font_outline_.Add(font_outline_60);
	font_outline_.Add(font_outline_61);
	font_outline_.Add(font_outline_62);
	font_outline_.Add(font_outline_63);
	font_outline_.Add(font_outline_64);
	font_outline_.Add(font_outline_65);
	font_outline_.Add(font_outline_66);
	font_outline_.Add(font_outline_67);
	font_outline_.Add(font_outline_68);
	font_outline_.Add(font_outline_69);
	font_outline_.Add(font_outline_70);
	font_outline_.Add(font_outline_71);
	font_outline_.Add(font_outline_72);
	font_outline_.Add(font_outline_73);
	font_outline_.Add(font_outline_74);
	font_outline_.Add(font_outline_75);
	font_outline_.Add(font_outline_76);
	font_outline_.Add(font_outline_77);
	font_outline_.Add(font_outline_78);
	font_outline_.Add(font_outline_79);
	font_outline_.Add(font_outline_80);
	font_outline_.Add(font_outline_81);
	font_outline_.Add(font_outline_82);
	font_outline_.Add(font_outline_83);
	font_outline_.Add(font_outline_84);
	font_outline_.Add(font_outline_85);
	font_outline_.Add(font_outline_86);
	font_outline_.Add(font_outline_87);
	font_outline_.Add(font_outline_88);
	font_outline_.Add(font_outline_89);
	font_outline_.Add(font_outline_90);
	font_outline_.Add(font_outline_91);
	font_outline_.Add(font_outline_92);
	font_outline_.Add(font_outline_93);
	font_outline_.Add(font_outline_94);
	font_outline_.Add(font_outline_95);
	font_outline_.Add(font_outline_96);
	font_outline_.Add(font_outline_97);
	font_outline_.Add(font_outline_98);
	font_outline_.Add(font_outline_99);
	font_outline_.Add(font_outline_100);
	font_outline_.Add(font_outline_101);
	font_outline_.Add(font_outline_102);
	font_outline_.Add(font_outline_103);
	font_outline_.Add(font_outline_104);
	font_outline_.Add(font_outline_105);
	font_outline_.Add(font_outline_106);
	font_outline_.Add(font_outline_107);
	font_outline_.Add(font_outline_108);
	font_outline_.Add(font_outline_109);
	font_outline_.Add(font_outline_110);
	font_outline_.Add(font_outline_111);
	font_outline_.Add(font_outline_112);
	font_outline_.Add(font_outline_113);
	font_outline_.Add(font_outline_114);
	font_outline_.Add(font_outline_115);
	font_outline_.Add(font_outline_116);
	font_outline_.Add(font_outline_117);
	font_outline_.Add(font_outline_118);
	font_outline_.Add(font_outline_119);
	font_outline_.Add(font_outline_120);
	font_outline_.Add(font_outline_121);
	font_outline_.Add(font_outline_122);
	font_outline_.Add(font_outline_123);
	font_outline_.Add(font_outline_124);
	font_outline_.Add(font_outline_125);
	font_outline_.Add(font_outline_126);
	monster_.Add(monster_1);
	monster_.Add(monster_2);
	monster_.Add(monster_3);
	monster_.Add(monster_4);
	staff_.Add(staff_5);
	sword_.Add(sword_16);
	co_return;
}
