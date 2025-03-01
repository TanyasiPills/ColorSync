package com.example.colorsync;

import com.example.colorsync.DataTypes.ImageData;
import com.example.colorsync.DataTypes.LoginRequest;
import com.example.colorsync.DataTypes.PostResponse;
import com.example.colorsync.DataTypes.RegisterRequest;
import com.example.colorsync.DataTypes.UserWIthToken;

import java.util.List;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.POST;
import retrofit2.http.Path;
import retrofit2.http.Query;

public interface APIService {
    @GET("posts")
    Call<PostResponse> getAllPost(@Query("offset") int offset);

    @POST("users/login")
    Call<UserWIthToken> login(@Body LoginRequest loginRequest);

    @POST("users")
    Call<UserWIthToken> register(@Body RegisterRequest loginRequest);

    @GET("users")
    Call<User> getUser(@Header("Authorization") String token);

    @GET("images/user/{id}")
    Call<List<ImageData>> getUserImages(@Header("Authorization") String token, @Path("id") int id);

}
