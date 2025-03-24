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
    private canvasSize: [number, number];
    private fbo: WebGLFramebuffer | null;
    private drawing: Drawing | null;

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
    
    public nodes: Map<number, NodeMatyi> = new Map();
    public folders: number[] = [];
    public layers: number[] =  [];



    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.layers = [];
        this.cursor = new RenderData();
        this.drawing = null;

        this.cursorRadius = 0.01;
        this.initialCanvasRatio = [1.0, 1.0];
        this.canvasRatio = new Float32Array([1.0, 1.0]);
        this.identityRatio = [1.0, 1.0];
        this.offset = [0, 0];
        this.cursorScale = [1.0, 1.0, 1.0];
        this.identityOffset = [0, 0];
        this.prevPos = [0, 0];
        this.canvasSize = [1, 1];
        this.fbo = null;
    }

    init(canvasWIn: number, canvasHIn: number, screenW: number, screenH: number): void {
        this.fbo = this.gl.createFramebuffer();
        this.canvasSize[0] = canvasWIn;
        this.canvasSize[1] = canvasHIn;
        this.drawing = new Drawing(this.gl, canvasWIn, canvasHIn);
        this.drawing.initBrush(this.cursor, this.cursorRadius, null);
        const canvasData: CanvasData = this.drawing.initCanvas(canvasWIn, canvasHIn);
        this.layers.push(canvasData.data);
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbo);
        this.gl.framebufferTexture2D(this.gl.FRAMEBUFFER, this.gl.COLOR_ATTACHMENT0, this.gl.TEXTURE_2D, canvasData.data.texture.getId(), 0);
        if (this.gl.checkFramebufferStatus(this.gl.FRAMEBUFFER) != this.gl.FRAMEBUFFER_COMPLETE) {
            console.error(this.gl.checkFramebufferStatus(this.gl.FRAMEBUFFER));
        }
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, 0);
        this.initialCanvasRatio[0] = canvasData.canvasX;
        this.initialCanvasRatio[1] = canvasData.canvasY;
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

    InitLayers(canvasData: CanvasData): void {
        this.nodes.set(this.nextFreeNodeIndex, new Folder("Root", this.nextFreeNodeIndex));
        this.folders.push(this.nextFreeNodeIndex);
        this.nextFreeNodeIndex++;

        this.nodes.set(this.nextFreeNodeIndex, new Layer("Main", this.nextFreeNodeIndex, canvasData.data));
        this.layers.push(this.nodes[this.nextFreeNodeIndex]);
        this.currentNode = this.nextFreeNodeIndex;
        this.nextFreeNodeIndex++;

        (this.nodes.get(0) as Folder)?.AddChild(this.currentNode);

        this.nodes.set(this.nextFreeNodeIndex, new Folder("Folder", this.nextFreeNodeIndex));
        this.folders.push(this.nextFreeNodeIndex);
        (this.nodes.get(0) as Folder)?.AddChild(this.nextFreeNodeIndex);

        let folder = this.nextFreeNodeIndex;
        this.nextFreeNodeIndex++;

        let layerTwo: RenderData = new RenderData();
        NewDraw.initLayer(layerTwo, canvasRatio[0], canvasRatio[1]);

        this.nodes.set(this.nextFreeNodeIndex, new Layer("Not main", this.nextFreeNodeIndex, layerTwo));
        this.layers.push(this.nextFreeNodeIndex);
        (this.nodes.get(folder) as Folder)?.AddChild(this.nextFreeNodeIndex);
        this.nextFreeNodeIndex++;
    }

    /**
     * rövid leírás
     * 
     * @param offsetIn leírás
     */
    moveLayers(offsetIn: Float32Array): void {
        this.offset[0] = offsetIn[0];
        this.offset[1] = offsetIn[1];
        for (let item of this.layers) {
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset);
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
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset)
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
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset)
        }
    }

    loadPrevCursor(prevIn: Float32Array): void {
        this.prevPos[0] = prevIn[0];
        this.prevPos[1] = prevIn[1];
    }

    renderCursorToCanvas(): void {
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbo);
        this.gl.viewport(0, 0, this.canvasSize[0], this.canvasSize[1]);
        //var pos: Float32Array = 

    }

    setDrawData(canvasWidthIn: number, canvasHeightIn: number): void {
        // Implement setting draw data logic
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
    
    renderCursorToCanvas(): void {
        // Implement cursor rendering on canvas
    }
    
    moveLayers(offset: Float32Array): void {
        this.offset[0] = offset[0];
        this.offset[1] = offset[1];
        for (let item of this.layers) {
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset);
        }
    }
    
    zoom(scale: number, cursorPos: Float32Array): void {
        this.offset[0] *= scale;
        this.offset[1] *= scale;
        this.initialCanvasRatio[0] *= scale;
        this.initialCanvasRatio[1] *= scale;
        this.offset[0] = cursorPos[0];
        this.offset[1] = cursorPos[1];
    
        for (let item of this.layers) {
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset);
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
            this.drawing?.moveCanvas(item, this.canvasRatio, this.offset);
        }
    }
    
    loadPrevCursor(GlCursorPos: Float32Array): void {
        this.prevPos[0] = GlCursorPos[0];
        this.prevPos[1] = GlCursorPos[1];
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
    
    executeMainThreadTask(drawMessage: DrawMessage): void {
        this.taskQueue.push(drawMessage);
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