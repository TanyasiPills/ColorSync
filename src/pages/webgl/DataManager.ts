import * as fs from 'fs'
import { Folder, Layer, Render, RenderData } from './Render'

export interface FolderSave {
    name: string
    path: string
    visible?: boolean
    editing?: boolean
    selected?: boolean
    id?: number
    opacity?: number
    open?: boolean
    childrenIds?: number[]
}

export interface Sync {
    canvasWidth: number
    canvasHeight: number
    layerLength: number
    layers: Layer[]
    layerTextures: Uint8Array[]
    folderLength: number
    folders: Folder[]
}

export interface Re {
    layers: number[]
    folders: number[]
    nodes: Map<number, any>
    inited: boolean
    getCanvasSize(): [number, number]
    setDrawData(width: number, height: number): void
}

let sync: Sync = { canvasWidth: 0, canvasHeight: 0, layerLength: 0, layers: [], layerTextures: [], folderLength: 0, folders: [] }
let renderer: Render | null = null
let textures: number[] = []

export const NewDraw = {
    initLayer(pixels: Uint8Array): RenderData {
        return {} as RenderData
    }
}

export const NewRendererAPI = {
    getTexturePixels(id: number, width: number, height: number): Uint8Array {
        return new Uint8Array(width * height * 4)
    }
}

export class DataManager {
    static setRenderer(rendererIn: Render): void {
        renderer = rendererIn
    }

    static saveSyncData(path: string): void {
        try {
            fs.unlinkSync(path)
        } catch (e) { }
        DataManager.setSyncData()
        DataManager.saveSync(path)
    }

    static loadSyncData(path: string): void {
        DataManager.loadSync(path)
        DataManager.setRenderData()
        if (renderer) renderer.inited = true
    }

    static writeFolder(folders: FolderSave[]): Buffer {
        let buffers: Buffer[] = []
        folders.forEach(folder => {
            const nameBuf = Buffer.from(folder.name)
            const pathBuf = Buffer.from(folder.path)
            const nameSizeBuf = Buffer.alloc(4)
            const pathSizeBuf = Buffer.alloc(4)
            nameSizeBuf.writeInt32LE(nameBuf.length, 0)
            pathSizeBuf.writeInt32LE(pathBuf.length, 0)
            buffers.push(nameSizeBuf, pathSizeBuf, nameBuf, pathBuf)
        })
        return Buffer.concat(buffers)
    }

    static readFolder(buf: Buffer, offset: number, count: number): { folders: FolderSave[]; offset: number } {
        let folders: FolderSave[] = []
        for (let i = 0; i < count; i++) {
            let nameSize = buf.readInt32LE(offset); offset += 4
            let pathSize = buf.readInt32LE(offset); offset += 4
            let name = buf.slice(offset, offset + nameSize).toString(); offset += nameSize
            let path = buf.slice(offset, offset + pathSize).toString(); offset += pathSize
            folders.push({ name, path })
        }
        return { folders, offset }
    }

    static setSyncData(): void {
        textures.forEach(t => { })
        textures = []
        sync.layers = []
        sync.folders = []
        sync.layerTextures = []
        if (!renderer) return
        let sizes = renderer.getCanvasSize()
        sync.canvasWidth = sizes[0]
        sync.canvasHeight = sizes[1]
        sync.layerLength = renderer.layers.length
        renderer.layers.forEach(item => {
            let toAdd = renderer!.nodes.get(item) as Layer
            if (toAdd) {
                sync.layers.push({ ...toAdd })
                let id = toAdd.id
                let pixels = NewRendererAPI.getTexturePixels(id, sync.canvasWidth, sync.canvasHeight)
                sync.layerTextures.push(pixels)
            }
        })
        sync.folderLength = renderer.folders.length
        renderer.folders.forEach(item => {
            let toAdd = renderer!.nodes.get(item) as Folder
            if (toAdd) {
                const folderClone = Object.assign(new Folder(toAdd.name, toAdd.id), toAdd)
                sync.folders.push(folderClone)
            }
        })
    }


    static checkAndDeleteFile(filePath: string): void {
        if (fs.existsSync(filePath) && fs.lstatSync(filePath).isFile()) {
            try {
                fs.unlinkSync(filePath)
            } catch (e) { }
        }
    }

    static saveSync(path: string): void {
        let buffers: Buffer[] = []
        let buf = Buffer.alloc(4)
        buf.writeInt32LE(sync.canvasWidth, 0)
        buffers.push(buf)
        buf = Buffer.alloc(4)
        buf.writeInt32LE(sync.canvasHeight, 0)
        buffers.push(buf)
        buf = Buffer.alloc(4)
        buf.writeInt32LE(sync.layerLength, 0)
        buffers.push(buf)
        sync.layers.forEach(layer => {
            let nameBuf = Buffer.from(layer.name)
            buf = Buffer.alloc(4)
            buf.writeInt32LE(nameBuf.length, 0)
            buffers.push(buf, nameBuf)
            buf = Buffer.alloc(1)
            buf.writeUInt8(layer.visible ? 1 : 0, 0)
            buffers.push(buf)
            buf = Buffer.alloc(1)
            buf.writeUInt8(layer.editing ? 1 : 0, 0)
            buffers.push(buf)
            buf = Buffer.alloc(1)
            buf.writeUInt8(layer.selected ? 1 : 0, 0)
            buffers.push(buf)
            buf = Buffer.alloc(4)
            buf.writeInt32LE(layer.id, 0)
            buffers.push(buf)
            let opacityBuf = Buffer.alloc(4)
            opacityBuf.writeFloatLE(layer.opacity, 0)
            buffers.push(opacityBuf)
        })
        sync.layerTextures.forEach(pixels => {
            buffers.push(Buffer.from(pixels))
        })
        buf = Buffer.alloc(4)
        buf.writeInt32LE(sync.folderLength, 0)
        buffers.push(buf)
        sync.folders.forEach(folder => {
            let nameBuf = Buffer.from(folder.name)
            buf = Buffer.alloc(4)
            buf.writeInt32LE(nameBuf.length, 0)
            buffers.push(buf, nameBuf)
            buf = Buffer.alloc(1)
            buf.writeUInt8(folder.visible ? 1 : 0, 0)
            buffers.push(buf)
            buf = Buffer.alloc(1)
            buf.writeUInt8(folder.editing ? 1 : 0, 0)
            buffers.push(buf)
            buf = Buffer.alloc(1)
            buf.writeUInt8(folder.selected ? 1 : 0, 0)
            buffers.push(buf)
            buf = Buffer.alloc(4)
            buf.writeInt32LE(folder.id, 0)
            buffers.push(buf)
            let opacityBuf = Buffer.alloc(4)
            opacityBuf.writeFloatLE(folder.opacity, 0)
            buffers.push(opacityBuf)
            buf = Buffer.alloc(1)
            buf.writeUInt8(folder.open ? 1 : 0, 0)
            buffers.push(buf)
            buf = Buffer.alloc(4)
            let childrenSize = folder.childrenIds ? folder.childrenIds.length : 0
            buf.writeInt32LE(childrenSize, 0)
            buffers.push(buf)
            if (folder.childrenIds) {
                folder.childrenIds.forEach(id => {
                    buf = Buffer.alloc(4)
                    buf.writeInt32LE(id, 0)
                    buffers.push(buf)
                })
            }
        })
        fs.writeFileSync(path, Buffer.concat(buffers))
    }

    static loadSync(path: string): void {
        try {
            let buf = fs.readFileSync(path)
            let offset = 0
            sync.canvasWidth = buf.readInt32LE(offset); offset += 4
            sync.canvasHeight = buf.readInt32LE(offset); offset += 4
            let layerSize = buf.readInt32LE(offset); offset += 4
            sync.layerLength = layerSize
            sync.layers = []
            for (let i = 0; i < layerSize; i++) {
                let nameLength = buf.readInt32LE(offset); offset += 4
                let name = buf.slice(offset, offset + nameLength).toString(); offset += nameLength
                let visible = buf.readUInt8(offset) === 1; offset += 1
                let editing = buf.readUInt8(offset) === 1; offset += 1
                let selected = buf.readUInt8(offset) === 1; offset += 1
                let id = buf.readInt32LE(offset); offset += 4
                let opacity = buf.readFloatLE(offset); offset += 4
                sync.layers.push({
                    name, visible, editing, selected, id, opacity,
                    data: {} as RenderData
                })
            }
            sync.layerTextures = []
            let pixelSize = sync.canvasWidth * sync.canvasHeight * 4
            for (let i = 0; i < layerSize; i++) {
                let pixels = buf.slice(offset, offset + pixelSize)
                offset += pixelSize
                sync.layerTextures.push(new Uint8Array(pixels))
            }
            let folderSize = buf.readInt32LE(offset); offset += 4
            sync.folderLength = folderSize
            sync.folders = []
            for (let i = 0; i < folderSize; i++) {
                let nameLength = buf.readInt32LE(offset); offset += 4
                let name = buf.slice(offset, offset + nameLength).toString(); offset += nameLength
                let visible = buf.readUInt8(offset) === 1; offset += 1
                let editing = buf.readUInt8(offset) === 1; offset += 1
                let selected = buf.readUInt8(offset) === 1; offset += 1
                let id = buf.readInt32LE(offset); offset += 4
                let opacity = buf.readFloatLE(offset); offset += 4
                let open = buf.readUInt8(offset) === 1; offset += 1
                let childrenSize = buf.readInt32LE(offset); offset += 4
                let childrenIds: number[] = []
                for (let j = 0; j < childrenSize; j++) {
                    childrenIds.push(buf.readInt32LE(offset))
                    offset += 4
                }
                let folderInst = new Folder(name, id)
                folderInst.visible = visible
                folderInst.editing = editing
                folderInst.selected = selected
                folderInst.opacity = opacity
                folderInst.open = open
                folderInst.childrenIds = childrenIds
                sync.folders.push(folderInst)
            }
        } catch (e) { }
    }

    static setRenderData(): void {
        if (!renderer) return
        renderer.nodes.clear()
        renderer.layers = []
        renderer.folders = []
        renderer.setDrawData(sync.canvasWidth, sync.canvasHeight)
        let index = 0
        sync.layers.forEach(layer => {
            let layerData: RenderData = NewDraw.initLayer(sync.layerTextures[index])
            let newLayer = new Layer(layer.name, layer.id, layerData)
            newLayer.visible = layer.visible
            newLayer.editing = layer.editing
            newLayer.selected = layer.selected
            newLayer.opacity = layer.opacity
            renderer!.nodes.set(layer.id, newLayer)
            renderer!.layers.push(layer.id)
            index++
        })

        sync.folders.forEach(folder => {
            let newFolder = new Folder(folder.name, folder.id)
            newFolder.visible = folder.visible
            newFolder.editing = folder.editing
            newFolder.selected = folder.selected
            newFolder.opacity = folder.opacity
            newFolder.open = folder.open
            newFolder.childrenIds = folder.childrenIds
            renderer!.nodes.set(folder.id, newFolder)
            let folderNode: Folder = renderer!.nodes.get(folder.id) as Folder
            if (folderNode && folder.childrenIds) {
                folder.childrenIds.forEach(child => {
                    folderNode.addChild(child);
                })
            }
            renderer!.folders.push(folder.id)
        })
    }

}

export const Explorer = {
    getFavorites(): FolderSave[] {
        return []
    },
    getRecents(): FolderSave[] {
        return []
    },
    setFavorites(favs: FolderSave[]): void { },
    setRecents(recents: FolderSave[]): void { }
}

export const RuntimeData = (function () {
    let instance: { username: string, token: string, ip: string, password: string } | null = null
    return {
        getInstance: function () {
            if (!instance) {
                instance = { username: '', token: '', ip: '', password: '' }
            }
            return instance
        }
    }
})()
