#pragma once
#include "looper.h"
#include "msgs.h"

struct Peer;
struct Client {
	xx::Shared<Msgs::Global::Scene> scene;
	XY centerPos;
	XY min{}, max{};

	int32_t clientId{};
	xx::Weak<Peer> peer;
	xx::Queue<xx::DataShared> recvs;
	xx::Task<> task;
	xx::Task<> Task();

	void Init(XY const& centerPos_);
	void Update();
	void Draw();

	void CleanUp();
	void Dial();
	bool Alive() const;
	bool Joined() const;
	void Close();
	bool Send(xx::DataShared ds);

	void Log_Dial_Fail_Retry() {
		xx::CoutTN("C Log_Dial_Fail_Retry");
	};
	void Log_Send_Join_Fail() {
		xx::CoutTN("C Log_Send_Join_Fail");
	};
	void Log_Msg_Wait_Join_r_Disconnected() { 
		xx::CoutTN("C Log_Msg_Wait_Join_r_Disconnected");
	};
	void Log_Msg_Wait_Join_r_Receive_Unknown(uint16_t typeId) {
		xx::CoutTN("C Log_Msg_Wait_Join_r_Receive_Unknown: ", typeId);
	};
	void Log_Msg_Wait_Commands_Disconnected() {
		xx::CoutTN(clientId, " C Log_Msg_Wait_Commands_Disconnected");
	};
	void Log_Msg_Wait_Commands_Receive_Unknown() {
		xx::CoutTN(clientId, " C Log_Msg_Wait_Commands_Receive_Unknown");
	};
	void Log_Msg_Handle_Summon_r_Error_Player_Not_Found(int32_t clientId) {
		xx::CoutTN(this->clientId, " C Log_Msg_Handle_Summon_r_Error_Player_Not_Found: ", clientId);
	};
	template<typename T> void Log_Msg_ReadError() {
		xx::CoutTN(clientId, " C Log_Msg_ReadError: ", nameof::nameof_type<T>());
	};
};
