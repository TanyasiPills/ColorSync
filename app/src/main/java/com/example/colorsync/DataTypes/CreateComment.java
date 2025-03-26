package com.example.colorsync.DataTypes;

public class CreateComment {
    public Integer postId;
    public String text;

    public CreateComment(Integer postId, String text) {
        this.postId = postId;
        this.text = text;
    }
}
