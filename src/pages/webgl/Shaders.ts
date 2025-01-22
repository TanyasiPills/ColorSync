export const vertexShaderSource = `
  attribute vec2 aVertexPosition;
  uniform vec2 uScalingFactor;
  void main() {
    gl_Position = vec4(aVertexPosition * uScalingFactor, 0.0, 1.0);
  }
`;

export const fragmentShaderSource = `
 precision mediump float;
  uniform vec3 myColor;
  void main() {
    gl_FragColor = vec4(myColor, 1.0);
  }
`;
