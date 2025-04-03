package com.example.colorsync.DataType;

public class Comment {
    private int id;
    private String text;
    private String date;
    private User user;

    public Comment(int id, String text, String date, User user) {
        this.id = id;
        this.text = text;
        this.date = date;
        this.user = user;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }

    public String getDate() {
        return date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }
}
