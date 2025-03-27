import { useRender, useColorWheel } from "./CallBack";
import { CanvasData, Drawing } from "./Drawing";
import { IndexBuffer } from "./IndexBuffer";
import { Shader } from "./Shader";
import { VertexArray } from "./Shaders/VertexArray ";
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
    private color: [number, number, number];
    

    //Nézd át Matyi mit ad hozzá

    private drawing: Drawing | null;
    private callBack: useRender | null;    

    public brushes: RenderData[] = [];
    public usersToMove: Map<number, UserMoveMessage> = new Map();
    public recieving: boolean = false;
    public onUI: boolean = false;
    public currentNode: number = 0;
    public nextFreeNodeIndex: number = 0;
    public currentFolder: number = 0;
    public isEditor: boolean = false;
    public tool: number = 0;
    public currentLayerToDrawOn: number = 0;
    public inited: boolean = false;
    public online: boolean = false;

    public nodes: Map<number, Node> = new Map();
    public folders: number[] = [];
    public layers: number[] = [];

    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.cursor = new RenderData();

        this.drawing = null;
        this.callBack = null;

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
        this.color = [0, 0, 0];
    }

    setDrawData(canvasWIn: number, canvasHIn: number): void {
        this.canvasSize[0] = canvasWIn;
        this.canvasSize[1] = canvasHIn;

        this.drawing = new Drawing(this.gl, canvasWIn, canvasHIn);
        this.drawing.initBrush(this.cursor, this.cursorRadius, null);

        const canvasData: CanvasData = this.drawing.initCanvas(canvasWIn, canvasHIn);

        this.initialCanvasRatio[0] = canvasData.canvasX;
        this.initialCanvasRatio[1] = canvasData.canvasY;

        this.canvasRatio[0] = canvasData.canvasX;
        this.canvasRatio[1] = canvasData.canvasY;

        this.initLayers(canvasData);

        this.inited = true;
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

    initLayers(canvasData: CanvasData): void {
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
    }

    /**
     * rövid leírás
     * 
     * @param offsetIn leírás
     */
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
    }

    onResize(x: number, y: number, offsetIn: Float32Array, yRatio: number): void {
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

                    for (let i = 0; i < numSamples; i++) {
                        const t = i / numSamples;
                        const vx = (1 - t) * (1 - t) * this.prevPrevPos[0] + 2 * (1 - t) * t * ctrlX + t * t * pos[0];
                        const vy = (1 - t) * (1 - t) * this.prevPrevPos[1] + 2 * (1 - t) * t * ctrlY + t * t * pos[1];

                        const tmp: [number, number] = [vx, vy];

                        this.drawing?.brushToPosition(this.cursor, this.cursorRadius, this.canvasRatio, this.offset, this.cursorScale, tmp);
                        this.draw(this.cursor);
                    }
                    this.prevPrevPos[0] = this.prevPos[0];
                    this.prevPrevPos[1] = this.prevPos[1];
                    this.prevPos[0] = pos[0];
                    this.prevPos[1] = pos[1];
                } break;                

                case 1: { // erase
                    // Implementation needed
                } break;

                default:
                    break;
            }

            layer.texture.bind();
            this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
            const width = this.gl.canvas.width;
            const height = this.gl.canvas.height;
            this.gl.viewport(0, 0, width, height);
        }

    }

    draw(data: RenderData): void {
        // Implement drawing logic
    }

    clear(): void {
        // Implement clearing logic
    }

    render(): void {
        // Implement full render logic
    }

    renderLayers(): void {
        // Implement layer rendering logic
    }

    renderCursor(): void {
        // Implement cursor rendering logic
    }


    setColor(color: Float32Array): void {
        // Implement color setting logic
    }

    renderDrawMessage(drawMessage: DrawMessage): void {
        // Implement rendering logic for draw messages
    }

    sendDraw(): void {
        // Implement sending draw data
    }

    sendLayerRename(name: string, location: number): void {
        // Implement renaming layer logic
    }

    getParent(id: number): number {
        for (const foldrIndex of this.folders) {
            Folder
        }
        return -1;
    }

    createLayer(parent: number): number {
        return -1;
    }

    createFolder(parent: number): number {
        return -1;
    }

    removeLayer(index: number): void {
        // Implement removing layer logic
    }

    removeFolder(index: number): void {
        // Implement removing folder logic
    }

    changeBrush(index: number): void {
        // Implement brush change logic
    }

    setOnline(value: boolean): void {
        this.online = value;
    }

    getOnline(): boolean {
        return this.online;
    }

    processTasks(): void {
        let task: DrawMessage | null;
        while ((task = this.taskQueue.pop()) !== null) {
            this.renderDrawMessage(task);
        }
    }


    swapView(): void {
        // Implement view swapping logic
    }


}