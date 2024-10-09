#pragma once
#include "looper.h"
#include "msgs.h"

struct Server;
struct Client;
struct Peer {
	Server* server{};
	int32_t clientId{};
	xx::Weak<Client> client;
	xx::Queue<xx::DataShared> recvs;
	xx::Task<> task;
	xx::Task<> Task();

	void Init(Server* server_, xx::Weak<Client>&& client_);
	bool Alive() const;
	bool Send(xx::DataShared ds);

	// print client id ?
	void Log_Msg_Wait_Join_Disconnected() { xx::CoutTN("S Log_Msg_Wait_Join_Disconnected"); };
	void Log_Msg_Wait_Join_Receive_Unknown() { xx::CoutTN("S Log_Msg_Wait_Join_Receive_Unknown"); };
	void Log_Msg_Read_Join_Error() { xx::CoutTN("S Log_Msg_Read_Join_Error"); };
	void Log_Msg_Wait_Commands_Disconnected() { xx::CoutTN("S Log_Msg_Wait_Commands_Disconnected"); };
};

struct Server {
	xx::Shared<Msgs::Global::Scene> scene;

	int32_t autoClientId{};
	xx::Listi32<xx::Shared<Peer>> peers;

	void Init();
	void Update();

	xx::Weak<Peer> Accept(xx::Shared<Client> const& client_);
};
