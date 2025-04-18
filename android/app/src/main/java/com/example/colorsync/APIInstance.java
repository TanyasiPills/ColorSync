package com.example.colorsync;

import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class APIInstance {
    public static final String BASE_URL = "http://192.168.1.176:3000/";
    private static Retrofit retrofit;

    public static Retrofit getInstance() {
        if (retrofit == null) {
            retrofit = new Retrofit.Builder()
                    .baseUrl(BASE_URL)
                    .addConverterFactory(GsonConverterFactory.create())
                    .build();
        }
        return retrofit;
    }
}
