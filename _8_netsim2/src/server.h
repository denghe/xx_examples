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
	// ~Peer() notice all ?

	void Init(Server* server_, xx::Weak<Client>&& client_);
	bool Alive() const;
	bool Send(xx::DataShared ds);

	// print client id ?
	void Log_Msg_Wait_Join_Disconnected() {
		xx::CoutTN(clientId, " S Log_Msg_Wait_Join_Disconnected"); 
	};
	void Log_Msg_Wait_Join_Receive_Unknown(int16_t typeId) {
		xx::CoutTN(clientId, " S Log_Msg_Wait_Join_Receive_Unknown: ", typeId);
	};
	void Log_Msg_Wait_Commands_Disconnected() {
		xx::CoutTN(clientId, " S Log_Msg_Wait_Commands_Disconnected");
	};
	void Log_Msg_Handle_Summon_Error_Player_Not_Found() {
		xx::CoutTN(clientId, " S Log_Msg_Handle_Summon_Error_Player_Not_Found");
	};
	void Log_Msg_Wait_Commands_Receive_Unknown(int16_t typeId) {
		xx::CoutTN(clientId, " S Log_Msg_Wait_Commands_Receive_Unknown: ", typeId);
	};
	template<typename T> void Log_Msg_ReadError() {
		xx::CoutTN(clientId, " S Log_Msg_ReadError: ", nameof::nameof_type<T>());
	};
};

struct Server {
	xx::Shared<Msgs::Global::Scene> scene;

	int32_t autoClientId{};
	xx::Listi32<xx::Shared<Peer>> peers;

	void Init();
	void Update();

	xx::Weak<Peer> Accept(xx::Shared<Client> const& client_);
	void SendToAll(xx::DataShared ds);

	void Gen250Monsters();
};
