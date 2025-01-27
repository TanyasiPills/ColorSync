export class User {
  public id: number;
  public username: string;
  public email: string;
  public profile_picture: string;

  public constructor(id: number, username: string, email: string, profile_picture: string) {
    this.id = id;
    this.username = username;
    this.email = email;
    this.profile_picture = profile_picture;
  }
}
