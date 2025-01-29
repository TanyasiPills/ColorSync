import { VertexBufferLayout } from "../VertxBufferLayout";

export class VertexArray {
    //private layout:VertexBufferLayout | undefined;

    constructor(gl: WebGLRenderingContext){
        
    }
}
/*
export function VertexArrayFunction(raidus: number, side: number, x: number, y: number) {
    
    let verticies: number[] = [];
    let fok = Math.PI * 2 / side;
    let fokNow = 0;
    for (let index = 0; index < side; index++) {
        let xPose = raidus * Math.cos(fokNow) + x;
        let yPose = raidus * Math.sin(fokNow) + y;
        verticies.push(xPose, yPose);
        fokNow += fok;
    }
    return verticies;
}*/