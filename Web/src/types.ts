export interface post {
    id: number;
    imageId?: number;
    text: string;
    date: Date;
    user: { username: string; id: number };
    comments: comment[];
    tags: string[];
    likes: number
}

export interface comment {
    id: number;
    text: string;
    date: Date;
    user: { username: string; id: number };
}

export interface user {
    id: number;
    username: string;
    email: string;
    profile_description: string;
}

export interface image {
    id: number;
    date: Date;
    visibility?: visibility;
}

export enum visibility {
    public = "public",
    private = "private"
}
  

export interface modalProp {
    show: boolean;
    onHide: () => void;
}

export interface LeftNavbarProps {
  isOpen: boolean;
  onClose?: () => void;
  closable: boolean;
}