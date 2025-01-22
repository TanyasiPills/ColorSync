export class User {
  protected id: number;
  protected username: string;
  protected email: string;
  protected profile_picture: string;

  public constructor(id: number, username: string, email: string, profile_picture: string) {
    this.id = id;
    this.username = username;
    this.email = email;
    this.profile_picture = profile_picture;
  }

  public getId(): number {
    return this.id;
  }

  public getUsername(): string {
    return this.username;
  }

  public getEmail(): string {
    return this.email;
  }

  public getProfilePicture(): string {
    return this.profile_picture;
  }
}
