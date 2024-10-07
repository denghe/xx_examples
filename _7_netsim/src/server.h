#pragma once
#include "looper.h"
#include "msgs.h"
#include "net.h"

struct Server : NetServer {
	xx::Shared<Msgs::Global::Scene> scene;

	void Init();
	void Update();

	void Log_Msg_Read_Error() {}
	void Log_Msg_Receive_Unknown() {}
};
