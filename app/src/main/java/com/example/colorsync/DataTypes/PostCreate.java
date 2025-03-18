package com.example.colorsync.DataTypes;

public class PostCreate {
    public String text;
    public Integer imageId;
    public String[] tags;
    public boolean forcePost;

    public PostCreate(String text) {
        this.text = text;
        this.imageId = null;
        this.tags = null;
        this.forcePost = false;
    }

    public PostCreate(String text, Integer imageId) {
        this.text = text;
        this.imageId = imageId;
        this.tags = null;
        this.forcePost = false;
    }

    public PostCreate(String text, String[] tags) {
        this.text = text;
        this.tags = tags;
        this.imageId = null;
        this.forcePost = false;
    }

    public PostCreate(String text, Integer imageId, String[] tags) {
        this.text = text;
        this.tags = tags;
        this.imageId = imageId;
        this.forcePost = false;
    }

    public PostCreate forcePost() {
        this.forcePost = true;
        return this;
    }
}
