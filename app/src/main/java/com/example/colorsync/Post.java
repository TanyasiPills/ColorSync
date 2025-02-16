package com.example.colorsync;

import java.util.List;

public class Post {
    private int id;
    private Integer imageId;
    private User user;
    private String text;
    private List<Comment> comments;

    public Post(int id, int imageId, User user, String text, List<Comment> comments) {
        this.id = id;
        this.imageId = imageId;
        this.user = user;
        this.text = text;
        this.comments = comments;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public Integer getImageId() {
        return imageId;
    }

    public void setImageId(Integer imageId) {
        this.imageId = imageId;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }

    public List<Comment> getComments() {
        return comments;
    }

    public void setComments(List<Comment> comments) {
        this.comments = comments;
    }
}
