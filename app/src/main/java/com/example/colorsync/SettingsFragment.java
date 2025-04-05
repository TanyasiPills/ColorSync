package com.example.colorsync;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.example.colorsync.DataType.User;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;

public class SettingsFragment extends Fragment {
    private ImageButton back;
    private ImageButton logout;
    private TextView profilePictureText;
    private ImageView profilePicture;
    private TextView usernameText;
    private EditText username;
    private TextView descriptionText;
    private EditText description;

    public SettingsFragment() {}

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @SuppressLint("CheckResult")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_settings, container, false);
        User user = UserManager.user;

        back = view.findViewById(R.id.back);
        logout = view.findViewById(R.id.logout);
        profilePictureText = view.findViewById(R.id.profilePictureText);
        profilePicture = view.findViewById(R.id.profilePicture);
        usernameText = view.findViewById(R.id.usernameText);
        username = view.findViewById(R.id.username);
        descriptionText = view.findViewById(R.id.descriptionText);
        description = view.findViewById(R.id.description);

        back.setOnClickListener(v -> {
            MainActivity.getInstance().goToProfile();
        });

        logout.setOnClickListener(v -> {
            UserManager.logout(getContext())
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribe(() -> {
                        MainActivity.getInstance().login();
                    }, throwable -> {
                        new AlertDialog.Builder(getContext())
                                .setTitle("Error logging out")
                                .setMessage(throwable.getMessage())
                                .setPositiveButton("OK", null)
                                .show();
                    });
        });

        Glide.with(view)
                .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
                .signature(GlideUtils.getSignature())
                .into(profilePicture);

        username.setHint(user.getUsername());
        description.setHint(user.getProfile_description());


        return view;
    }
}