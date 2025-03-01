package com.example.colorsync;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;

import android.transition.ChangeBounds;
import android.transition.TransitionManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.Interpolator;
import android.widget.Button;
import android.widget.Toast;

import com.example.colorsync.DataTypes.LoginRequest;
import com.example.colorsync.DataTypes.RegisterRequest;
import com.example.colorsync.DataTypes.UserWIthToken;
import com.google.android.material.textfield.TextInputEditText;

import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class LoginFragment extends Fragment {
    private ConstraintLayout constraintLayout;
    private TextInputEditText usernameInput;
    private TextInputEditText emailInput;
    private TextInputEditText passwordInput;
    private Button registerChange;
    private Button loginChange;
    private Button loginButton;
    private Button registerButton;
    private ConstraintSet registerSet;
    private ConstraintSet loginSet;

    public LoginFragment() { }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_login, container, false);

        constraintLayout = view.findViewById(R.id.loginLayout);
        usernameInput = view.findViewById(R.id.username);
        emailInput = view.findViewById(R.id.email);
        passwordInput = view.findViewById(R.id.password);
        registerChange = view.findViewById(R.id.registerChange);
        loginChange = view.findViewById(R.id.loginChange);
        loginButton = view.findViewById(R.id.login);
        registerButton = view.findViewById(R.id.register);

        registerButton.setOnClickListener(e -> {
            String username = Objects.requireNonNull(usernameInput.getText()).toString();
            String email = Objects.requireNonNull(emailInput.getText()).toString();
            String password = Objects.requireNonNull(passwordInput.getText()).toString();

            if (username.isEmpty() || email.isEmpty() || password.isEmpty()) return;

            MainActivity.getApi().register(new RegisterRequest(email, password, username)).enqueue(new Callback<UserWIthToken>() {
                @Override
                public void onResponse(Call<UserWIthToken> call, Response<UserWIthToken> response) {
                    if (response.code() == 409) {
                        new android.app.AlertDialog.Builder(getContext())
                                .setTitle("Email Already in use")
                                .setMessage("Please use another email")
                                .setPositiveButton("OK", null)
                                .show();
                        return;
                    }

                    if (response.isSuccessful() && response.body() != null) {
                        UserManager.user = new User(response.body().id, response.body().username);
                        UserManager.saveToken(e.getContext(), response.body().access_token);
                        Toast.makeText(e.getContext(), "Registered", Toast.LENGTH_SHORT).show();
                        MainActivity.getInstance().goToHome();
                    } else {
                        new AlertDialog.Builder(e.getContext())
                                .setTitle("Error")
                                .setMessage("Failed to register")
                                .setPositiveButton("OK", null)
                                .show();
                    }
                }

                @Override
                public void onFailure(Call<UserWIthToken> call, Throwable throwable) {
                    new AlertDialog.Builder(e.getContext())
                            .setTitle("Error")
                            .setMessage(throwable.getMessage())
                            .setPositiveButton("OK", null)
                            .show();
                }
            });
        });
        loginButton.setOnClickListener(e -> {
            String email = Objects.requireNonNull(emailInput.getText()).toString();
            String password = Objects.requireNonNull(passwordInput.getText()).toString();

            if (email.isEmpty() || password.isEmpty()) return;


            MainActivity.getApi().login(new LoginRequest(email, password)).enqueue(new Callback<UserWIthToken>() {
                @Override
                public void onResponse(Call<UserWIthToken> call, Response<UserWIthToken> response) {
                    if (response.code() == 401) {
                        new AlertDialog.Builder(requireContext())
                                .setTitle("Error")
                                .setMessage("Invalid email or password")
                                .setPositiveButton("OK", null)
                                .show();
                        return;
                    }

                    if (response.isSuccessful() && response.body() != null) {
                        UserManager.user = new User(response.body().id, response.body().username);
                        UserManager.saveToken(e.getContext(), response.body().access_token);
                        MainActivity.getInstance().goToHome();
                    } else {
                        new AlertDialog.Builder(e.getContext())
                                .setTitle("Error")
                                .setMessage("Failed to login")
                                .setPositiveButton("OK", null)
                                .show();
                    }
                }

                @Override
                public void onFailure(Call<UserWIthToken> call, Throwable throwable) {
                    new AlertDialog.Builder(e.getContext())
                            .setTitle("Error")
                            .setMessage(throwable.getMessage())
                            .setPositiveButton("OK", null)
                            .show();
                }
            });
        });

        registerChange.setOnClickListener(e -> {
            Interpolator interpolator = new AccelerateDecelerateInterpolator();
            registerButton.setVisibility(View.VISIBLE);
            loginChange.setVisibility(View.VISIBLE);

            ObjectAnimator fadeOut = ObjectAnimator.ofFloat(loginButton, "alpha", 1f, 0f);
            ObjectAnimator fadeIn = ObjectAnimator.ofFloat(registerButton, "alpha", 0f, 1f);
            ObjectAnimator fadeOut2 = ObjectAnimator.ofFloat(registerChange, "alpha", 1f, 0f);
            ObjectAnimator fadeIn2 = ObjectAnimator.ofFloat(loginChange, "alpha", 0f, 1f);
            fadeOut.setDuration(250);
            fadeIn.setDuration(250);
            fadeOut2.setDuration(250);
            fadeIn2.setDuration(250);
            fadeOut.setInterpolator(interpolator);
            fadeIn.setInterpolator(interpolator);
            fadeOut2.setInterpolator(interpolator);
            fadeIn2.setInterpolator(interpolator);

            fadeOut.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    loginButton.setVisibility(View.GONE);
                    registerChange.setVisibility(View.GONE);
                }
            });
            fadeOut.start();
            fadeIn.start();
            fadeOut2.start();
            fadeIn2.start();

            constraintLayout.post(() -> {
                TransitionManager.beginDelayedTransition(constraintLayout, new ChangeBounds().setDuration(250).setInterpolator(interpolator));
                registerSet.applyTo(constraintLayout);
            });
        });

        loginChange.setOnClickListener(e -> {
            Interpolator interpolator = new AccelerateDecelerateInterpolator();
            loginButton.setVisibility(View.VISIBLE);
            registerChange.setVisibility(View.VISIBLE);

            ObjectAnimator fadeOut = ObjectAnimator.ofFloat(registerButton, "alpha", 1f, 0f);
            ObjectAnimator fadeIn = ObjectAnimator.ofFloat(loginButton, "alpha", 0f, 1f);
            ObjectAnimator fadeOut2 = ObjectAnimator.ofFloat(loginChange, "alpha", 1f, 0f);
            ObjectAnimator fadeIn2 = ObjectAnimator.ofFloat(registerChange, "alpha", 0f, 1f);
            fadeOut.setDuration(250);
            fadeIn.setDuration(250);
            fadeOut2.setDuration(250);
            fadeIn2.setDuration(250);
            fadeOut.setInterpolator(interpolator);
            fadeIn.setInterpolator(interpolator);
            fadeOut2.setInterpolator(interpolator);
            fadeIn2.setInterpolator(interpolator);
            fadeOut.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    registerButton.setVisibility(View.GONE);
                    loginChange.setVisibility(View.GONE);
                }
            });
            fadeOut.start();
            fadeIn.start();
            fadeOut2.start();
            fadeIn2.start();

            constraintLayout.post(() -> {
                TransitionManager.beginDelayedTransition(constraintLayout, new ChangeBounds().setDuration(250).setInterpolator(interpolator));
                loginSet.applyTo(constraintLayout);
            });
        });

        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        MainActivity.getInstance().setFullScreenOn();

        registerSet = new ConstraintSet();
        registerSet.clone(constraintLayout);
        loginSet = new ConstraintSet();
        loginSet.clone(constraintLayout);
        registerSet.clear(R.id.usernameContainer, ConstraintSet.BOTTOM);
        registerSet.connect(R.id.usernameContainer, ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
        loginSet.clear(R.id.usernameContainer, ConstraintSet.TOP);
        loginSet.connect(R.id.usernameContainer, ConstraintSet.BOTTOM, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        MainActivity.getInstance().setFullScreenOff();
    }
}