package com.example.colorsync.DataTypes;

public class PostCreate {
    public String text;
    public Integer imageId;
    public String[] tags;

    public PostCreate(String text) {
        this.text = text;
        this.imageId = null;
        this.tags = null;
    }

    public PostCreate(String text, Integer imageId) {
        this.text = text;
        this.imageId = imageId;
        this.tags = null;
    }

    public PostCreate(String text, String[] tags) {
        this.text = text;
        this.tags = tags;
        this.imageId = null;
    }

    public PostCreate(String text, Integer imageId, String[] tags) {
        this.text = text;
        this.tags = tags;
        this.imageId = imageId;
    }
}
