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
    const leftResize = document.getElementById("leftResize");
    const rightResize = document.getElementById("rightResize");

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
    let size = parseFloat(sizeRef.current?.value || "0.1");

    let wd = 100;
    let x: number, dx: number;

    const startResize = (evt: { screenX: number }) => {
      x = evt.screenX;
    };

    const resize = (evt: { screenX: number }, side: 'left' | 'right') => {
      dx = evt.screenX - x;
      x = evt.screenX;

      if (side === 'left') {
        wd += dx;
        const main = document.querySelector("#sideBar#left") as HTMLElement;
        main.style.width = wd + "px";
      } else if (side === 'right') {
        const mainRight = document.querySelector("#sideBar#right") as HTMLElement;
        mainRight.style.width = (wd + dx) + "px";
      }
    };

    leftResize?.addEventListener("mousedown", (evt) => {
      startResize(evt);
      document.body.addEventListener("mousemove", (e) => resize(e, 'left'));
      document.body.addEventListener("mouseup", () => {
        document.body.removeEventListener("mousemove", (e) => resize(e, 'left'));
      });
    });

    rightResize?.addEventListener("mousedown", (evt) => {
      startResize(evt);
      document.body.addEventListener("mousemove", (e) => resize(e, 'right'));
      document.body.addEventListener("mouseup", () => {
        document.body.removeEventListener("mousemove", (e) => resize(e, 'right'));
      });
    });

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
    })
    canvas.addEventListener("mouseleave", () => {
      canvas.style.cursor = "default";
    });

    var circleVertices = new Float32Array();

    canvas.addEventListener("mousemove", (event) => {
      const rect = canvas.getBoundingClientRect();
      const centerX = ((event.clientX - rect.left) / canvas.width) * 2 - 1;
      const centerY = 1 - ((event.clientY - rect.top) / canvas.height) * 2;

      
    });


    const render = () => {
      if (!isRunning) return;


      clearCanvas(gl);

      for (let index = 0; index < drawn.length; index++) {
        gl.uniform3f(colourLoc, drawn[index][0], drawn[index][1], drawn[index][2]);
        var drawnElement = drawn[index].slice(3);
        gl.bufferData(gl.ARRAY_BUFFER, drawnElement, gl.DYNAMIC_DRAW);
        gl.drawArrays(gl.TRIANGLE_FAN, 0, drawnElement.length / 2);

      }
      var cursorToRender = circleVertices.slice(3);
      gl.bufferData(gl.ARRAY_BUFFER, cursorToRender, gl.STATIC_DRAW);
      gl.drawArrays(gl.TRIANGLE_FAN, 0, cursorToRender.length / 2);


      requestAnimationFrame(render);
    };

    canvas.addEventListener("mousedown", (event) => {
      const rect = canvas.getBoundingClientRect();
      const centerX = ((event.clientX - rect.left) / canvas.width) * 2 - 1;
      const centerY = 1 - ((event.clientY - rect.top) / canvas.height) * 2;

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

    const colorPicker = colorPickerRef.current;
    colorPicker?.addEventListener("change", () => {
      let hex = colorPicker.value;
      hex = hex.replace(/^#/, '');

      const bigInt: number = parseInt(hex, 16)
      const r = ((bigInt >> 16) & 255) / 255;
      const g = ((bigInt >> 8) & 255) / 255;
      const b = (bigInt & 255) / 255;

      colour = new Float32Array([r, g, b]);
    });

    const sizeInput = sizeRef.current;
    sizeInput?.addEventListener("input", () => {
      size = parseFloat(sizeInput.value);
    });

    render();

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
          width={700}
          height={700}
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
