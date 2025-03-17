package com.example.colorsync;

import com.example.colorsync.DataTypes.ImageData;
import com.example.colorsync.DataTypes.LoginRequest;
import com.example.colorsync.DataTypes.Post;
import com.example.colorsync.DataTypes.PostCreate;
import com.example.colorsync.DataTypes.PostResponse;
import com.example.colorsync.DataTypes.RegisterRequest;
import com.example.colorsync.DataTypes.User;
import com.example.colorsync.DataTypes.UserWIthToken;

import java.util.List;

import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.Multipart;
import retrofit2.http.POST;
import retrofit2.http.Part;
import retrofit2.http.Path;
import retrofit2.http.Query;

public interface APIService {
    @GET("posts")
    Call<PostResponse> getAllPost(@Query("offset") int offset);

    @Multipart
    @POST("posts")
    Call<Post> createPostWithFile(
            @Header("Authorization") String token,
            @Part MultipartBody.Part file,
            @Part("text") RequestBody text,
            @Part("tags[]") List<RequestBody> tags
    );

    @POST("posts")
    Call<Post> createPost(@Body PostCreate postCreate);


    @POST("users/login")
    Call<UserWIthToken> login(@Body LoginRequest loginRequest);

    @POST("users")
    Call<UserWIthToken> register(@Body RegisterRequest loginRequest);

    @GET("users")
    Call<User> getUser(@Header("Authorization") String token);


    @GET("images/user/{id}")
    Call<List<ImageData>> getUserImages(@Header("Authorization") String token, @Path("id") int id);

    @GET("users/{id}")
    Call<User> getUserById(@Path("id") int id);

    @GET("images")
    Call<List<ImageData>> getUserImages(@Header("Authorization") String token);

}
