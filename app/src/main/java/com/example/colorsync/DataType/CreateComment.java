package com.example.colorsync.DataType;

public class CreateComment {
    public Integer postId;
    public String text;

    public CreateComment(Integer postId, String text) {
        this.postId = postId;
        this.text = text;
    }
}
