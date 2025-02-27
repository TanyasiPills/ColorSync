package com.example.colorsync;

import android.os.Build;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowInsetsController;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.fragment.app.Fragment;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import io.reactivex.rxjava3.schedulers.Schedulers;

public class MainActivity extends AppCompatActivity {
    private static APIService api;
    private int currentPage;

    public static APIService getApi() {
        return api;
    }

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
                .subscribeOn(Schedulers.io())
                        .subscribe(pref -> {
                            Toast.makeText(this, "loaded token: " + pref, Toast.LENGTH_SHORT).show();
                        }, throwable -> {
                            Toast.makeText(this, "error loadig: " + throwable.getMessage(), Toast.LENGTH_SHORT).show();
                        });
        UserManager.saveToken(this, "alma")
                .subscribe(preferences -> {
                    Toast.makeText(this, "Token saved: " + preferences, Toast.LENGTH_SHORT).show();
                }, throwable -> {
                    Toast.makeText(this, "Token error: " + throwable.getMessage(), Toast.LENGTH_SHORT).show();
                });

        currentPage = R.id.nav_home;
        getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, new HomeFragment()).commit();

        BottomNavigationView navbar = findViewById(R.id.navbar);
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
                fragment = new ProfileFragment();
                currentPage = R.id.nav_profile;
                nav_home.setIcon(R.drawable.home);
            } else if (item.getItemId() == R.id.nav_draw) {
                //TODO
                nav_home.setIcon(R.drawable.home);
            }
            if (fragment != null) {
                getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, fragment).commit();
            }
            return true;
        });
    }


    @Override
    protected void onResume() {
        super.onResume();
        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
        decorView.setSystemUiVisibility(uiOptions);
    }



}