#version 430

layout(rgba8, binding = 0) uniform image2D img; // The texture
layout(std430, binding = 1) buffer ActivePixels { ivec2 activePixels[]; }; // Queue of pixels to process
layout(std430, binding = 2) buffer Visited { bool visited[]; }; // Track filled pixels

uniform vec4 fillColor;
uniform vec4 targetColor;
uniform int width;
uniform int height;
uniform int activeCount; // Number of active pixels in queue

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= activeCount) return; // Out of bounds

    ivec2 pos = activePixels[id];
    if (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) return;

    int index = pos.y * width + pos.x;
    if (visited[index]) return;

    vec4 pixel = imageLoad(img, pos);
    if (pixel != targetColor) return;

    // Fill pixel
    imageStore(img, pos, fillColor);
    visited[index] = true;

    // Add neighbors to activePixels buffer (next pass)
    int offset = id * 4;
    activePixels[offset] = ivec2(pos.x + 1, pos.y);
    activePixels[offset + 1] = ivec2(pos.x - 1, pos.y);
    activePixels[offset + 2] = ivec2(pos.x, pos.y + 1);
    activePixels[offset + 3] = ivec2(pos.x, pos.y - 1);
}