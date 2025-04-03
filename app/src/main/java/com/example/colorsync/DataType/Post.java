package com.example.colorsync.DataType;

import java.util.Date;
import java.util.List;

public class Post {
    private int id;
    private Integer imageId;
    private User user;
    private String text;
    private List<Comment> comments;
    private int likes;
    private Date date;
    private boolean liked;
    private List<String> tags;

    public Post(int id, int imageId, User user, String text, List<Comment> comments, int likes, Date date, boolean liked, List<String> tags) {
        this.id = id;
        this.imageId = imageId;
        this.user = user;
        this.text = text;
        this.comments = comments;
        this.likes = likes;
        this.date = date;
        this.liked = liked;
        this.tags = tags;
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

    public int getLikes() {
        return likes;
    }

    public void setLikes(int likes) {
        this.likes = likes;
    }

    public Date getDate() {
        return date;
    }

    public void setDate(Date date) {
        this.date = date;
    }

    public boolean isLiked() {
        return liked;
    }

    public void setLiked(boolean liked) {
        this.liked = liked;
    }

    public List<String> getTags() {
        return tags;
    }

    public void setTags(List<String> tags) {
        this.tags = tags;
    }
}
