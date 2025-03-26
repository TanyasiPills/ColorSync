package com.example.colorsync;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.example.colorsync.DataTypes.ImageData;
import com.example.colorsync.DataTypes.User;

import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ProfileFragment extends Fragment {
    private User user;
    private View view;
    private ImageView profilePicture;
    private TextView username;
    private RecyclerView imagesContainer;
    private List<ImageData> items;
    private ImageGridAdapter adapter;
    private boolean loaded = false;


    public ProfileFragment() {
        this(0);
    }

    public ProfileFragment(int id) {
        if (id < 1) {
            user = UserManager.user;
            load();
        }
        else MainActivity.getApi().getUserById(id).enqueue(new Callback<User>() {
            @Override
            public void onResponse(@NonNull Call<User> call, @NonNull Response<User> response) {
                if (response.isSuccessful() && response.body() != null) {
                    user = response.body();
                    load();
                } else {
                    new AlertDialog.Builder(MainActivity.getInstance())
                            .setTitle("Failed to load user")
                            .setPositiveButton("Ok", (dialog, which) -> MainActivity.getInstance().goToHome())
                            .show();
                }
            }

            @Override
            public void onFailure(@NonNull Call<User> call, @NonNull Throwable throwable) {
                new AlertDialog.Builder(MainActivity.getInstance())
                        .setTitle("Failed to load user")
                        .setMessage(throwable.getMessage())
                        .setPositiveButton("Ok", (dialog, which) -> MainActivity.getInstance().goToHome())
                        .show();
            }
        });
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {super.onCreate(savedInstanceState); }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_profile, container, false);
        imagesContainer = view.findViewById(R.id.imagesContainer);
        username = view.findViewById(R.id.username);
        profilePicture = view.findViewById(R.id.profilePicture);
        items = new ArrayList<>();

        adapter = new ImageGridAdapter(items);
        imagesContainer.setAdapter(adapter);
        imagesContainer.setLayoutManager(new GridLayoutManager(requireContext(), 2));

        if (loaded) load();
        else loaded = true;

        return view;
    }

    private void load() {
        if (!loaded) {
            loaded = true;
            return;
        }
        username.setText(user.getUsername());
        Glide.with(view)
                .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
                .into(profilePicture);

        loadImages();
    }

    private void loadImages() {
        MainActivity.getApi().getUserImages(UserManager.getBearer(), user.getId()).enqueue(new Callback<List<ImageData>>() {
            @SuppressLint("NotifyDataSetChanged")
            @Override
            public void onResponse(@NonNull Call<List<ImageData>> call, @NonNull Response<List<ImageData>> response) {
                if (response.isSuccessful() && response.body() != null) {
                    items.clear();
                    items.addAll(response.body());
                    adapter.notifyDataSetChanged();
                    if (items.isEmpty()) view.findViewById(R.id.imagesCardView).setVisibility(View.GONE);
                } else if (response.code() == 404) {
                    //TODO: no profile images
                }
                else {
                    new AlertDialog.Builder(requireContext())
                            .setTitle("Error loading images")
                            .setMessage(response.message())
                            .setPositiveButton("OK", null)
                            .show();
                }
            }

            @Override
            public void onFailure(@NonNull Call<List<ImageData>> call, @NonNull Throwable throwable) {
                new AlertDialog.Builder(requireContext())
                        .setTitle("Error loading images")
                        .setMessage(throwable.getMessage())
                        .setPositiveButton("OK", null)
                        .show();
            }
        });
    }
}