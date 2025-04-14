package com.example.colorsync;

import com.example.colorsync.DataType.Comment;
import com.example.colorsync.DataType.CreateComment;
import com.example.colorsync.DataType.IdType;
import com.example.colorsync.DataType.ImageData;
import com.example.colorsync.DataType.ImageVisibility;
import com.example.colorsync.DataType.LoginRequest;
import com.example.colorsync.DataType.Post;
import com.example.colorsync.DataType.PostCreate;
import com.example.colorsync.DataType.PostResponse;
import com.example.colorsync.DataType.RegisterRequest;
import com.example.colorsync.DataType.User;
import com.example.colorsync.DataType.UserWIthToken;

import java.util.List;

import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.DELETE;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.Multipart;
import retrofit2.http.PATCH;
import retrofit2.http.POST;
import retrofit2.http.Part;
import retrofit2.http.Path;
import retrofit2.http.Query;

public interface APIService {
    @GET("posts")
    Call<PostResponse> getAllPost(@Header("Authorization") String token, @Query("offset") int offset);

    @GET("posts/search")
    Call<PostResponse> searchPosts(@Header("Authorization") String token, @Query("q") String query, @Query("tags") List<String> tags, @Query("offset") int offset, @Query("imageOnly") boolean imageOnly);

    @Multipart
    @POST("posts")
    Call<Post> createPostWithFile(
            @Header("Authorization") String token,
            @Part MultipartBody.Part file,
            @Part("text") RequestBody text,
            @Part("tags[]") List<RequestBody> tags
    );
    @POST("posts")
    Call<Post> createPost(@Header("Authorization") String token, @Body PostCreate postCreate);
    @POST("posts/like/{id}")
    Call<Void> likePost(@Header("Authorization") String token, @Path("id") int postId);


    @POST("/comments")
    Call<Comment> createComment(@Header("Authorization") String token, @Body CreateComment comment);


    @POST("users/login")
    Call<UserWIthToken> login(@Body LoginRequest loginRequest);
    @POST("users")
    Call<UserWIthToken> register(@Body RegisterRequest loginRequest);
    @GET("users")
    Call<User> getUser(@Header("Authorization") String token);
    @GET("users/{id}")
    Call<User> getUserById(@Path("id") int id);
    @GET("users/likes")
    Call<List<Integer>> getLikes(@Header("Authorization") String token);
    @PATCH("users")
    Call<User> updateUser(@Header("Authorization") String token, @Body User user);
    @Multipart
    @POST("users/pfp")
    Call<Void> uploadPfp(@Header("Authorization") String token, @Part MultipartBody.Part file);


    @GET("images/user/{id}")
    Call<List<ImageData>> getUserImages(@Header("Authorization") String token, @Path("id") int id);
    @GET("images")
    Call<List<ImageData>> getUserImages(@Header("Authorization") String token);
    @PATCH("images/{id}")
    Call<Void> updateImage(@Header("Authorization") String token, @Path("id") int id, @Body ImageVisibility visibility);
    @DELETE("images/{id}")
    Call<Void> deleteImage(@Header("Authorization") String token, @Path("id") int id, @Query("forceDelete") boolean forceDelete);

    @Multipart
    @POST("images")
    Call<IdType> uploadImage(
            @Header("Authorization") String token,
            @Part MultipartBody.Part file,
            @Part("visibility") RequestBody text
    );
}
