import { useRender } from "./CallBack";
import { CanvasData, Drawing } from "./Drawing";
import { IndexBuffer } from "./IndexBuffer";
import { DrawMessage, NodeRenameMessage, Position, UserMoveMessage } from "./Messages";
import { Shader } from "./Shader";
import { VertexArray } from "./Shaders/VertexArray ";
import { SManager } from "./SocksManager";
import { Texture } from "./Texture";

export class RenderData {
    va!: VertexArray;
    ib!: IndexBuffer;
    shader!: Shader;
    texture!: Texture;
    fbo: WebGLBuffer | null = null;
}

export class Node {
    name: string;
    visible: boolean = true;
    editing: boolean = false;
    selected: boolean = false;
    id: number;
    opacity: number = 100;

    constructor(name: string, id: number) {
        this.name = name;
        this.id = id;
    }
}

export class Layer extends Node {
    data: RenderData;

    constructor(name: string, id: number, data: RenderData) {
        super(name, id);
        this.data = data;
    }
}

export class Folder extends Node {
    open: boolean = false;
    childrenIds: number[] = [];

    constructor(name: string, id: number) {
        super(name, id);
    }

    addChild(childId: number): void {
        this.childrenIds.push(childId);
    }
}

export class Render {
    private gl: WebGL2RenderingContext;
    public cursor!: RenderData;
    public dataForCanvas: CanvasData;

    private zoomRatio: number;
    private cursorRadius: number;
    private initialCanvasRatio: [number, number];
    private canvasRatio: Float32Array;
    private identityRatio: [number, number];
    private offset: [number, number];
    private cursorScale: [number, number, number];
    private identityOffset: [number, number];
    private prevPos: [number, number];
    private prevPrevPos: [number, number];
    private canvasSize: [number, number];
    private currentBrush: number;
    private currentPos: [number, number];

    private color: [number, number, number];
    private sentBrushSize: number;
    private sentOffset: [number, number];

    private drawing: Drawing | null;
    private callBack: useRender | null;
    private Smanager: SManager | null;

    public brushes: RenderData[] = [];
    public usersToMove: Map<number, UserMoveMessage> = new Map();
    public recieving: boolean = false;
    public onUI: boolean = false;
    public currentNode: number = 0;
    public nextFreeNodeIndex: number = 0;
    public currentFolder: number = 0;
    public tool: number = 0;
    public currentLayerToDrawOn: number = 0;
    public inited: boolean = false;
    public online: boolean = false;

    public nodes: Map<number, Node> = new Map();
    public folders: number[] = [];
    public layers: number[] = [];
    public taskQueue: DrawMessage[] = [];

    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.cursor = new RenderData();
        this.dataForCanvas = new CanvasData();
        this.Smanager = new SManager();

        this.drawing = null;
        this.callBack = null;

        this.zoomRatio = 0;
        this.cursorRadius = 0.01;
        this.initialCanvasRatio = [1.0, 1.0];
        this.canvasRatio = new Float32Array([1.0, 1.0]);
        this.identityRatio = [1.0, 1.0];
        this.offset = [0, 0];
        this.cursorScale = [1.0, 1.0, 1.0];
        this.identityOffset = [0, 0];
        this.prevPos = [0, 0];
        this.prevPrevPos = [0, 0];
        this.canvasSize = [1, 1];
        this.currentBrush = 0;
        this.currentPos = [0, 0];

        this.sentBrushSize = 0;
        this.sentOffset = [0, 0];

        const rgbString = localStorage.getItem("currentRGB");
        if (rgbString) {
            const matches = rgbString.match(/^rgb\((\d+), (\d+), (\d+)\)$/);
            if (matches) {
                this.color = [Number(matches[1]), Number(matches[2]), Number(matches[3])];
            } else {
                console.error("Invalid RGB format");
                this.color = [0, 0, 0];
            }
        } else {
            this.color = [0, 0, 0];
        }

        localStorage.setItem("currentRGB", `rgb(${this.color[0]}, ${this.color[1]}, ${this.color[2]})`);
    }

    public getCanvasSize(): [number, number] {
        console.log(`width: ${this.canvasSize[0]}, height: ${this.canvasSize[1]}`)
        return this.canvasSize
    }

    setDrawData(canvasWidthIn: number, canvasHeightIn: number): void {
        this.canvasSize[0] = canvasWidthIn;
        this.canvasSize[1] = canvasHeightIn;

        this.initBrushes();

        const canvasData: CanvasData = this.drawing!.initCanvas(canvasWidthIn, canvasHeightIn);

        this.initialCanvasRatio[0] = canvasData.canvasX;
        this.initialCanvasRatio[1] = canvasData.canvasY;

        this.canvasRatio[0] = canvasData.canvasX;
        this.canvasRatio[1] = canvasData.canvasY;
    }

    executeMainThreadTask(drawMessage: DrawMessage): void {
        this.taskQueue.push(drawMessage);
    }

    setDrawDataJa() {
        this.sentBrushSize = this.cursorRadius;
        this.sentOffset[0] = this.offset[0];
        this.sentOffset[1] = this.offset[1];
    }

    initBrushes(): void {
        let cursorBrush: RenderData = new RenderData();
        this.drawing?.initBrush(cursorBrush, this.cursorRadius, "Shaders/Textures/cursor.png");
        this.brushes.push(cursorBrush);

        let penBrush: RenderData = new RenderData();
        this.drawing?.initBrush(penBrush, this.cursorRadius, "Shaders/Textures/penBrush.png");
        this.brushes.push(penBrush);

        let airBrush: RenderData = new RenderData();
        this.drawing?.initBrush(airBrush, this.cursorRadius, "Shaders/Textures/airBrush.png");
        this.brushes.push(airBrush);

        let waterBrush: RenderData = new RenderData();
        this.drawing?.initBrush(waterBrush, this.cursorRadius, "Shaders/Textures/waterBrush.png");
        this.brushes.push(waterBrush);

        let charCoalBrush: RenderData = new RenderData();
        this.drawing?.initBrush(charCoalBrush, this.cursorRadius, "Shaders/Textures/charCoalBrush.png");
        this.brushes.push(charCoalBrush);

        this.cursor = this.brushes[0];
    }

    /*initLayers(canvasData: CanvasData): void {
        this.nodes.set(this.nextFreeNodeIndex, new Folder("Root", this.nextFreeNodeIndex));
        this.folders.push(this.nextFreeNodeIndex);
        this.nextFreeNodeIndex++;

        this.nodes.set(this.nextFreeNodeIndex, new Layer("Main", this.nextFreeNodeIndex, canvasData.data));
        this.layers.push(this.nextFreeNodeIndex);
        this.currentNode = this.nextFreeNodeIndex;
        this.nextFreeNodeIndex++;

        (this.nodes.get(0) as Folder)?.addChild(this.currentNode);

        this.nodes.set(this.nextFreeNodeIndex, new Folder("Folder", this.nextFreeNodeIndex));
        this.folders.push(this.nextFreeNodeIndex);
        (this.nodes.get(0) as Folder)?.addChild(this.nextFreeNodeIndex);

        let folder = this.nextFreeNodeIndex;
        this.nextFreeNodeIndex++;

        let layerTwo: RenderData = new RenderData();
        this.drawing?.initLayer(layerTwo, this.canvasRatio[0], this.canvasRatio[1]);

        this.nodes.set(this.nextFreeNodeIndex, new Layer("Not main", this.nextFreeNodeIndex, layerTwo));
        this.layers.push(this.nextFreeNodeIndex);
        (this.nodes.get(folder) as Folder)?.addChild(this.nextFreeNodeIndex);
        this.nextFreeNodeIndex++;
    }*/

    createLayer(parent: number): number {
        const index = this.nextFreeNodeIndex;

        let createdLayer: RenderData = new RenderData();
        this.drawing?.initLayer(createdLayer);

        const newLayer = new Layer("NewLayer" + (this.layers.length + 1), index, createdLayer);
        this.nodes.set(index, newLayer);

        this.layers.push(index);

        this.nextFreeNodeIndex++;

        const parentFolder = this.nodes.get(parent) as Folder;
        parentFolder?.addChild(index);

        this.drawing?.moveCanvas(createdLayer, this.canvasRatio, this.offset);

        return index;
    }

    createFolder(parent: number): number {
        const index = this.nextFreeNodeIndex;

        const newFolder = new Folder("Folder", index);
        this.nodes.set(index, newFolder);

        this.folders.push(index);

        const parentFolder = this.nodes.get(parent) as Folder;
        parentFolder?.addChild(index);

        this.nextFreeNodeIndex++;

        return index;
    }

    removeLayer(index: number): void {
        const layerIndex = this.layers.indexOf(index);
        if (layerIndex !== -1) {
            this.layers.splice(layerIndex, 1);

            this.nodes.delete(index);
        }
    }

    removeFolder(index: number): void {
        const folderIndex = this.folders.indexOf(index);
        if (folderIndex !== -1) {
            this.folders.splice(folderIndex, 1);

            this.nodes.delete(index);
        }
    }

    changeBrush(index: number): void {
        this.cursor = this.brushes[index];
        this.currentBrush = index;
    }

    initNewCanvas(): void {
        this.nodes.clear();
        this.folders.length = 0;
        this.layers.length = 0;
        this.nextFreeNodeIndex = 0;

        this.nodes.set(this.nextFreeNodeIndex, new Folder("Root", this.nextFreeNodeIndex));
        this.folders.push(this.nextFreeNodeIndex);
        this.nextFreeNodeIndex++;

        this.nodes.set(this.nextFreeNodeIndex, new Layer("BackGround", this.nextFreeNodeIndex, this.dataForCanvas.data));
        this.layers.push(this.nextFreeNodeIndex);
        this.currentNode = this.nextFreeNodeIndex;
        this.nextFreeNodeIndex++;

        (this.nodes.get(0) as Folder)?.addChild(this.currentNode);

        const index = this.nextFreeNodeIndex;
        this.layers.push(index);
        const createdLayer = new RenderData();
        this.drawing?.initLayer(createdLayer);
        this.nodes.set(index, new Layer("Main", index, createdLayer));
        this.nextFreeNodeIndex++;

        (this.nodes.get(0) as Folder)?.addChild(index);

        if (this.online) {
            this.Smanager?.processHistory();
        }

        this.inited = true;

    }

    moveLayers(offsetIn: [number, number]): void {
        this.offset[0] = offsetIn[0];
        this.offset[1] = offsetIn[1];

        for (let item of this.layers) {
            const layer: Layer = this.nodes.get(item) as Layer;
            this.drawing?.moveCanvas(layer.data, this.canvasRatio, this.offset)
        }
    }

    zoom(scale: number, offsetIn: [number, number]): void {
        this.offset[0] *= scale;
        this.offset[1] *= scale;
        this.initialCanvasRatio[0] *= scale;
        this.initialCanvasRatio[1] *= scale;
        this.offset[0] = offsetIn[0];
        this.offset[1] = offsetIn[1];

        for (let item of this.layers) {
            const layer: Layer = this.nodes.get(item) as Layer;
            this.drawing?.moveCanvas(layer.data, this.canvasRatio, this.offset)
        }

        this.cursorRadius *= scale;
        this.zoomRatio *= scale;
    }

    onResize(x: number, y: number, offsetIn: [number, number], yRatio: number): void {
        this.cursorScale[0] = x;
        this.cursorScale[1] = y * yRatio;
        this.canvasRatio[0] = x * this.initialCanvasRatio[0];
        this.canvasRatio[1] = y * this.initialCanvasRatio[1];

        this.offset[0] = offsetIn[0];
        this.offset[1] = offsetIn[1];

        for (let item of this.layers) {
            const layer: Layer = this.nodes.get(item) as Layer;
            this.drawing?.moveCanvas(layer.data, this.canvasRatio, this.offset)
        }
    }

    loadPrevCursor(prevIn: Float32Array): void {
        this.prevPrevPos[0] = prevIn[0];
        this.prevPrevPos[1] = prevIn[1];

        this.prevPos[0] = prevIn[0];
        this.prevPos[1] = prevIn[1];
    }

    renderCursorToCanvas(): void {
        if (this.recieving) return;
        const node = this.nodes.get(this.currentNode);
        if (node instanceof Layer) {
            const layer = node.data;
            this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, layer.fbo);
            this.gl.viewport(0, 0, this.canvasSize[0], this.canvasSize[1]);

            const pos: [number, number] = this.callBack!.cursorPosition();

            switch (this.tool) {
                case 0: {
                    const dx = pos[0] - this.prevPos[0];
                    const dy = pos[1] - this.prevPos[1];
                    const scaledDx = dx * this.canvasSize[0];
                    const scaledDy = dy * this.canvasSize[1];
                    const distance = Math.sqrt(scaledDx * scaledDx + scaledDy * scaledDy);

                    const numSamples = 1 + distance / (this.cursorRadius * 1000);

                    const ctrlX = 2 * this.prevPos[0] - 0.5 * (this.prevPrevPos[0] + pos[0]);
                    const ctrlY = 2 * this.prevPos[1] - 0.5 * (this.prevPrevPos[1] + pos[1]);

                    const drawData = this.drawing!;
                    //drawData.drawLine(layer.data, this.color, this.cursor, this.cursorRadius, numSamples, this.prevPos, pos, ctrlX, ctrlY);
                    break;
                }
                case 1:
                    break;
            }

            this.prevPrevPos[0] = this.prevPos[0];
            this.prevPrevPos[1] = this.prevPos[1];

            this.prevPos[0] = pos[0];
            this.prevPos[1] = pos[1];
        }
    }

    changeCursor(position: [number, number]): void {
        this.cursorScale[0] = position[0];
        this.cursorScale[1] = position[1];
    }

    sendDraw(): void {
        let msg: DrawMessage = new DrawMessage();
        msg.type = 0;
        msg.layer = this.currentNode;
        msg.brush = this.currentBrush;
        msg.size = this.cursorRadius;
        //msg.positions = draw
        msg.offset = new Position(this.offset[0], this.offset[1]);
        msg.color[0] = this.color[0];
        msg.color[1] = this.color[1];
        msg.color[2] = this.color[2];
        msg.ratio.x = this.canvasRatio[0];
        msg.ratio.y = this.canvasRatio[1];
        msg.cursorScale[0] = this.cursorScale[0];
        msg.cursorScale[1] = this.cursorScale[1];
        msg.cursorScale[2] = this.cursorScale[2];

        //drawpositions clear
    }

    sendLayerRename(nameIn: string, locationIn: number): void {
        let msg: NodeRenameMessage = new NodeRenameMessage();
        msg.type = 2;
        msg.name = nameIn;
        msg.location = locationIn;
    }

    clear(): void {
        this.gl.clearColor(0.188, 0.188, 0.313, 0);
        this.gl.clear(this.gl.COLOR_BUFFER_BIT);
    }

    draw(data: RenderData): void {

        data.va.bind();
        data.shader.bind();
        data.texture.bind();

        this.gl.drawElements(this.gl.TRIANGLES, data.ib.getCount(), this.gl.UNSIGNED_INT, 0);
    }

    renderDrawMessage(): void {
        //uhm Bence
    }

    renderLayers(): void {
        this.gl.blendFunc(this.gl.SRC_ALPHA, this.gl.ONE_MINUS_SRC_ALPHA);
        for (let item of this.layers) {
            const layer: Layer = this.nodes.get(item) as Layer;
            layer.data.shader.bind();
            layer.data.shader.setUniform1f("opacity", layer.opacity / 100);
            this.draw(layer.data);
        }
        this.gl.blendFunc(this.gl.ONE, this.gl.ONE_MINUS_SRC_ALPHA);
    }

    renderCursor(): void {
        let now: [number, number] = this.callBack!.cursorPosition();
        this.drawing?.brushToPosition(this.cursor, this.cursorRadius, this.identityRatio, this.identityOffset, this.cursorScale, now);
    }

    render(): void {
        this.clear();
        if (this.inited) {
            this.renderLayers();
            this.renderCursor();
            const pos: [number, number] = this.callBack!.cursorPosition();
            if (this.online) {
                if (this.currentPos[0] != pos[0] || this.currentPos[1] != pos[1]) {
                    let posSend: Position = new Position();
                    posSend.x = pos[0];
                    posSend.y = pos[1];
                    this.currentPos[0] = pos[0];
                    this.currentPos[1] = pos[1];
                }
            }
        }
    }
}
