package com.example.colorsync.DataTypes;

import java.util.List;

public class PostCreate {
    public String text;
    public Integer imageId;
    public List<String> tags;
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

    public PostCreate(String text, List<String> tags) {
        this.text = text;
        this.tags = tags;
        this.imageId = null;
        this.forcePost = false;
    }

    public PostCreate(String text, Integer imageId, List<String> tags) {
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
