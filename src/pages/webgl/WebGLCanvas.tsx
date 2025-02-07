import React, { useEffect, useRef } from "react";
import { clearCanvas, compileShader, createShaderProgram } from "./WebGLUtilites";
import { fragmentShaderSource, vertexShaderSource } from "./Shaders/CursorShader";
import "./WebGlCanvas.css"

const WebGLCanvas: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const clearButtonRef = useRef<HTMLButtonElement | null>(null);
  const colorPickerRef = useRef<HTMLInputElement | null>(null);
  const sizeRef = useRef<HTMLInputElement | null>(null);

  useEffect(() => {
    const canvas = canvasRef.current;

    if (!canvas) {
      console.error("Canvas element not found.");
      return;
    }

    const gl = canvas.getContext("webgl2");

    if (!gl) {
      alert(
        "Unable to initialize WebGL 2. Your browser or machine may not support it."
      );
      return;
    }

    var drawn: Float32Array[] = [];
    let isRunning = true;
    var mousehold: boolean = false;
    var colour: Float32Array = new Float32Array([0, 0, 0]);
    
    clearCanvas(gl);

    const vertexShader = compileShader(gl, vertexShaderSource, gl.VERTEX_SHADER);
    const fragmentShader = compileShader(gl, fragmentShaderSource, gl.FRAGMENT_SHADER);
    const shaderProgram = createShaderProgram(gl, vertexShader, fragmentShader);

    var colourLoc = gl.getUniformLocation(shaderProgram, "myColor");

    gl.useProgram(shaderProgram);


    const vertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);

    const aVertexPosition = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    const uScalingFactor = gl.getUniformLocation(shaderProgram, "uScalingFactor");

    const aspectRatio = canvas.width / canvas.height;
    gl.uniform2f(uScalingFactor, aspectRatio, 1.0);

    gl.vertexAttribPointer(aVertexPosition, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(aVertexPosition);

/*    canvas.addEventListener("mouseenter", () => {
      canvas.style.cursor = "none";
    })*/
    canvas.addEventListener("mouseleave", () => {
      canvas.style.cursor = "default";
    });


    canvas.addEventListener("mousemove", (event) => {
      
    });

    canvas.addEventListener("mousedown", (event) => {
      mousehold = true;
    });

    addEventListener("mouseup", () => {
      mousehold = false;
    });


    gl.drawArrays(gl.TRIANGLE_FAN, 0, drawn.length / 2);


    const clearStuffOnCanvas = () => {
      drawn = [];
    }

    const clearButton = clearButtonRef.current;
    if (clearButton) {
      clearButton.addEventListener("click", () => {
        clearStuffOnCanvas();
      });
    }

    return () => {
      isRunning = false;
    };
    
  }, []);



  return (
    <div className="layout">
      <div className="sideBar" id="left">
        <button ref={clearButtonRef}>Clear Canvas</button><br />
        <input ref={colorPickerRef} type="color" /><br />
        <input ref={sizeRef} type="range" min={0.01} max={1} step={0.01} defaultValue={0.1} />
        <div id="leftResize" className="rsz"></div>
      </div>
      <div className="canvasContainer">
        <canvas
          ref={canvasRef}
          id="gl-canvas"
          width={'100%'}
          height={'100%'}
        />
      </div>
      <div className="sideBar" id="right">
        
        <p>Itt majd lesz valami</p>
        <div id="rightResize" className="rsz"></div>
      </div>
    </div>
  );
};

export default WebGLCanvas;
