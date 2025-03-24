export interface post {
    id: number;
    imageId?: number;
    text: string;
    date: Date;
    user: { username: string; id: number};
    comments: comment[];
    tags: string[];
    likes: number
}

export interface comment {
    /**
     * kommnet vagy wiki
     */
    id: number;
    text: string;
    date: Date;
    user: { username: string; id: number };
}

export interface user {
    id: number;
    username: string;
    email: string;
}

export interface image {
    id: number;
    date: Date;
    visibility?: visibility;
}

export enum visibility{
    public, private
}

export interface modalProp {
    show: boolean;
    onHide: () => void;
  }