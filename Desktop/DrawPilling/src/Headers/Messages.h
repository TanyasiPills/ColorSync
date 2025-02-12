#pragma once
#include <vector>
struct Position {
	float x, y;

	Position() : x(0), y(0) {}
	Position(float xIn, float yIn) : x(xIn), y(yIn) {}
};

struct Message {
	int type;

	virtual ~Message() = default;
};

struct DrawMessage : public Message {
	int layer;
	int brush;
	float size;
	std::vector<Position> positions;
	Position offset;
	Position ratio;
	float cursorScale[3];
	float color[3];

	DrawMessage() : layer(0), brush(0), size(0.0f), positions(), offset(), color(), cursorScale() {}
};

struct NodeAddMessage : public Message {
	std::string name;
	int location;
	int nodeType;
};

struct NodeRenameMessage : public Message {
	std::string name;
	int location;
};