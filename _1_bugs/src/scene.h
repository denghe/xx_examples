#pragma once
#include "looper.h"
#include "bug.h"

struct Scene : SceneBase {
	Shared<Node> ui;

	Camera camera;
	Rnd rnd;
	SpaceGrid<BugBody> grid;

	void CreateBug(XY const& headPos, int len);

	virtual void Init() override;
	virtual void BeforeUpdate() override;
	virtual void Draw() override;
};

inline Scene* gScene{};
