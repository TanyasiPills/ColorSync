package com.example.colorsync;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.transition.ChangeBounds;
import android.transition.Slide;
import android.transition.TransitionManager;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowInsetsController;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.fragment.app.Fragment;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import io.reactivex.rxjava3.schedulers.Schedulers;
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
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        instance = this;

        constraintLayout = findViewById(R.id.main);


        View decorView = getWindow().getDecorView();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            WindowInsetsController insetsController = getWindow().getInsetsController();
            if (insetsController != null) {
                insetsController.hide(WindowInsetsController.BEHAVIOR_SHOW_BARS_BY_SWIPE);
                insetsController.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            decorView.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
                @Override
                public void onSystemUiVisibilityChange(int visibility) {
                    if ((visibility & View.SYSTEM_UI_FLAG_HIDE_NAVIGATION) == 0) {
                        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
                    }
                }
            });
        }

        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES);

        api = APIInstance.getInstance().create(APIService.class);

        UserManager.loadToken(this)
                .observeOn(AndroidSchedulers.mainThread())
                        .subscribe(pref -> {
                            if (pref == null || pref.isEmpty()) {
                                login();
                                return;
                            }
                            if (UserManager.user == null) {
                                api.getUser("Bearer " + pref).enqueue(new Callback<User>() {
                                    @Override
                                    public void onResponse(Call<User> call, Response<User> response) {
                                        if (response.code() == 401) {
                                            login();
                                            return;
                                        }
                                        if (response.isSuccessful() && response.body() != null) {
                                            UserManager.user = response.body();
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
                            }
                        }, throwable -> {
                            Toast.makeText(this, "error loadig: " + throwable.getMessage(), Toast.LENGTH_SHORT).show();
                        });
        navbar = findViewById(R.id.navbar);

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
                fragment = new ProfileFragment(true, 0);
                currentPage = R.id.nav_profile;
                nav_home.setIcon(R.drawable.home);
            } else if (item.getItemId() == R.id.nav_draw) {
                fragment = new LoginFragment();
                currentPage = R.id.nav_draw;
                nav_home.setIcon(R.drawable.home);
            }
            if (fragment != null) {
                getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, fragment).commit();
            }
            return true;
        });

        goToHome();
    }


    @Override
    protected void onResume() {
        super.onResume();
        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
        decorView.setSystemUiVisibility(uiOptions);
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
}