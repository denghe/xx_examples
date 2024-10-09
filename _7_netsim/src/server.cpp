#include "pch.h"
#include "looper.h"
#include "server.h"
#include "client.h"

void Server::Init() {
	gIsServer = true;
	scene.Emplace()->Init();
}

void Server::Update() {
	gIsServer = true;

	for(auto i = peers.len - 1; i >= 0; --i) {
		auto& peer = peers[i];
		if (peer->task()) {
			peers.SwapRemoveAt(i);
		}
	}

	scene->Update();
}

xx::Weak<Peer> Server::Accept(xx::Shared<Client> const& client_) {
	auto& p = peers.Emplace();
	p.Emplace()->Init(this, client_.ToWeak());
	return p.ToWeak();
}

xx::Task<> Peer::Task() {

LabWaitJoin:
	co_yield 0;

	if (!Alive()) {
		Log_Msg_Wait_Join_Disconnected();
		co_return;
	}

	if (xx::DataShared ds; recvs.TryPop(ds)) {
		if (xx::ReadTypeId(ds) != Msgs::C2S::Join::cTypeId) {
			Log_Msg_Wait_Join_Receive_Unknown();
			co_return;
		}
		if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::C2S::Join>(ds); !msg) {
			Log_Msg_Read_Join_Error();
			co_return;
		} else {
			// handle
			clientId = ++server->autoClientId;
			auto player = xx::MakeShared<Msgs::Global::Player>();
			player->Init(server->scene, clientId);
			server->scene->players.Emplace(player);

			// make & send result
			auto rtv = xx::MakeShared<Msgs::S2C::Join_r>();
			rtv->clientId = clientId;
			rtv->scene = server->scene;
			Send(Msgs::gSerdeInfo.MakeDataShared(rtv));
		}
	} else {
		// todo: timeout check?
		goto LabWaitJoin;
	}

LabPlay:
	co_yield 0;

	if (!Alive()) {
		Log_Msg_Wait_Commands_Disconnected();
		co_return;
	}
	// handle commands & sync
	{
		xx::DataShared ds;
		while (recvs.TryPop(ds)) {
			auto typeId = xx::ReadTypeId(ds);
			switch (typeId) {
				// todo
			}
		}
	}
	goto LabPlay;
}

void Peer::Init(Server* server_, xx::Weak<Client>&& client_) {
	server = server_;
	client = std::move(client_);
	task = Task();
}

bool Peer::Alive() const {
	return client;
}

bool Peer::Send(xx::DataShared ds) {
	if (!client) return false;
	client->recvs.Emplace(std::move(ds));
	return true;
}
