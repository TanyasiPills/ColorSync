package com.example.colorsync.DataType;

public class User {
    private int id;
    private String username;
    private String profile_description;
    private String email;

    public User(int id, String username) {
        this.id = id;
        this.username = username;
    }

    public User(int id, String username, String email, String profile_description) {
        this.id = id;
        this.username = username;
        this.email = email;
        this.profile_description = profile_description;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getProfile_description() {
        return profile_description;
    }

    public void setProfile_description(String profile_description) {
        this.profile_description = profile_description;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }
}
