import React, { useEffect, useRef, useState } from "react";
import { clearCanvas, compileShader, createShaderProgram } from "./WebGLUtilites";
import { fragmentShaderSource, vertexShaderSource } from "./Shaders/CursorShader";
import "./WebGlCanvas.css"
import { useColorPicker } from "./CallBack";

const WebGLCanvas: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const clearButtonRef = useRef<HTMLButtonElement | null>(null);
  const sizeRef = useRef<HTMLInputElement | null>(null);
  const { colorWheelRef, colorColumnRef, RGBColor, cwColor, markerCW, markerC } = useColorPicker();
  const canvas = canvasRef.current;

  useEffect(() => {
    drawColorWheel();
    drawColorColumn();
    const curCol = document.getElementById("currentColor");
    if (curCol) {
      curCol.style.backgroundColor = RGBColor;
    }

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

    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    clearCanvas(gl);

    /*var drawn: Float32Array[] = [];
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

      canvas.addEventListener("mouseenter", () => {
      canvas.style.cursor = "none";
    })*/
    canvas.addEventListener("mouseleave", () => {
      canvas.style.cursor = "default";
    });


    canvas.addEventListener("mousemove", (event) => {

    });

    canvas.addEventListener("mousedown", (event) => {
      //mousehold = true;
    });

    addEventListener("mouseup", () => {
      //mousehold = false;
    });


    //gl.drawArrays(gl.TRIANGLE_FAN, 0, drawn.length / 2);


    const clearStuffOnCanvas = () => {
      //drawn = [];
    }

    const clearButton = clearButtonRef.current;
    if (clearButton) {
      clearButton.addEventListener("click", () => {
        clearStuffOnCanvas();
      });
    }

    return () => {
      //isRunning = false;
    };

  }, [cwColor, RGBColor]);

  function drawColorWheel() {
    const colorWheel = colorWheelRef.current;
    if (!colorWheel) return;
    const ctx = colorWheel.getContext("2d");
    if (!ctx) return;

    const gradientH = ctx.createLinearGradient(0, 0, colorWheel.width, 0);
    gradientH.addColorStop(0, "#fff");
    if (!cwColor) {
      gradientH.addColorStop(1, "rgb(255, 0, 0)");
    }
    else {
      gradientH.addColorStop(1, cwColor);
    }
    ctx.fillStyle = gradientH;
    ctx.fillRect(0, 0, colorWheel.width, colorWheel.height);

    const gradientV = ctx.createLinearGradient(0, 0, 0, colorWheel.height);
    gradientV.addColorStop(0, "rgba(0,0,0,0)");
    gradientV.addColorStop(1, "#000");
    ctx.fillStyle = gradientV;
    ctx.fillRect(0, 0, colorWheel.width, colorWheel.height);
  }

  function drawColorColumn() {
    const colorColumn = colorColumnRef.current;
    if (!colorColumn) return;
    const ctx = colorColumn.getContext("2d");
    if (!ctx) return;

    const colors = ["rgb(255,0,0)", "rgb(255,165,0)", "rgb(255,255,0)", "rgb(0,255,0)", "rgb(0,255,255)", "rgb(0,0,255)", "rgb(255,0,255)"];
    const gradientV = ctx.createLinearGradient(0, 0, 0, colorColumn.height);
    colors.forEach((color, index) => {
      gradientV.addColorStop(index / (colors.length - 1), color);
    });

    ctx.fillStyle = gradientV;
    ctx.fillRect(0, 0, colorColumn.width, colorColumn.height);
  }

  useEffect(() => {
    drawColorWheel();
    drawColorColumn();
  }, [cwColor]);

  return (
    <div className="layout">
      <div className="sideBar" id="left"> {/*not resizbale */}
        <div className="topRow"> {/*not resizbale */}
          <div id="Color">
            <div id="colorWheelDiv">
              <canvas ref={colorWheelRef} id="colorWheel" width="150px" height="150px" />
              <div id="marker" ref={markerCW} />
            </div>
            <div id="colorColDiv">
              <canvas ref={colorColumnRef} id="colorColumns" width="30px" height="150px" />
              <div id="markerCol" ref={markerC} />
            </div>
          </div>
          <div id="currentColor" />
        </div>
        <div className="secondRow">
        <input ref={sizeRef} type="range" min={0.01} max={1} step={0.01} defaultValue={0.1} /> {/*resizable height wise*/}
        </div>
        <div className="thirdRow">{/*height wise resizable*/}
          <p>itt majd még brush sizes</p>
        </div>
      </div>
      <div className="canvasContainer">
        <canvas ref={canvasRef} id="gl-canvas" />
      </div>
      <div className="sideBar" id="right"> {/*resizable width wise*/}
        {/*inside it resizbale height wise*/}
        <p>Itt majd lesz valami</p>
      </div>
    </div>
  );
};

export default WebGLCanvas;
