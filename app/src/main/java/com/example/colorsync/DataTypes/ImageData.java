package com.example.colorsync.DataTypes;

import java.util.Date;

public class ImageData {
    public int id;
    public Date date;
    public int userId;
    public String visibility;

    public ImageData(int id, Date date, int userId, String visibility) {
        this.id = id;
        this.date = date;
        this.userId = userId;
        this.visibility = visibility;
    }
}
