import { useState, useEffect, useRef } from "react";

export function useColorPicker() {
    const colorWheelRef = useRef<HTMLCanvasElement | null>(null);
    const colorColumnRef = useRef<HTMLCanvasElement | null>(null);
    const [cwColor, setCwColor] = useState("rgb(255, 0, 0)");
    const [RGBColor, setRGBColor] = useState("rgb(0, 0, 0)");
    const [hold, setHold] = useState(false);
    const markerCW = useRef<HTMLDivElement | null>(null);
    const markerC = useRef<HTMLDivElement | null>(null);
    const [markerCWPos, setMarkerCWPos] = useState<{ x: number; y: number } | null>(null);

    useEffect(() => {
        if (colorWheelRef.current) {
            const canvas = colorWheelRef.current;
            const initX = canvas.width;
            const initY = canvas.height;
            setMarkerCWPos({ x: initX, y: initY });
            if (markerCW.current) {
                markerCW.current.style.left = `${initX }px`;
                markerCW.current.style.top = `${initY}px`;
            }
            updateRGBFromMarkerCW();
        }
        if (colorColumnRef.current && markerC.current) {
            markerC.current.style.top = `0px`;
            markerC.current.style.backgroundColor = cwColor;
        }
        const handleMouseUp = () => setHold(false);
        window.addEventListener("mouseup", handleMouseUp);

        return () => {
            window.removeEventListener("mouseup", handleMouseUp);
        };
    }, []);

    const handleMouseDown = () => {
        setHold(true);
    };

    const updateRGBFromMarkerCW = () => {
        if (!colorWheelRef.current || !markerCWPos) return;
        const canvas = colorWheelRef.current;
        const ctx = canvas.getContext("2d");
        if (!ctx) return;
        const { x, y } = markerCWPos;
        if (x < 0 || x >= canvas.width || y < 0 || y >= canvas.height) return;
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
        if (hold) return
        if (!colorWheelRef.current) return;

        const canvas = colorWheelRef.current;
        const rect = canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        if (x < 0 || x >= canvas.width || y < 0 || y >= canvas.height) return;

        setMarkerCWPos({ x, y });
        if (markerCW.current) {
            markerCW.current.style.left = `${x - 5}px`;
            markerCW.current.style.top = `${y - 5}px`;
        }
        updateRGBFromMarkerCW();
    };

    const handleColorWheelMove = (event: MouseEvent) => {
        if (!hold || !colorWheelRef.current) return;
        const canvas = colorWheelRef.current;
        const rect = canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        if (x < 0 || x >= canvas.width || y < 0 || y >= canvas.height) return;

        setMarkerCWPos({ x, y });
        if (markerCW.current) {
            markerCW.current.style.left = `${x}px`;
            markerCW.current.style.top = `${y}px`;
        }
        updateRGBFromMarkerCW();
    };

    const handleColorColumnClick = (event: MouseEvent) => {
        if (hold) return;
        if (!colorColumnRef.current) return;
        const canvas = colorColumnRef.current;
        const rect = canvas.getBoundingClientRect();
        const y = event.clientY - rect.top;
        if (y < 0 || y >= canvas.height) return;

        const ctx = canvas.getContext("2d")!;
        const pixel = ctx.getImageData(0, y, 1, 1).data;
        const newCWColor = `rgb(${pixel[0]}, ${pixel[1]}, ${pixel[2]})`;
        setCwColor(newCWColor);
        localStorage.setItem("selectedColor", newCWColor);
        if (markerC.current) {
            markerC.current.style.top = `${y - 5}px`;
            markerC.current.style.backgroundColor = newCWColor;
        }
    };

    const handleColorColumnMove = (event: MouseEvent) => {
        if (!hold || !colorColumnRef.current) return;
        const canvas = colorColumnRef.current;
        const rect = canvas.getBoundingClientRect();
        const y = event.clientY - rect.top;
        if (y < 0 || y >= canvas.height) return;

        const ctx = canvas.getContext("2d")!;
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

        wheelCanvas?.addEventListener("mousedown", handleMouseDown);
        columnCanvas?.addEventListener("mousedown", handleMouseDown);

        window.addEventListener("mousemove", handleColorWheelMove);
        window.addEventListener("mousemove", handleColorColumnMove);

        wheelCanvas?.addEventListener("click", handleColorWheelClick);
        columnCanvas?.addEventListener("click", handleColorColumnClick);

        return () => {
            wheelCanvas?.removeEventListener("mousedown", handleMouseDown);
            columnCanvas?.removeEventListener("mousedown", handleMouseDown);
            window.removeEventListener("mousemove", handleColorWheelMove);
            window.removeEventListener("mousemove", handleColorColumnMove);
            wheelCanvas?.removeEventListener("click", handleColorWheelClick);
            columnCanvas?.removeEventListener("click", handleColorColumnClick);
        };
    }, [hold]);

    return { colorWheelRef, colorColumnRef, RGBColor, cwColor, hold, setHold, markerCW, markerC };
}
