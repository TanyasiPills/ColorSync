export class User {
  public id: number;
  public username: string;
  public email: string;
  public profile_picture: string;

  constructor(id: number, username: string, email: string, profile_picture: string) {
    this.id = id;
    this.username = username;
    this.email = email;
    this.profile_picture = profile_picture;
  }
}

export type Action = {
  type: string;
  data: any;
}

export class ActionList<T> {
  private size: number;
  private items: T[];

  constructor(size: number) {
    this.size = size;
    this.items = [];
  }

  public add(item: T): void {
    this.items.push(item);

    if (this.items.length > this.size) {
      this.items.shift();
    }
  }

  public pop() {
    return this.items.pop();
  }

  public getList() {
    return this.items;
  }

  public clear() {
    this.items.slice(0, this.items.length);
  }
}

export class History {
  private ids: number[];
  private actions: Action[];

  constructor() {
    this.ids = [];
    this.actions = [];
  }

  public add(userId: number, action: Action) {
    this.ids.push(userId);
    this.actions.push(action);
  }

  public getActions() {
    return this.actions;
  }

  public getIds() {
    return this.ids;
  }

  public undo(id: number) {
    //WIP
  }




}
