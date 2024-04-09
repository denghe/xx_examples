#pragma once
#include "looper.h"
#include "bug.h"

struct Scene : SceneBase {
	Shared<Node> rootNode;

	Camera camera;
	Rnd rnd;
	SpaceGrid<BugBody> grid;

	void CreateSnake(XY const& headPos, int len);

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
};

inline Scene* gScene{};
