import { backendIp } from "../../constants"
import { DrawMessage, Message, NodeAddMessage, NodeDeleteMessage, NodeRenameMessage } from "./Messages"
import { Folder, Render } from "./Render"
import { io, Socket } from "socket.io-client"

export enum ActionType {
    Draw,
    AddNode,
    RenameNode,
    DeleteNode
}

export type RoomUser = { id: number, username: string, isOwner: boolean }
export type Position = { x: number, y: number }

export class SManager {
    private socket: Socket | null = null
    private onServer: boolean = false
    private rendererSocks: Render | null = null
    private runtime: RuntimeDataManager = RuntimeDataManager.getInstance()
    private canvasSizes: [number, number] = [0, 0]
    private users: RoomUser[] = []
    private history: any[] = []
    private isAmOwner: boolean = false

    setRenderer(rendererIn: Render): void {
        this.rendererSocks = rendererIn;
    }

    setMyOwnerState(state: boolean): void {
        this.isAmOwner = state;
    }

    amIOwner(): boolean {
        return this.isAmOwner;
    }

    getCanvasSize(): [number, number] {
        return this.canvasSizes;
    }

    setCanvasSize(width: number, height: number): void {
        this.canvasSizes = [width, height];
    }

    getUsers(): RoomUser[] {
        return this.users
    }

    connect(header: { [key: string]: string }, room: { [key: string]: string }): void {
        const options = {
            extraHeaders: {
                token: header["token"],
                password: header["password"]
            },
            query: room
        }
        this.socket = io(backendIp, options)
        this.socket.on("connect", () => {
            console.log("Connected to server")
            this.onServer = true
        })
        this.socket.on("disconnect", (reason: any) => {
            console.log("Connection closed", reason)
            this.onServer = false
        })
        this.socket.on("system message", (data: any) => {
            this.onSystemMessage(data)
        })
        this.socket.on("error", (error: any) => {
            console.error("[ERROR]:", error.message)
        })
        this.socket.on("message", (data: any) => {
            this.onMessage(data)
        })
        this.socket.on("mouse", (data: any) => {
            this.onPositionMessage(data)
        })
        this.socket.on("action", (data: any) => {
            this.onAction(data)
        })
    }

    down(): void {
        if (this.socket) {
            this.socket.disconnect()
        }
    }

    processHistory(): void {
        for (const data of this.history) {
            this.processAction(data)
        }
        this.history = []
    }

    processAction(dataIn: any): void {
        const type: number = dataIn.type;
        const data = dataIn.data;
        switch (type) {
            case ActionType.Draw:
                try {
                    const drawMessage = new DrawMessage();
                    drawMessage.layer = data.layer;
                    drawMessage.brush = data.brush;
                    drawMessage.size = data.size;
                    drawMessage.positions = data.positions.map((pos: any) => ({ x: pos.x, y: pos.y }));
                    drawMessage.color = [data.color.r, data.color.g, data.color.b];
                    drawMessage.offset = { x: data.offset.x, y: data.offset.y };
                    drawMessage.ratio = { x: data.ratio.x, y: data.ratio.y };
                    drawMessage.cursorScale = [data.CurSca.x, data.CurSca.y, data.CurSca.z];
                    if (this.rendererSocks) {
                        this.rendererSocks.executeMainThreadTask(drawMessage);
                    }
                } catch (e) {
                    console.error("Error receiving DrawMessage")
                }
                break
            case ActionType.AddNode:
                try {
                    const typeOfNode: number = data.node
                    const location: number = data.location
                    if (typeOfNode === 0) {
                        if (this.rendererSocks) {
                            this.rendererSocks.createLayer(location)
                        }
                    } else if (typeOfNode === 1) {
                        if (this.rendererSocks) {
                            this.rendererSocks.createFolder(location)
                        }
                    }
                } catch (e) {
                    console.error("Error receiving AddNodeMessage")
                }
                break
            case ActionType.RenameNode:
                try {
                    const location: number = data.location;
                    const node = this.rendererSocks?.nodes.get(location);
                    if (node) {
                        node.name = data.name;
                    }
                } catch (e) {
                    console.error("Error receiving RenameNodeMessage");
                }
                break;

            case ActionType.DeleteNode:
                try {
                    const id: number = data.location;
                    const node = this.rendererSocks?.nodes.get(id);
                    if (node) {
                        if (node as Folder) {                            
                            this.rendererSocks!.removeFolder(id);
                        } else {
                            this.rendererSocks!.removeLayer(id);
                        }
                    }
                } catch (e) {
                    console.error("Error receiving DeleteNodeMessage");
                }
                break;
        }
    }

    onPositionMessage(data: any): void {
        const userId = data.userId
        const userPos = { x: data.position.x, y: data.position.y }
        if (DrawUI.userPositions.has(userId)) {
            const user = DrawUI.userPositions.get(userId)
            if (user) {
                user.pos = userPos
            }
        }
    }

    sendPositionMessage(cursorPos: Position): void {
        const msg = { position: { x: cursorPos.x, y: cursorPos.y } }
        if (this.socket) {
            this.socket.emit("mouse", msg)
        }
    }

    onSystemMessage(data: any): void {
        console.log("[SYSTEM]:")
        const type = data.type
        switch (type) {
            case 0:
                console.log("[SERVER MESSAGE]:", data.message)
                break
            case 1:
                console.log("[SERVER MESSAGE]:", data.message)
                this.users.push({ id: data.id, username: data.username, isOwner: false })
                break
            case 2:
                console.log("[SERVER MESSAGE]:", data.message)
                this.users = this.users.filter(user => user.id !== data.id)
                break
            case 3:
                this.users = []
                this.history = []
                const admin = data.owner
                this.users.push({ id: this.runtime.id, username: this.runtime.username, isOwner: this.runtime.id === admin })
                this.isAmOwner = this.runtime.id === admin
                for (const userData of data.users) {
                    this.users.push({ id: userData.id, username: userData.username, isOwner: userData.id === admin })
                }
                this.history = data.history
                this.canvasSizes = [data.width, data.height]
                DrawUI.canInit = true
                break
            default:
                break
        }
    }

    onAction(data: any): void {
        this.processAction(data)
    }

    kick(id: number): void {
        if (!this.onServer) return
        const msg = { type: "kick", data: { id } }
        if (this.socket) {
            this.socket.emit("manage", msg)
        }
    }

    sendAction(dataIn: Message): void {
        if (!this.onServer) return
        let msg: any = {}
        let data: any = {}
        switch (dataIn.type) {
            case ActionType.Draw:
                try {
                    const draw = dataIn as DrawMessage
                    msg.type = ActionType.Draw
                    data.layer = draw.layer
                    data.brush = draw.brush
                    data.size = draw.size
                    data.positions = draw.positions.map(p => ({ x: p.x, y: p.y }))
                    data.offset = { x: draw.offset.x, y: draw.offset.y }
                    data.ratio = { x: draw.ratio.x, y: draw.ratio.y }
                    data.color = { r: draw.color[0], g: draw.color[1], b: draw.color[2] }
                    data.CurSca = { x: draw.cursorScale[0], y: draw.cursorScale[1], z: draw.cursorScale[2] }
                } catch (e) {
                    console.error("bad data type: not DrawMessage")
                }
                break
            case ActionType.AddNode:
                try {
                    const node = dataIn as NodeAddMessage
                    msg.type = ActionType.AddNode
                    data.node = node.nodeType
                    data.location = node.location
                } catch (e) {
                    console.error("bad data type: not AddNodeMessage")
                }
                break
            case ActionType.RenameNode:
                try {
                    const node = dataIn as NodeRenameMessage
                    msg.type = ActionType.RenameNode
                    data.name = node.name
                    data.location = node.location
                } catch (e) {
                    console.error("bad data type: not RenameNodeMessage")
                }
                break
            case ActionType.DeleteNode:
                try {
                    const node = dataIn as NodeDeleteMessage
                    msg.type = ActionType.DeleteNode
                    data.location = node.location
                } catch (e) {
                    console.error("bad data type: not DeleteNodeMessage")
                }
                break
            default:
                break
        }
        msg.data = data
        if (this.socket) {
            this.socket.emit("action", msg)
        }
    }

    onMessage(data: any): void {
        const username = data.username
        const message = data.message
        const messageObject: { [key: string]: string } = { username, message }
        DrawUI.getMsg(messageObject)
    }

    sendMsg(msg: string): void {
        if (this.socket) {
            this.socket.emit("message", { message: msg })
        }
    }
}