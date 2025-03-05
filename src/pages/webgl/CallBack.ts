import { useState, useEffect, useRef } from "react";
import { Render } from "./Render";

export function useColorWheel() {
    const colorWheelRef = useRef<HTMLCanvasElement | null>(null);
    const colorColumnRef = useRef<HTMLCanvasElement | null>(null);
    const [cwColor, setCwColor] = useState("rgb(255, 0, 0)");
    const [RGBColor, setRGBColor] = useState("rgb(0, 0, 0)");
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
        localStorage.setItem("selectedColor", newRGB);
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

export class useRender {
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

    constructor() {
        let canvas: HTMLCanvasElement;
        if (this.canvasRef.current) {
            canvas = this.canvasRef.current;
        }
        
        if (this.canvasRef.current) {
            this.canvasRef.current.addEventListener("mouseenter", () => {
                this.mouseIn = true;
            });
            this.canvasRef.current.addEventListener("mouseleave", () => {
                this.mouseIn = false;
            });
            if (!this.mouseIn) return;
            this.canvasRef.current.addEventListener("wheel", (event) =>{
                if (event.deltaY > 0) {
                    this.offset[0] -= (this.GlCursorPos[0] - this.offset[0]) * 0.25;
                    this.offset[1] -= (this.GlCursorPos[1] - this.offset[1]) * 0.25;
                    this.render.zoom(1.25, this.offset);
                }
                else if(event.deltaY < 0){
                    this.offset[0] += (this.GlCursorPos[0] - this.offset[0]) * 0.2;
                    this.offset[1] += (this.GlCursorPos[1] - this.offset[1]) * 0.2;
                    this.render.zoom(0.8, this.offset);
                }
            })

        }

    }
}
