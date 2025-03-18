package com.example.colorsync;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.transition.ChangeBounds;
import android.transition.TransitionManager;
import android.view.MenuItem;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.inputmethod.InputMethodManager;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;

import com.example.colorsync.DataTypes.User;
import com.google.android.material.bottomnavigation.BottomNavigationView;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class MainActivity extends AppCompatActivity {
    private static APIService api;
    private int currentPage;
    private ConstraintLayout constraintLayout;
    private BottomNavigationView navbar;
    private static MainActivity instance;

    public static APIService getApi() {
        return api;
    }

    public static MainActivity getInstance() {
        return instance;
    }

    @SuppressLint("CheckResult")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);

        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            getWindow().setNavigationBarContrastEnforced(false);
        }

        api = APIInstance.getInstance().create(APIService.class);

        instance = this;

        constraintLayout = findViewById(R.id.main);

        login();

        UserManager.loadToken(this)
                .observeOn(AndroidSchedulers.mainThread())
                        .subscribe(pref -> {
                            if (pref == null || pref.isEmpty()) {
                                login();
                                return;
                            }
                            if (UserManager.user == null || UserManager.token == null) {
                                api.getUser("Bearer " + pref).enqueue(new Callback<User>() {
                                    @Override
                                    public void onResponse(Call<User> call, Response<User> response) {
                                        if (response.code() == 401) {
                                            login();
                                            return;
                                        }
                                        if (response.isSuccessful() && response.body() != null) {
                                            UserManager.user = response.body();
                                            UserManager.token = pref;
                                            goToHome();
                                        } else login();
                                    }

                                    @Override
                                    public void onFailure(Call<User> call, Throwable throwable) {
                                        new AlertDialog.Builder(getBaseContext())
                                                .setTitle("Error loading user")
                                                .setMessage(throwable.getMessage())
                                                .setPositiveButton("OK", null)
                                                .show();
                                        login();
                                    }
                                });
                            } else {
                                goToHome();
                            }
                        }, throwable -> {
                            login();
                        });

        navbar = findViewById(R.id.navbar);
        navbar.setSelectedItemId(R.id.nav_home);

        navbar.setOnItemSelectedListener(item -> {
            Fragment fragment = null;
            MenuItem nav_home = navbar.getMenu().getItem(1);
            if (item.getItemId() == R.id.nav_home) {
                if (currentPage == R.id.nav_home) {
                    HomeFragment homeFragment = (HomeFragment) getSupportFragmentManager().findFragmentById(R.id.fragment_container);
                    if (homeFragment != null) homeFragment.addPost();
                } else {
                    fragment = new HomeFragment();
                    currentPage = R.id.nav_home;
                    nav_home.setIcon(R.drawable.add);
                }
            } else if (item.getItemId() == R.id.nav_profile) {
                fragment = new ProfileFragment(0);
                currentPage = R.id.nav_profile;
                nav_home.setIcon(R.drawable.home);
            } else if (item.getItemId() == R.id.nav_draw) {
                fragment = new LoginFragment();
                currentPage = R.id.loginLayout;
                nav_home.setIcon(R.drawable.home);
            }
            if (fragment != null) {
                hideKeyboard();
                getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, fragment).commit();
            }
            return true;
        });
    }

    private void login() {
        currentPage = R.id.loginLayout;
        getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, new LoginFragment()).commit();
    }

    public void setFullScreenOn() {
        setFullScreen(true);
    }

    public void setFullScreenOff() {
        setFullScreen(false);
    }
    private void setFullScreen(boolean fullScreen) {
        hideKeyboard();
        ConstraintSet off = new ConstraintSet();
        off.clone(constraintLayout);
        ConstraintSet on = new ConstraintSet();
        on.clone(constraintLayout);
        off.clear(R.id.navbar, ConstraintSet.TOP);
        off.connect(R.id.navbar, ConstraintSet.BOTTOM, ConstraintSet.PARENT_ID, ConstraintSet.BOTTOM, 0);

        on.connect(R.id.navbar, ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.BOTTOM, 0);
        on.clear(R.id.navbar, ConstraintSet.BOTTOM);

        if (fullScreen) {
            off.applyTo(constraintLayout);
        } else {
            on.applyTo(constraintLayout);
        }
        constraintLayout.post(() -> {
            TransitionManager.beginDelayedTransition(constraintLayout, new ChangeBounds().setDuration(500).setInterpolator(new AccelerateDecelerateInterpolator()));
            if (fullScreen) {
                on.applyTo(constraintLayout);
            } else {
                off.applyTo(constraintLayout);
            }
        });
    }

    public void goToHome() {
        navbar.setSelectedItemId(R.id.nav_home);
    }

    public void hideKeyboard() {
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm != null && getCurrentFocus() != null) {
            imm.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), 0);
        }
    }
}