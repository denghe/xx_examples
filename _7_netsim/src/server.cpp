#include "pch.h"
#include "looper.h"
#include "server.h"

void Server::Init() {
	gIsServer = true;
	scene.Emplace()->Init();
}

void Server::Update() {
	gIsServer = true;
	// todo: peer update use Task ?
	// todo: disconnect auto remove player ?

	for(auto i = peers.len - 1; i >= 0; --i) {
		auto& peer = peers[i];
		auto& recvs = peer->recvs;
		xx::DataShared ds;
		while (recvs.TryPop(ds)) {
			auto typeId = xx::ReadTypeId(ds);
			if (!typeId) {
				Log_Msg_Receive_Bad_Data();
				peers.SwapRemoveAt(i);
				break;
			}
			switch (typeId) {
			case Msgs::C2S::Join::cTypeId: {
				xx::Shared<Msgs::C2S::Join> cmd;
				{
					// deserialize
					auto dr = Msgs::gSerdeInfo.MakeDataEx_r(ds);
					if (auto r = dr.Read(cmd)) {
						Log_Msg_Read_Error();
						peers.SwapRemoveAt(i);
						break;
					}
				}
				// make context
				auto player = xx::MakeShared<Msgs::Global::Player>();
				player->Init(scene, ++autoClientId);
				scene->players.Emplace(player);

				auto rtv = xx::MakeShared<Msgs::S2C::Join_r>();
				rtv->clientId = player->clientId;
				rtv->scene = scene;
				{
					// serialize & send
					auto d = Msgs::gSerdeInfo.MakeDataEx();
					d.Write(rtv);
					peer->Send(xx::DataShared(std::move(d)));
				}
				break;
			}
			default:
				Log_Msg_Receive_Unknown();
			}
		}
	}

	scene->Update();
}
