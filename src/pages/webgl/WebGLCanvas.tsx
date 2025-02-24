import React, { useEffect, useRef } from "react";
import { clearCanvas, compileShader, createShaderProgram } from "./WebGLUtilites";
import { fragmentShaderSource, vertexShaderSource } from "./Shaders/CursorShader";
import "./WebGlCanvas.css"

const WebGLCanvas: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const clearButtonRef = useRef<HTMLButtonElement | null>(null);
  const sizeRef = useRef<HTMLInputElement | null>(null);
  const colorWheelRef = useRef<HTMLCanvasElement | null>(null);
  const colorColumnRef = useRef<HTMLCanvasElement | null>(null);
  let hold = false;
  let cwColor = "rgb(255, 0, 0)";

  useEffect(() => {
    colorWheel();
    colorCollumn();
    const canvas = canvasRef.current;
    if (hold) {
      colorWheel();
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

  }, []);

  function colorWheel() {
    var colorCanvas = colorWheelRef.current;
    var ColorCtx = colorCanvas!.getContext("2d")!;
    var marker = document.getElementById("marker")!;

    let gradientH = ColorCtx.createLinearGradient(0, 0, ColorCtx.canvas.width, 0);
    gradientH.addColorStop(0, "#fff");
    gradientH.addColorStop(1, cwColor);
    ColorCtx.fillStyle = gradientH;
    ColorCtx.fillRect(0, 0, ColorCtx.canvas.width, ColorCtx.canvas.height);

    let gradientV = ColorCtx.createLinearGradient(0, 0, 0, colorCanvas!.height);
    gradientV.addColorStop(0, "rgba(0,0,0,0)");
    gradientV.addColorStop(1, "#000");
    ColorCtx.fillStyle = gradientV;
    ColorCtx.fillRect(0, 0, ColorCtx.canvas.width, ColorCtx.canvas.height);

    colorCanvas!.addEventListener("mousedown", function (event) {
      hold = true;
      let x = 0;
      let y = 0;
      if (hold) {
        let rect = colorCanvas!.getBoundingClientRect();
        x = event.clientX - rect.left;
        y = event.clientY - rect.top;
      }
      if (x >= 0 && x < colorCanvas!.width && y >= 0 && y < colorCanvas!.height) {
        let pixel = ColorCtx.getImageData(x, y, 1, 1).data;
        let rgb = `rgb(${pixel[0]}, ${pixel[1]}, ${pixel[2]})`;

        marker.style.left = x + "px";
        marker.style.top = y + "px";
        marker.style.backgroundColor = rgb;

        let brightness = (pixel[0] * 0.299) + (pixel[1] * 0.587) + (pixel[2] * 0.114);
        marker.style.borderColor = brightness < 28 ? "white" : "black";

        localStorage.setItem("selectedColor", rgb);
        localStorage.setItem("markerX", x.toString());
        localStorage.setItem("markerY", y.toString());
      }
    });

    window.addEventListener("load", () => {
      let savedX = localStorage.getItem("markerX");
      let savedY = localStorage.getItem("markerY");
      let savedColor = localStorage.getItem("selectedColor");

      if (savedX && savedY && savedColor) {
        let x = parseFloat(savedX);
        let y = parseFloat(savedY);

        marker.style.left = x + "px";
        marker.style.top = y + "px";
        marker.style.backgroundColor = savedColor;
      }
    });
  }

  function colorCollumn() {
    var colorCanvas = colorColumnRef.current;
    var ColorCtx = colorCanvas!.getContext("2d")!;
    var marker = document.getElementById("markerCol")!;

    var colors = [
      "rgb(255, 0, 0)",      
      "rgb(255, 165, 0)",    
      "rgb(255, 255, 0)",    
      "rgb(0, 255, 0)",      
      "rgb(0, 255, 255)",    
      "rgb(0, 0, 255)",      
      "rgb(255, 0, 255)"     
    ];

    let gradientV = ColorCtx.createLinearGradient(0, 0, 0, ColorCtx.canvas.height);

    let stopIncrement = 1 / (colors.length - 1);
    colors.forEach((color, index) => {
      gradientV.addColorStop(index * stopIncrement, color);
    });

    ColorCtx.fillStyle = gradientV;
    ColorCtx.fillRect(0, 0, ColorCtx.canvas.width, ColorCtx.canvas.height);

    colorCanvas!.addEventListener("mousedown", function (event){
      hold = true;
      let x = 0;
      let y = 0;
      if (hold) {
        let rect = colorCanvas!.getBoundingClientRect();
        y = event.clientY - rect.top;
      }
      if (y >= 0 && y < colorCanvas!.height) {
        let pixel = ColorCtx.getImageData(x, y, 1, 1).data;
        let rgb = `rgb(${pixel[0]}, ${pixel[1]}, ${pixel[2]})`;
        marker.style.backgroundColor = rgb;
        cwColor = rgb;

        marker.style.top = y + "px";

        localStorage.setItem("selectedColor", rgb);
        localStorage.setItem("markerY", y.toString());
      }
    });
  }

  return (
    <div className="layout">
      <div className="sideBar" id="left">
        <div id="Color">
          <div className="colorDiv">
            <canvas ref={colorWheelRef} width="150px" height="150px" id="colorWheel" />
            <div id="marker" />
          </div>
          <div className="colorDiv">
            <canvas ref={colorColumnRef} width="50px" height="150px" id="colorColumns" />
            <div id="markerCol" />
          </div>
        </div>
        <input ref={sizeRef} type="range" min={0.01} max={1} step={0.01} defaultValue={0.1} />
        <div id="leftResize" className="rsz"></div>
      </div>
      <div className="canvasContainer">
        <canvas ref={canvasRef} id="gl-canvas" />
      </div>
      <div className="sideBar" id="right">

        <p>Itt majd lesz valami</p>
        <div id="rightResize" className="rsz"></div>
      </div>
    </div>
  );
};

export default WebGLCanvas;
