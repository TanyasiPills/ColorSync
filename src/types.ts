export interface post {
    id: number;
    userId: number;
    imageId?: number;
    text: string;
    date: Date;
    user: { username: string; };
    comments: comment[];
}

export interface comment {
    id: number;
    userId: number;
    text: string;
    date: Date;
    user: { username: string; };
}

export interface user {
    id: number;
    username: string;
    email?: string;
}

export interface image {
    id: number;
    date: Date;
    visibility?: visibility;
}

export enum visibility{
    public, private
}