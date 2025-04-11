export class Position {
    x: number;
    y: number;

    constructor(xIn: number = 0, yIn: number = 0) {
        this.x = xIn;
        this.y = yIn;
    }
}

abstract class Message {
    type: number;

    constructor(type: number) {
        this.type = type;
    }
}

export class DrawMessage extends Message {
    layer: number;
    brush: number;
    size: number;
    positions: Position[];
    offset: Position;
    ratio: Position;
    cursorScale: number[];
    color: number[];

    constructor() {
        super(0); 
        this.layer = 0;
        this.brush = 0;
        this.size = 0.0;
        this.positions = [];
        this.offset = new Position();
        this.ratio = new Position();
        this.cursorScale = [0, 0, 0];
        this.color = [0, 0, 0];
    }
}

export class NodeAddMessage extends Message {
    location: number;
    nodeType: number;

    constructor(location: number = 0, nodeType: number = 0) {
        super(1); 
        this.location = location;
        this.nodeType = nodeType;
    }
}

export class NodeRenameMessage extends Message {
    name: string;
    location: number;

    constructor(name: string = '', location: number = 0) {
        super(2); 
        this.name = name;
        this.location = location;
    }
}

export class NodeDeleteMessage extends Message {
    location: number;

    constructor(location: number = 0) {
        super(3); 
        this.location = location;
    }
}

export class UserMoveMessage extends Message {
    name: string;
    profileId: number;
    position: Position;

    constructor(name: string = '', profileId: number = 0, position: Position = new Position()) {
        super(4); 
        this.name = name;
        this.profileId = profileId;
        this.position = position;
    }
}