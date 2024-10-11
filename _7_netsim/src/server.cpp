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
			auto clientId = peer->clientId;
			peers.SwapRemoveAt(i);
			scene->RemovePlayer(clientId);
			// make & notice all
			auto rtv = xx::MakeShared<Msgs::S2C::PlayerLeave>();
			rtv->clientId = clientId;
			rtv->frameNumber = scene->frameNumber;
			SendToAll(Msgs::gSerdeInfo.MakeDataShared(rtv));
		}
	}

	scene->Update();
}

xx::Weak<Peer> Server::Accept(xx::Shared<Client> const& client_) {
	auto& p = peers.Emplace();
	p.Emplace()->Init(this, client_.ToWeak());
	return p.ToWeak();
}

void Server::SendToAll(xx::DataShared ds) {
	for (auto e = peers.len, i = 0; i < e; ++i) {
		auto& peer = peers[i];
		if (peer->clientId) {
			peer->Send(ds);
		}
	}
}

xx::Task<> Peer::Task() {

LabWaitJoin:
	co_yield 0;

	if (!Alive()) {
		Log_Msg_Wait_Join_Disconnected();
		co_return;
	}

	if (xx::DataShared ds; recvs.TryPop(ds)) {
		if (auto typeId = xx::ReadTypeId(ds); typeId != Msgs::C2S::Join::cTypeId) {
			Log_Msg_Wait_Join_Receive_Unknown(typeId);
			co_return;
		}
		if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::C2S::Join>(ds); !msg) {
			Log_Msg_ReadError<Msgs::C2S::Join>();
			co_return;
		} else {
			// handle
			clientId = ++server->autoClientId;
			xx::MakeShared<Msgs::Global::Player>()->Init(server->scene, clientId);

			// make & send result
			{
				auto rtv = xx::MakeShared<Msgs::S2C::Join_r>();
				rtv->clientId = clientId;
				rtv->scene = server->scene;
				Send(Msgs::gSerdeInfo.MakeDataShared(rtv));
			}

			// make & notice all
			{
				auto notice = xx::MakeShared<Msgs::S2C::PlayerJoin>();
				notice->clientId = clientId;
				notice->frameNumber = server->scene->frameNumber;
				server->SendToAll(Msgs::gSerdeInfo.MakeDataShared(notice));
			}
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
			case Msgs::C2S::Summon::cTypeId:
			{
				if (auto msg = Msgs::gSerdeInfo.MakeMessage<Msgs::C2S::Summon>(ds); !msg) {
					Log_Msg_ReadError<Msgs::C2S::Summon>();
					co_return;
				} else {

					// safe auth
					if (msg->bornPos.x < 0 || msg->bornPos.x >= Msgs::Global::Scene::mapSize.x
						|| msg->bornPos.y < 0 || msg->bornPos.y >= Msgs::Global::Scene::mapSize.y) {
						Log_Msg_ReadError<Msgs::C2S::Summon>();
						co_return;
					}

					if (auto& player = server->scene->RefPlayer(clientId); !player) {
						Log_Msg_Handle_Summon_Error_Player_Not_Found();
						co_return;
					} else {
						// handle
						xx::MakeShared<Msgs::Global::Monster>()->Init(server->scene, player, msg->bornPos);

						// make & notice all
						{
							auto rtv = xx::MakeShared<Msgs::S2C::Summon>();
							rtv->clientId = clientId;
							rtv->frameNumber = server->scene->frameNumber;
							rtv->bornPos = msg->bornPos;
							server->SendToAll(Msgs::gSerdeInfo.MakeDataShared(rtv));
						}
					}
				}
				break;
			}
			// ...
			default:
				Log_Msg_Wait_Commands_Receive_Unknown(typeId);
				co_return;
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
