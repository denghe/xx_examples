#include "pch.h"
#include "looper.h"
#include "server.h"

void Server::Init() {
	gIsServer = true;
	scene.Emplace()->Init();
}

void Server::Update() {
	gIsServer = true;
	
	for(auto i = peers.len - 1; i >= 0; --i) {
		auto& peer = peers[i];
		auto& recvs = peer->recvs;
		xx::DataShared ds;
		if (recvs.TryPop(ds)) {
			auto dr = Msgs::gSerdeInfo.MakeDataEx_r(ds);
			xx::Shared<xx::SerdeBase> ssb;
			if (auto r = dr.Read(ssb)) {
				Log_Msg_Read_Error();
				peers.SwapRemoveAt(i);
				continue;
			}
			switch (ssb->typeId) {
			case Msgs::C2S::Join::cTypeId: {
				auto d = Msgs::gSerdeInfo.MakeDataEx();
				// todo: send Join_r
				d.Write(scene);
				peer->Send(xx::DataShared(std::move(d)));
				break;
			}
			// todo
			default:
				Log_Msg_Receive_Unknown();
			}
		}
	}

	scene->Update();
}
