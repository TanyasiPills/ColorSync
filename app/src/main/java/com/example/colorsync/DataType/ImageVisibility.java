package com.example.colorsync.DataType;

public class ImageVisibility {
    public static final String PUBLIC = "public";
    public static final String PRIVATE = "private";

    public String visibility;
    public boolean forceUpdate;

    public ImageVisibility(String visibility) {
        this.visibility = visibility;
    }

    public ImageVisibility() {}

    public ImageVisibility forceUpdate() {
        this.forceUpdate = true;
        return this;
    }

}
