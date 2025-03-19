package com.example.colorsync;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.transition.ChangeBounds;
import android.transition.TransitionManager;
import android.util.AttributeSet;
import android.view.MenuItem;
import android.view.View;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.inputmethod.InputMethodManager;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.fragment.NavHostFragment;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import com.example.colorsync.DataTypes.User;
import com.google.android.material.bottomnavigation.BottomNavigationView;

import java.util.Objects;

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
    public NavController navController;
    private NavHostFragment navHostFragment;
    private boolean navbarUIOnly;

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

        navHostFragment = (NavHostFragment) getSupportFragmentManager().findFragmentById(R.id.fragment_container);
        navController = Objects.requireNonNull(navHostFragment).getNavController();

        api = APIInstance.getInstance().create(APIService.class);

        instance = this;
        navbarUIOnly = false;

        constraintLayout = findViewById(R.id.main);

        navbar = findViewById(R.id.navbar);
        navbar.setSelectedItemId(R.id.nav_home);
        currentPage = R.id.loginFragment;

        navbar.setOnItemSelectedListener(item -> {
            if (navbarUIOnly) {
                navbarUIOnly = false;
                return true;
            }
            int destination = item.getItemId();
            if (destination == R.id.nav_draw) login();
            else navigateTo(destination);
            return true;
        });

        navController.addOnDestinationChangedListener((navController1, navDestination, bundle) -> {
            MenuItem nav_home = navbar.getMenu().getItem(1);
            int destination = navDestination.getId();
            if (destination == R.id.loginFragment) {
                Toast.makeText(this, "login", Toast.LENGTH_SHORT).show();
                if (currentPage == R.id.nav_home) nav_home.setIcon(R.drawable.home);
                currentPage = R.id.loginFragment;
            }
            else if (destination == R.id.homeFragment) {
                Toast.makeText(this, "home", Toast.LENGTH_SHORT).show();
                if (currentPage != R.id.nav_home) nav_home.setIcon(R.drawable.add);
                currentPage = R.id.nav_home;
            }
            else if (destination == R.id.profileFragment) {
                Toast.makeText(this, "profile", Toast.LENGTH_SHORT).show();
                if (currentPage == R.id.nav_home) nav_home.setIcon(R.drawable.home);
                currentPage = R.id.nav_profile;
            }
            if (currentPage != R.id.loginFragment) {
                navbarUIOnly = true;
                navbar.setSelectedItemId(currentPage);
            }
        });

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
    }

    private void nagviatePop(int id, int destination) {
        boolean popped = navController.popBackStack(id, false);
        if (!popped) {
            navController.navigate(destination);
        }
    }

    private void navigateTo(int destination) {
        if (destination == R.id.nav_home) {
            if (currentPage == R.id.nav_home) {
                if (navHostFragment != null) {
                    Fragment currentFragment = navHostFragment.getChildFragmentManager().getFragments().get(0);
                    if (currentFragment instanceof HomeFragment) {
                        HomeFragment homeFragment = (HomeFragment) currentFragment;
                        homeFragment.addPost();
                    }
                }
            } else if (currentPage == R.id.loginFragment) {
                navController.navigate(R.id.action_loginFragment_to_homeFragment);
                navController.popBackStack(destination, true, false);
            } else if (currentPage == R.id.nav_profile) {
                nagviatePop(R.id.homeFragment, R.id.action_profileFragment_to_homeFragment);
                navController.popBackStack(destination, true, false);
            }
        } else if (destination == R.id.nav_profile) {
            if (currentPage == R.id.nav_home) {
                navController.navigate(R.id.action_homeFragment_to_profileFragment);
                navController.popBackStack(destination, true, false);
            } else if (currentPage == R.id.loginFragment) {
                navController.navigate(R.id.action_loginFragment_to_profileFragment);
                navController.popBackStack(destination, true, false);
            }
        } else if (destination == R.id.loginFragment) {
            if (currentPage == R.id.nav_home) {
                navController.navigate(R.id.action_homeFragment_to_loginFragment);
                navController.popBackStack(destination, true, false);
            } else if (currentPage == R.id.nav_profile) {{
                navController.navigate(R.id.action_profileFragment_to_loginFragment);
                navController.popBackStack(destination, true, false);
            }}
        }
    }

    private void login() {
        navigateTo(R.id.loginFragment);
    }

    public void goToHome() {
        navbar.setSelectedItemId(R.id.nav_home);
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

    public void hideKeyboard() {
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm != null && getCurrentFocus() != null) {
            imm.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), 0);
        }
    }
}