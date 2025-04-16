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
  type: number;
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
  private undoLimit: number;
  private undoPosition: number;

  private redoActions : Map<number, ActionList<Action>>;

  constructor(size: number) {
    this.ids = [];
    this.actions = [];
    this.undoLimit = size;
    this.undoPosition = -1;

    this.redoActions = new Map<number, ActionList<Action>>();
  }

  public connect(id: number): void {
    this.redoActions.set(id, new ActionList<Action>(this.undoLimit));
  }

  public disconnect(id: number) : void {
    this.redoActions.delete(id);
  }

  public add(id: number, action: Action): void {
    this.ids.push(id);
    this.actions.push(action);
  }

  public getActions(): Action[] {
    return this.actions;
  }

  public getIds(): number[] {
    return this.ids;
  }

  public getUndoPosition() {
    return this.undoPosition;
  }

  public undo(id: number): {history: Action[], compilePostition: number} {
    let index = undefined;
    const count = new Map<number, number>();
    let last;
    for (let i = this.actions.length - 1; i >= Math.max(Math.max(this.actions.length - this.undoLimit * 10, 0), this.undoPosition + 1); i--) {
      if (index === undefined && this.ids[i] === id) index = i;
      if (count[i] >= this.undoLimit) continue;
      else {
        count[i] = (count[i] || 0) + 1;
        last = i;
      }
    }
    if (index === undefined) return null;
    for (let value of count.values()) {
      if (value < 30) {
        last = Math.max(this.actions.length - this.undoLimit * 10 - 1, 0);
        break;
      }
    }
    last--;

    this.redoActions.get(id).add(this.actions[index]);

    this.ids.splice(index, 1);
    this.actions.splice(index, 1);

    let result = this.actions.slice(this.undoPosition + 1);
    let compilePostition = last - this.undoPosition;
    return {history: result, compilePostition};
  }

  public redo(id: number): Action {
    const action = this.redoActions[id].pop();
    if (action === undefined) return undefined;
    this.add(id, action);
    return action;
  }

  public clear(id: number): void {
    this.redoActions[id].clear();
  }
}
