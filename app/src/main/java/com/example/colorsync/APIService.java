package com.example.colorsync;

import com.example.colorsync.DataTypes.PostResponse;

import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.Query;

public interface APIService {
    @GET("posts")
    Call<PostResponse> getAllPost(@Query("offset") int offset);
}
