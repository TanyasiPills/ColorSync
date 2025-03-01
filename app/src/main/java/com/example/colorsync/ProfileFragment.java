package com.example.colorsync;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.os.Bundle;

import androidx.constraintlayout.helper.widget.Grid;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.example.colorsync.DataTypes.ImageData;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

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

    public ProfileFragment(boolean own, int id) {
        if (own) user = UserManager.user;
        //TODO else
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

        username.setText(user.getUsername());
        Glide.with(view)
                .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
                .into(profilePicture);

        loadImages();

        return view;
    }

    private void loadImages() {
        MainActivity.getApi().getUserImages(UserManager.token, user.getId()).enqueue(new Callback<List<ImageData>>() {
            @SuppressLint("NotifyDataSetChanged")
            @Override
            public void onResponse(Call<List<ImageData>> call, Response<List<ImageData>> response) {
                if (response.isSuccessful() && response.body() != null) {
                    items.clear();
                    items.addAll(response.body());
                    adapter.notifyDataSetChanged();
                } else {
                    new AlertDialog.Builder(requireContext())
                            .setTitle("Error")
                            .setMessage("Error loading images")
                            .setPositiveButton("OK", null)
                            .show();
                }
            }

            @Override
            public void onFailure(Call<List<ImageData>> call, Throwable throwable) {
                new AlertDialog.Builder(requireContext())
                        .setTitle("Error loading images")
                        .setMessage(throwable.getMessage())
                        .setPositiveButton("OK", null)
                        .show();
            }
        });
    }
}