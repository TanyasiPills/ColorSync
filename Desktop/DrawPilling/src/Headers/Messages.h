#pragma once
#include <vector>
struct Position {
	double x = 0.0;
	double y = 0.0;

	Position() : x(0), y(0) {}
	Position(double xIn, double yIn) : x(xIn), y(yIn) {}
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
	int location = -1;
	DrawMessage() : layer(0), brush(0), size(0.0f), positions(), offset(), color(), cursorScale() {}
};

struct Task {
	DrawMessage message;
	int location = -1;
};

struct NodeAddMessage : public Message {
	int location;
	int nodeType;
};

struct NodeRenameMessage : public Message {
	std::string name;
	int location;
};

struct NodeDeleteMessage : public Message {
	int location;
};

struct UserMoveMessage : public Message {
	std::string name;
	int profileId;
	Position position;
};