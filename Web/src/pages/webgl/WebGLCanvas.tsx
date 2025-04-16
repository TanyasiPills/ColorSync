import React, { useEffect, useRef, useState } from "react";
import { Panel, PanelGroup, PanelResizeHandle } from "react-resizable-panels";
import { EyeFill, EyeSlashFill, CaretRightFill, CaretDownFill } from "react-bootstrap-icons";
import { clearCanvas, compileShader, createShaderProgram } from "./WebGLUtilites";
import { fragmentShaderSource, vertexShaderSource } from "./Shaders/CursorShader";
import "../../css/WebGlCanvas.css";
import { useColorWheel } from "./CallBack";
import { Render } from "./Render";
import { WebGLData } from "../modals/WebGLData";
import { visibility } from "../../types";

// Define the interface for a layer (supports tree structure)
interface Layer {
  id: number;
  name: string;
  visible: boolean;
  expanded?: boolean;
  children?: Layer[];
}

// Sample initial layers with nested children (tree view)
const initialLayers: Layer[] = [
  { id: 1, name: "Background", visible: true },
  {
    id: 2,
    name: "Sketch",
    visible: true,
    expanded: true,
    children: [
    ]
  },
  { id: 5, name: "Details", visible: true }
];

const circleSizes = [1, 2, 4, 6, 8, 10, 12, 14, 16];
const iconData = [
  { name: "Icon 1", label: "Home" },
  { name: "Icon 2", label: "Search" },
  { name: "Icon 3", label: "Upload" },
  { name: "Icon 4", label: "Download" },
  { name: "Icon 5", label: "Settings" },
  { name: "Icon 6", label: "Profile" }
];

// Recursive component for displaying each layer item
const LayerItem: React.FC<{
  layer: Layer;
  level: number;
  toggleVisibility: (id: number) => void;
  toggleExpand: (id: number) => void;
}> = ({ layer, level, toggleVisibility, toggleExpand }) => {
  return (
    <li className="layer-item" style={{ paddingLeft: `${level * 15}px`, display: "flex", alignItems: "center" }}>
      {layer.children && layer.children.length > 0 && (
        <span onClick={() => toggleExpand(layer.id)} style={{ cursor: "pointer", marginRight: "5px" }}>
          {layer.expanded ? <CaretDownFill /> : <CaretRightFill />}
        </span>
      )}
      <span>{layer.name}</span>
      <span onClick={() => toggleVisibility(layer.id)} style={{ marginLeft: "auto", cursor: "pointer" }}>
        {layer.visible ? <EyeFill /> : <EyeSlashFill />}
      </span>
      {layer.expanded && layer.children && layer.children.length > 0 && (
        <ul style={{ listStyle: "none", margin: 0, padding: 0, width: "100%" }}>
          {layer.children.map(child => (
            <LayerItem key={child.id} layer={child} level={level + 1} toggleVisibility={toggleVisibility} toggleExpand={toggleExpand} />
          ))}
        </ul>
      )}
    </li>
  );
};

const WebGLCanvas: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const { colorWheelRef, colorColumnRef, RGBColor, cwColor, markerCW, markerC } = useColorWheel();
  const [show, setShow] = useState(true);
  const [canvasHeight, setCanvasHeight] = useState<number>(500);
  const [canvasWidth, setCanvasWidth] = useState<number>(800);
  const [roomName, setRoomName] = useState<string>("");
  const [layers, setLayers] = useState<Layer[]>(initialLayers);
  const [size, setSize] = useState<number>(1);

  const toggleVisibility = (id: number) => {
    const updateVisibility = (nodes: Layer[]): Layer[] =>
      nodes.map(node => {
        if (node.id === id) {
          return { ...node, visible: !node.visible };
        }
        if (node.children) {
          return { ...node, children: updateVisibility(node.children) };
        }
        return node;
      });
    setLayers(prev => updateVisibility(prev));
  };

  const toggleExpand = (id: number) => {
    const updateExpand = (nodes: Layer[]): Layer[] =>
      nodes.map(node => {
        if (node.id === id) {
          return { ...node, expanded: !node.expanded };
        }
        if (node.children) {
          return { ...node, children: updateExpand(node.children) };
        }
        return node;
      });
    setLayers(prev => updateExpand(prev));
  };

  useEffect(() => {
    drawColorWheel();
    drawColorColumn();
    const canvas = canvasRef.current;
    const curCol = document.getElementById("currentColor");
    if (curCol) {
      curCol.style.backgroundColor = RGBColor;
    }
    if (!canvas) {
      console.error("Canvas element not found.");
      return;
    }
    canvas.width = canvasHeight;
    canvas.height = canvasWidth;
    const gl = canvas.getContext("webgl2");
    if (!gl) {
      alert("Unable to initialize WebGL 2. Your browser or machine may not support it.");
      return;
    }
    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    clearCanvas(gl);
    const render: Render = new Render(gl);
    render.render();
    canvas.addEventListener("mouseenter", () => {
      canvas.style.cursor = "none";
    });
    canvas.addEventListener("mouseleave", () => {
      canvas.style.cursor = "default";
    });
    return () => {
      // Optional cleanup
    };
  }, [cwColor, RGBColor, canvasHeight, canvasWidth]);

  function drawColorWheel() {
    const colorWheel = colorWheelRef.current;
    if (!colorWheel) return;
    const ctx = colorWheel.getContext("2d");
    if (!ctx) return;
    const gradientH = ctx.createLinearGradient(0, 0, colorWheel.width, 0);
    gradientH.addColorStop(0, "#fff");
    gradientH.addColorStop(1, cwColor ? cwColor : "rgb(255, 0, 0)");
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
      <div className="sideBar" id="left">
        <div className="topRow">
          <div id="Color">
            <div id="colorWheelDiv">
              <canvas ref={colorWheelRef} id="colorWheel" width="150" height="150" />
              <div id="marker" ref={markerCW} />
            </div>
            <div id="colorColDiv">
              <canvas ref={colorColumnRef} id="colorColumns" width="30" height="150" />
              <div id="markerCol" ref={markerC} />
            </div>
          </div>
          <div id="currentColor" />
        </div>
        <PanelGroup direction="vertical">
          <Panel defaultSize={50} minSize={10}>
            <input type="range" min="0.01" max="1" step="0.01" defaultValue="0.1" className="full-width-range" onChange={(e) => setSize(parseFloat(e.target.value))}/>
            <div id="cirlce-sizes">
              <div className="circle-grid">
                {circleSizes.map((size) => (
                  <div key={size} className="circle" style={{ width: size * 2 + 24, height: size * 2 + 24, lineHeight: `${size * 4}px` }} onClick={() => setSize(size)}>
                    {size}
                  </div>
              ))}
              </div>
            </div>
          </Panel>
          <PanelResizeHandle>
            <hr />
          </PanelResizeHandle>
          <Panel defaultSize={50}>
            <div className="icon-grid">
              {iconData.map((icon, idx) => (
                <div key={idx} className="icon-item">
                  <div className="icon-placeholder">
                    {icon.name}
                  </div>
                  <div className="icon-label">
                    {icon.label}
                  </div>
                </div>
              ))}
            </div>
          </Panel>
        </PanelGroup>
      </div>
      <PanelGroup direction="horizontal" style={{ flexGrow: 1 }}>
        <Panel defaultSize={80} minSize={60} className="canvasContainer">
          <canvas id="gl-canvas" ref={canvasRef} />
        </Panel>
        <PanelResizeHandle />
        <Panel defaultSize={20} id="right" className="sideBar">
          <div className="room-header">
            <h3>{roomName || "Room Name"}</h3>
          </div>
          <div className="layers-tree">
            <ul style={{ listStyle: "none", margin: 0, padding: 0 }}>
              {layers.map(layer => (
                <LayerItem key={layer.id} layer={layer} level={0} toggleVisibility={toggleVisibility} toggleExpand={toggleExpand} />
              ))}
            </ul>
          </div>
          <div className="chat-container">
            <div className="chat-area">

            </div>
            <input type="text" placeholder="Type your message…" className="chat-input" />
          </div>
        </Panel>
      </PanelGroup>
      <WebGLData
        show={show}
        onHide={() => setShow(false)}
        onData={(height, width, roomName) => {
          setCanvasHeight(height);
          setCanvasWidth(width);
          if (roomName) setRoomName(roomName);
          setShow(false);
        }}
      />
    </div>
  );
};

export default WebGLCanvas;
