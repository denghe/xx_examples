#pragma once
#include "pch.h"
#include "looper.h"

struct NetClient;
struct NetPeer {
	xx::Weak<NetClient> nc;
	xx::Queue<xx::DataShared> recvs;
	bool Alive() const;
	bool Send(xx::DataShared ds);
};

struct NetServer {
	xx::Listi32<xx::Shared<NetPeer>> peers;
	xx::Weak<NetPeer> Accept(xx::Shared<NetClient> const& nc);
	// ... send cmds
};

struct NetClient {
	xx::Weak<NetPeer> ns;
	xx::Queue<xx::DataShared> recvs;

	void Dial();
	bool Alive() const;
	void Close();
	bool Send(xx::DataShared ds);

	bool Send_Join();
	// ... send cmds
};
