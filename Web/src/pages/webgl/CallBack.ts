import { useState, useEffect, useRef } from "react";
import { Render } from "./Render";

export function useColorWheel() {
    const colorWheelRef = useRef<HTMLCanvasElement | null>(null);
    const colorColumnRef = useRef<HTMLCanvasElement | null>(null);
    const [cwColor, setCwColor] = useState("rgb(255, 0, 0)");
    const [RGBColor, setRGBColor] = useState<string>(() => {        
        return localStorage.getItem("currentRGB") || "rgb(0, 0, 0)";
    });
    const [selectedColor, setSelectedColor] = useState<string | null>(() => {       
        return localStorage.getItem("selectedRGB");
    });
    const [holdC, setHoldC] = useState(false);
    const [holdCW, setHoldCW] = useState(false);
    const markerCW = useRef<HTMLDivElement | null>(null);
    const markerC = useRef<HTMLDivElement | null>(null);
    const [markerCWPos, setMarkerCWPos] = useState<{ x: number; y: number } | null>(null);

    useEffect(() => {
        if (markerCW.current) {
            markerCW.current.style.left = "150px";
            markerCW.current.style.top = "150px";
        }
        if (markerC.current) {
            markerC.current.style.top = "0px";
            markerC.current.style.background = cwColor;
        }
    }, []);


    useEffect(() => {
        const handleMouseUp = () => {
            setHoldC(false);
            setHoldCW(false);
        };

        window.addEventListener("mouseup", handleMouseUp);
    }, []);

    const handleMouseDownC = () => {
        setHoldC(true);
    };

    const handleMouseDownCW = () => {
        setHoldCW(true);
    }

    const updateRGBFromMarkerCW = () => {
        if (!colorWheelRef.current || !markerCWPos) return;
        const colorWheel = colorWheelRef.current;
        const ctx = colorWheel.getContext("2d");
        if (!ctx) return;
        const { x, y } = markerCWPos;
        if (x < 0 || x >= colorWheel.width || y < 0 || y >= colorWheel.height) return;
        const pixel = ctx.getImageData(x, y, 1, 1).data;
        const newRGB = `rgb(${pixel[0]}, ${pixel[1]}, ${pixel[2]})`;
        setRGBColor(newRGB);
        localStorage.setItem("currentRBG", newRGB);
        if (markerCW.current) {
            const brightness = (pixel[0] * 0.299) + (pixel[1] * 0.587) + (pixel[2] * 0.114);
            markerCW.current.style.borderColor = brightness < 28 ? "white" : "black";
        }
    };


    useEffect(() => {
        updateRGBFromMarkerCW();
    }, [cwColor, markerCWPos]);

    const handleColorWheelClick = (event: MouseEvent) => {
        if (holdCW) return
        if (!colorWheelRef.current) return;

        const colorWheel = colorWheelRef.current;
        const rect = colorWheel.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        if (x < 0 || x >= colorWheel.width || y < 0 || y >= colorWheel.height) return;

        setMarkerCWPos({ x, y });
        if (markerCW.current) {
            markerCW.current.style.left = `${x}px`;
            markerCW.current.style.top = `${y}px`;
        }
        updateRGBFromMarkerCW();
    };

    const handleColorWheelMove = (event: MouseEvent) => {
        if (!holdCW || !colorWheelRef.current) return;
        const colorWheel = colorWheelRef.current;
        const rect = colorWheel.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        if (x < 0 || x >= colorWheel.width || y < 0 || y >= colorWheel.height) return;

        setMarkerCWPos({ x, y });
        if (markerCW.current) {
            markerCW.current.style.left = `${x}px`;
            markerCW.current.style.top = `${y}px`;
        }
        updateRGBFromMarkerCW();
    };

    const handleColorColumnClick = (event: MouseEvent) => {
        if (holdC) return;
        if (!colorColumnRef.current) return;
        const colorColumn = colorColumnRef.current;
        const rect = colorColumn.getBoundingClientRect();
        const y = event.clientY - rect.top;
        if (y < 0 || y >= colorColumn.height) return;

        const ctx = colorColumn.getContext("2d")!;
        const pixel = ctx.getImageData(0, y, 1, 1).data;
        const newCWColor = `rgb(${pixel[0]}, ${pixel[1]}, ${pixel[2]})`;
        setCwColor(newCWColor);
        localStorage.setItem("selectedColor", newCWColor);
        if (markerC.current) {
            markerC.current.style.top = `${y}px`;
            markerC.current.style.backgroundColor = newCWColor;
        }
    };

    const handleColorColumnMove = (event: MouseEvent) => {
        if (!holdC || !colorColumnRef.current) return;
        const colorColumn = colorColumnRef.current;
        const rect = colorColumn.getBoundingClientRect();
        const y = event.clientY - rect.top;
        if (y < 0 || y >= colorColumn.height) return;

        const ctx = colorColumn.getContext("2d")!;
        const pixel = ctx.getImageData(0, y, 1, 1).data;
        const newCWColor = `rgb(${pixel[0]}, ${pixel[1]}, ${pixel[2]})`;
        setCwColor(newCWColor);
        localStorage.setItem("selectedColor", newCWColor);
        if (markerC.current) {
            markerC.current.style.top = `${y}px`;
            markerC.current.style.backgroundColor = newCWColor;
        }
        updateRGBFromMarkerCW();
    };

    useEffect(() => {
        const wheelCanvas = colorWheelRef.current;
        const columnCanvas = colorColumnRef.current;

        wheelCanvas?.addEventListener("mousedown", handleMouseDownCW);
        columnCanvas?.addEventListener("mousedown", handleMouseDownC);

        window.addEventListener("mousemove", handleColorWheelMove);
        window.addEventListener("mousemove", handleColorColumnMove);

        wheelCanvas?.addEventListener("click", handleColorWheelClick);
        columnCanvas?.addEventListener("click", handleColorColumnClick);

        return () => {
            wheelCanvas?.removeEventListener("mousedown", handleMouseDownCW);
            columnCanvas?.removeEventListener("mousedown", handleMouseDownC);
            window.removeEventListener("mousemove", handleColorWheelMove);
            window.removeEventListener("mousemove", handleColorColumnMove);
            wheelCanvas?.removeEventListener("click", handleColorWheelClick);
            columnCanvas?.removeEventListener("click", handleColorColumnClick);
        };
    }, [holdC, holdCW]);

    return { colorWheelRef, colorColumnRef, RGBColor, cwColor, markerCW, markerC, markerCWPos };
}

export function useColorPicker(){

}

export class useRender {
    private gl: WebGL2RenderingContext;
    private prevCursorPos: [number, number] = [0, 0];
    private GlCursorPos: [number, number] = [0, 0];
    private render!: Render;
    private screenW!: number;
    private screenH!: number;
    private offset: [number, number] = [0, 0];
    private windowOffset: [number, number] = [0, 0];

    private canvasRef = useRef<HTMLCanvasElement | null>(null);
    private mouseIn!: boolean;

    private mousedown: boolean = false;
    private moveCanvas: boolean = false;
    private scale: number = 1.0;
    private yRatio: number = 1.0;
    private originalSizeX: number = 1.0;
    private originalSizeY: number = 1.0;

    constructor(gl: WebGL2RenderingContext) {
        this.gl = gl;
        this.setupCanvasListeners();
      }
       
      private setupCanvasListeners() {
        if (this.canvasRef.current) {
          this.canvasRef.current.addEventListener("mouseenter", this.handleMouseEnter);
          this.canvasRef.current.addEventListener("mouseleave", this.handleMouseLeave);
          this.canvasRef.current.addEventListener("wheel", this.handleWheel);
          this.canvasRef.current.addEventListener("mousemove", this.handleMouseMove);
          this.canvasRef.current.addEventListener("mousedown", this.handleMouseDown);
          this.canvasRef.current.addEventListener("mouseup", this.handleMouseUp);
        }
      }
   
    private handleMouseEnter = () => {
        this.mouseIn = true;
    };
   
    private handleMouseLeave = () => {
        this.mouseIn = false;
    };
   
    private handleWheel = (event: WheelEvent): void => {
        if (!this.mouseIn) return;

        if (event.deltaY > 0) {
            this.offset[0] -= (this.GlCursorPos[0] - this.offset[0]) * 0.25;
            this.offset[1] -= (this.GlCursorPos[1] - this.offset[1]) * 0.25;
            this.render.zoom(1.25, this.offset);
        } else if (event.deltaY < 0) {
            this.offset[0] += (this.GlCursorPos[0] - this.offset[0]) * 0.2;
            this.offset[1] += (this.GlCursorPos[1] - this.offset[1]) * 0.2;
            this.render.zoom(0.8, this.offset);
        }
    };
   
    private handleMouseMove = (event: MouseEvent): void => {
        if (!this.mouseIn) return;
        const rect = this.canvasRef.current!.getBoundingClientRect();
        this.GlCursorPos[0] = (event.clientX - rect.left) / this.screenW * 2 - 1;
        this.GlCursorPos[1] = (event.clientY - rect.top) / this.screenH * 2 - 1;
        this.renderCursorToCanvas();
    };
   
    private handleMouseDown = (event: MouseEvent): void => {
        if (!this.mouseIn) return;

        this.mousedown = true;
        this.prevCursorPos = [this.GlCursorPos[0], this.GlCursorPos[1]];       
    };
   
    private handleMouseUp = (event: MouseEvent): void => {
        if (!this.mouseIn) return;

        this.mousedown = false;       
    };
   
    public cursorPosition(): [number, number] {
        return this.GlCursorPos;
    }

    private renderCursorToCanvas(): void {
        if (this.mousedown) {
            this.render.renderCursorToCanvas();
        }
    }
   
    public framebufferSizeCallback(width: number, height: number): void {
        this.gl.viewport(0, 0, width, height);
        this.render.clear();
        const x: number = this.originalSizeX / width;
        const y: number = this.originalSizeY / height;
        if (this.screenW != width) {
            const xOffset: number = ((width - this.screenW) / this.screenW) * x;
            this.offset[0] -= xOffset;
        }
        if (this.screenH != height) {
            const yOffset: number = ((width - this.screenH) / this.screenH) * y;
            this.offset[1] += yOffset;
        }

        this.screenW = width;
        this.screenH = height;
        
        this.render.onResize(x, y, this.offset, this.yRatio);
        this.render.render();
    }
   
    public handleCanvasMovement(event: MouseEvent): void {
        if (this.moveCanvas && this.canvasRef.current) {
            const deltaX = (event.clientX - this.prevCursorPos[0]) / this.screenW;
            const deltaY = (event.clientY - this.prevCursorPos[1]) / this.screenH;

            this.offset[0] += deltaX * 2;
            this.offset[1] -= deltaY * 2;

            this.prevCursorPos = [event.clientX, event.clientY];

            this.render.moveLayers(this.offset);
        }
    }
   
    public handleKeyDown(event: KeyboardEvent): void {
        if (event.key === " ") {
            this.moveCanvas = true;
        }
    }

    public handleKeyUp(event: KeyboardEvent): void {
        if (event.key === " ") {
            this.moveCanvas = false;
        }
    }

    public updateMarkerPosition(marker: HTMLElement, x: number, y: number): void {
        if (marker) {
            marker.style.left = `${x}px`;
            marker.style.top = `${y}px`;
        }
    }
       
   
    public cleanup() {
        const canvas = this.canvasRef.current;
        if (canvas) {
            canvas.removeEventListener("mouseenter", this.handleMouseEnter);
            canvas.removeEventListener("mouseleave", this.handleMouseLeave);
            canvas.removeEventListener("wheel", this.handleWheel);
            canvas.removeEventListener("mousemove", this.handleMouseMove);
            canvas.removeEventListener("mousedown", this.handleMouseDown);
            canvas.removeEventListener("mouseup", this.handleMouseUp);
        }

        window.removeEventListener("keydown", this.handleKeyDown);
        window.removeEventListener("keyup", this.handleKeyUp);
    }
}
