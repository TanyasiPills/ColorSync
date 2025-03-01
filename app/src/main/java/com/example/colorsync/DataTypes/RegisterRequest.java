package com.example.colorsync.DataTypes;

public class RegisterRequest {
    public String email;
    public String password;
    public String username;

    public RegisterRequest( String email, String password, String username) {
        this.password = password;
        this.email = email;
        this.username = username;
    }
}
