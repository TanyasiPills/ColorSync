package com.example.colorsync;

import android.animation.AnimatorSet;
import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.transition.ChangeBounds;
import android.transition.TransitionManager;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewPropertyAnimator;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.inputmethod.InputMethodManager;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.PopupMenu;
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
import androidx.navigation.fragment.NavHostFragment;

import com.bumptech.glide.Glide;
import com.bumptech.glide.load.DataSource;
import com.bumptech.glide.load.engine.GlideException;
import com.bumptech.glide.load.model.GlideUrl;
import com.bumptech.glide.load.model.LazyHeaders;
import com.bumptech.glide.request.RequestListener;
import com.bumptech.glide.request.target.SimpleTarget;
import com.bumptech.glide.request.target.Target;
import com.bumptech.glide.request.transition.Transition;
import com.example.colorsync.DataType.ImageData;
import com.example.colorsync.DataType.ImageVisibility;
import com.example.colorsync.DataType.User;
import com.example.colorsync.Fragment.HomeFragment;
import com.example.colorsync.Fragment.ProfileFragment;
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
    private MaskedImageView fullscreenImage;
    private ConstraintLayout fsiUI;
    private ImageButton fsiUI_back;
    private ImageButton fsiUI_options;
    private View fsiUI_background;
    private MainActivity.imageAnimationProperties fsiProps;

    private static MainActivity instance;
    public NavController navController;
    private NavHostFragment navHostFragment;
    private boolean navbarUIOnly;
    private boolean openAddPost;

    public static APIService getApi() {
        return api;
    }

    public static MainActivity getInstance() {
        return instance;
    }

    public boolean getOpenAddPost() {
        if (openAddPost) {
            openAddPost = false;
            return true;
        } else return false;
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
        openAddPost = false;

        constraintLayout = findViewById(R.id.main);

        navbar = findViewById(R.id.navbar);
        navbar.setSelectedItemId(R.id.nav_home);
        currentPage = R.id.loginFragment;

        fullscreenImage = findViewById(R.id.fullscreenImage);
        fullscreenImage.setPivotX(0f);
        fullscreenImage.setPivotY(0f);

        fsiProps = new MainActivity.imageAnimationProperties();

        fsiUI = findViewById(R.id.fsiUI_layout);
        fsiUI_back = findViewById(R.id.fsiUI_back);
        fsiUI_background = findViewById(R.id.fsiUI_background);
        fsiUI_options = findViewById(R.id.fsUI_options);

        navbar.setOnItemSelectedListener(item -> {
            if (currentPage == R.id.loginFragment) return true;
            if (navbarUIOnly) {
                navbarUIOnly = false;
                return true;
            }
            int destination = item.getItemId();
            return navigateTo(destination);
        });

        navController.addOnDestinationChangedListener((navController1, navDestination, bundle) -> {
            int destination = navDestination.getId();
            if (destination == R.id.homeFragment) {
                currentPage = R.id.nav_home;
            }
            else if (destination == R.id.profileFragment) {
                currentPage = R.id.nav_profile;
            } else {
                currentPage = destination;
            }
            if (currentPage != R.id.loginFragment && currentPage != R.id.settingsFragment) {
                navbarUIOnly = true;
                navbar.setSelectedItemId(currentPage);
            }
        });

        fsiUI_back.setOnClickListener(v -> {
            ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams) fullscreenImage.getLayoutParams();

            ValueAnimator widthAnimator = ValueAnimator.ofInt((int) fsiProps.endWidth, (int) fsiProps.bigStartWidth);
            widthAnimator.addUpdateListener(animation -> {
                params.width = (int) animation.getAnimatedValue();
                fullscreenImage.setLayoutParams(params);
            });

            ValueAnimator heightAnimator = ValueAnimator.ofInt((int) fsiProps.endHeight, (int) fsiProps.bigStartHeight);
            heightAnimator.addUpdateListener(animation -> {
                params.height = (int) animation.getAnimatedValue();
                fullscreenImage.setLayoutParams(params);
            });

            ValueAnimator maskAnimator = ValueAnimator.ofFloat(0, 1);
            maskAnimator.addUpdateListener(animation ->
                fullscreenImage.setMask(GoodMathUtils.lerp(fsiProps.endWidth, fsiProps.startWidth, animation.getAnimatedFraction()), GoodMathUtils.lerp(fsiProps.endHeight, fsiProps.startHeight, animation.getAnimatedFraction()))
            );


            AnimatorSet widthHeightSet = new AnimatorSet();
            widthHeightSet.playTogether(widthAnimator, heightAnimator, maskAnimator);
            widthHeightSet.setDuration(300);

            ViewPropertyAnimator imageAnim = fullscreenImage.animate()
                    .translationX(fsiProps.startX - ((fsiProps.bigStartWidth - fsiProps.startWidth) / 2))
                    .translationY(fsiProps.startY - ((fsiProps.bigStartHeight - fsiProps.startHeight) / 2))
                    .withEndAction(() -> fullscreenImage.setVisibility(View.GONE))
                    .setDuration(300);

            ViewPropertyAnimator uiBackground = fsiUI_background.animate()
                    .alpha(0f)
                    .withEndAction(() -> fsiUI_background.setVisibility(View.GONE))
                    .setDuration(300);

            if (fsiUI.getVisibility() == View.VISIBLE) {
                fsiUI.animate()
                        .alpha(0f)
                        .withEndAction(() -> {
                            fsiUI.setVisibility(View.GONE);
                            fsiUI.setAlpha(1f);
                        })
                        .setDuration(300)
                        .start();
            }

            widthHeightSet.start();
            imageAnim.start();
            uiBackground.start();
        });

        fsiUI_options.setOnClickListener(v -> {
            PopupMenu menu = new PopupMenu(MainActivity.this, v);
            menu.inflate(R.menu.image_options);

            MenuItem changeVisibility = menu.getMenu().findItem(R.id.changeVisibility);
            if (fsiProps.imageData.visibility.equals("private")) changeVisibility.setTitle(R.string.make_publicText);
            menu.setOnMenuItemClickListener(menuItem -> {
                if (menuItem.getItemId() == R.id.changeVisibility) {
                    api.updateImage(UserManager.getBearer(), fsiProps.imageData.id, new ImageVisibility(fsiProps.imageData.visibility.equals("public") ? "private" : "public")).enqueue(new Callback<Void>() {
                        @Override
                        public void onResponse(@NonNull Call<Void> call, @NonNull Response<Void> response) {
                            if (response.isSuccessful()) {
                                if (fsiProps.imageData.visibility.equals("public")) {
                                    fsiProps.imageData.visibility = "private";
                                    changeVisibility.setTitle(R.string.make_publicText);
                                } else {
                                    fsiProps.imageData.visibility = "public";
                                    changeVisibility.setTitle(R.string.make_privateText);
                                }
                            } else if (response.code() == 409) {
                                new AlertDialog.Builder(MainActivity.this)
                                        .setTitle("Couldn't change visibility")
                                        .setMessage("This image is part of a post, do you want to delete all posts containing it?")
                                        .setNegativeButton("Cancel", null)
                                        .setPositiveButton("Yes", (dialogInterface, i) ->
                                            api.updateImage(UserManager.getBearer(), fsiProps.imageData.id, new ImageVisibility("private").forceUpdate()).enqueue(new Callback<Void>() {
                                                @Override
                                                public void onResponse(@NonNull Call<Void> call, @NonNull Response<Void> response) {
                                                    if (response.isSuccessful()) {
                                                        fsiProps.imageData.visibility = "private";
                                                        changeVisibility.setTitle(R.string.make_publicText);
                                                    } else new AlertDialog.Builder(MainActivity.this)
                                                            .setTitle("Error changing visibility")
                                                            .setMessage(response.message())
                                                            .setPositiveButton("OK", null)
                                                            .show();
                                                }

                                                @Override
                                                public void onFailure(@NonNull Call<Void> call, @NonNull Throwable throwable) {
                                                    new AlertDialog.Builder(MainActivity.this)
                                                            .setTitle("Error changing visibility")
                                                            .setMessage(throwable.getMessage())
                                                            .setPositiveButton("OK", null)
                                                            .show();
                                                }
                                            })
                                        )
                                        .show();
                            } else {
                                new AlertDialog.Builder(MainActivity.this)
                                        .setTitle("Error changing visibility")
                                        .setMessage(response.message())
                                        .setPositiveButton("OK", null)
                                        .show();
                            }
                        }

                        @Override
                        public void onFailure(@NonNull Call<Void> call, @NonNull Throwable throwable) {
                            new AlertDialog.Builder(MainActivity.this)
                                    .setTitle("Error changing visibility")
                                    .setMessage(throwable.getMessage())
                                    .setPositiveButton("OK", null)
                                    .show();
                        }
                    });
                } else {
                    new AlertDialog.Builder(MainActivity.this)
                            .setTitle("Delete image")
                            .setMessage("Are you sure you want to delete the image?")
                            .setNegativeButton("Cancel", null)
                            .setPositiveButton("Yes", (dialogInterface, i) ->
                                api.deleteImage(UserManager.getBearer(), fsiProps.imageData.id, false).enqueue(new Callback<Void>() {
                                    @Override
                                    public void onResponse(@NonNull Call<Void> call, @NonNull Response<Void> response) {
                                        if (response.isSuccessful()) {
                                            deleteClose();
                                        } else if (response.code() == 409) {
                                            new AlertDialog.Builder(MainActivity.this)
                                                    .setTitle("Couldn't delete image")
                                                    .setMessage("This image is part of a post, do you want to delete all posts containing it?")
                                                    .setNegativeButton("Cancel", null)
                                                    .setPositiveButton("Yes", (dialogInterface, i) ->
                                                        api.deleteImage(UserManager.getBearer(), fsiProps.imageData.id, true).enqueue(new Callback<Void>() {
                                                            @Override
                                                            public void onResponse(@NonNull Call<Void> call, @NonNull Response<Void> response) {
                                                                if (response.isSuccessful()) {
                                                                    deleteClose();
                                                                } else new AlertDialog.Builder(MainActivity.this)
                                                                        .setTitle("Error deleting image")
                                                                        .setMessage(response.message())
                                                                        .setPositiveButton("OK", null)
                                                                        .show();
                                                            }

                                                            @Override
                                                            public void onFailure(@NonNull Call<Void> call, @NonNull Throwable throwable) {
                                                                new AlertDialog.Builder(MainActivity.this)
                                                                        .setTitle("Error deleting image")
                                                                        .setMessage(throwable.getMessage())
                                                                        .setPositiveButton("OK", null)
                                                                        .show();
                                                            }
                                                        })
                                                    )
                                                    .show();
                                        } else {
                                            new AlertDialog.Builder(MainActivity.this)
                                                    .setTitle("Error deleting image")
                                                    .setMessage(response.message())
                                                    .setPositiveButton("OK", null)
                                                    .show();
                                        }
                                    }

                                    @Override
                                    public void onFailure(@NonNull Call<Void> call, @NonNull Throwable throwable) {
                                        new AlertDialog.Builder(MainActivity.this)
                                                .setTitle("Error deleting image")
                                                .setMessage(throwable.getMessage())
                                                .setPositiveButton("OK", null)
                                                .show();
                                    }
                                })
                            )
                            .setNegativeButton("Cancel", null)
                            .show();
                }
                return false;
            });
            menu.show();
        });

        fullscreenImage.setOnClickListener(this::openFsiUI);

        fsiUI_background.setOnClickListener(this::openFsiUI);


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
                                    public void onResponse(@NonNull Call<User> call, @NonNull Response<User> response) {
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
                                    public void onFailure(@NonNull Call<User> call, @NonNull Throwable throwable) {
                                        new AlertDialog.Builder(MainActivity.this)
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
                        }, throwable ->
                            login()
                        );
    }

    private void deleteClose() {
        Fragment currentFragment = navHostFragment.getChildFragmentManager().getFragments().get(0);

        if (currentFragment instanceof ProfileFragment) {
            ((ProfileFragment)currentFragment).loadImages();
        }

        fullscreenImage.animate()
                .alpha(0f)
                .setDuration(300)
                .withEndAction(() -> {
                    fullscreenImage.setVisibility(View.GONE);
                    fullscreenImage.setAlpha(1f);
                })
                .start();

        fsiUI.animate()
                .alpha(0f)
                .setDuration(300)
                .withEndAction(() -> {
                    fsiUI.setVisibility(View.GONE);
                    fsiUI.setAlpha(1f);
                })
                .start();

        fsiUI_background.animate()
                .alpha(0f)
                .setDuration(300)
                .withEndAction(() -> {
                    fsiUI_background.setVisibility(View.GONE);
                    fsiUI_background.setAlpha(1f);
                })
                .start();
    }

    private void navigatePop(int id, int destination) {
        boolean popped = navController.popBackStack(id, false);
        if (!popped) {
            navController.navigate(destination);
        }
    }

    private boolean navigateTo(int destination) {
        return navigateTo(destination, 0);
    }

    private boolean navigateTo(int destination, int profileId) {
        if (destination == R.id.nav_home) {
            if (currentPage == R.id.loginFragment) {
                navigatePop(R.id.homeFragment, R.id.action_loginFragment_to_homeFragment);
            } else if (currentPage == R.id.nav_profile) {
                navigatePop(R.id.homeFragment, R.id.action_profileFragment_to_homeFragment);
            } else if (currentPage == R.id.settingsFragment) {
                navigatePop(R.id.homeFragment, R.id.action_settingsFragment_to_homeFragment);
            }
        } else if (destination == R.id.nav_profile) {
            Bundle bundle = new Bundle();
            bundle.putInt("id", profileId);

            if (currentPage == R.id.nav_home) {
                navController.navigate(R.id.action_homeFragment_to_profileFragment, bundle);
            } else if (currentPage == R.id.loginFragment) {
                navController.navigate(R.id.action_loginFragment_to_profileFragment, bundle);
            } else if (currentPage == R.id.settingsFragment) {
                navigatePop(R.id.profileFragment, R.id.action_settingsFragment_to_profileFragment);
            }
        } else if (destination == R.id.nav_add) {
            if (currentPage == R.id.nav_home) {
                if (navHostFragment != null) {
                    Fragment currentFragment = navHostFragment.getChildFragmentManager().getFragments().get(0);
                    if (currentFragment instanceof HomeFragment) {
                        HomeFragment homeFragment = (HomeFragment) currentFragment;
                        homeFragment.addPost();
                    }
                }
                return false;
            } else if (currentPage == R.id.nav_profile) {
                openAddPost = true;
                navigatePop(R.id.homeFragment, R.id.action_profileFragment_to_homeFragment);
                return false;
            }
        } else if (destination == R.id.settingsFragment) {
            if (currentPage == R.id.nav_profile) {
                navigatePop(R.id.profileFragment, R.id.action_profileFragment_to_settingsFragment);
            }
        } else if (destination == R.id.loginFragment) {
            if (currentPage == R.id.nav_home) {
                navController.navigate(R.id.action_homeFragment_to_loginFragment);
            } else if (currentPage == R.id.nav_profile) {
                navController.navigate(R.id.action_profileFragment_to_loginFragment);
            } else if (currentPage == R.id.settingsFragment) {
                navController.navigate(R.id.action_settingsFragment_to_loginFragment);
            }
        }
        return true;
    }

    private void openFsiUI(View v) {
        ConstraintSet open = new ConstraintSet();
        open.clone(constraintLayout);
        open.connect(R.id.fsiUI_layout, ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
        open.clear(R.id.fsiUI_layout, ConstraintSet.BOTTOM);
        open.setVisibility(R.id.fsiUI_layout, View.VISIBLE);
        open.clear(R.id.fullscreenImage);
        open.clear(R.id.fsiUI_background);

        ConstraintSet closed = new ConstraintSet();
        closed.clone(constraintLayout);
        closed.connect(R.id.fsiUI_layout, ConstraintSet.BOTTOM, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
        closed.clear(R.id.fsiUI_layout, ConstraintSet.TOP);
        closed.clear(R.id.fullscreenImage);
        closed.clear(R.id.fsiUI_background);

        ChangeBounds transition = new ChangeBounds();
        transition.setDuration(200);

        transition.excludeTarget(R.id.fullscreenImage, true);
        transition.excludeTarget(R.id.fsiUI_background, true);

        boolean opening = fsiUI.getVisibility() == View.GONE;
        if (opening) {
            closed.applyTo(constraintLayout);
            fsiUI.setVisibility(View.VISIBLE);
        } else {
            transition.addListener(new android.transition.Transition.TransitionListener() {
                @Override
                public void onTransitionStart(android.transition.Transition transition) {

                }

                @Override
                public void onTransitionEnd(android.transition.Transition transition) {
                    fsiUI.setVisibility(View.GONE);
                }

                @Override
                public void onTransitionCancel(android.transition.Transition transition) {

                }

                @Override
                public void onTransitionPause(android.transition.Transition transition) {

                }

                @Override
                public void onTransitionResume(android.transition.Transition transition) {

                }
            });
        }

        constraintLayout.post(() -> {
            TransitionManager.beginDelayedTransition(constraintLayout, transition);
            if (opening) {
                open.applyTo(constraintLayout);
            }
            else {
                closed.applyTo(constraintLayout);
            }
        });
    }

    private static class imageAnimationProperties {
        float startWidth = 0;
        float startHeight = 0;
        float bigStartWidth = 0;
        float bigStartHeight = 0;
        float startX = 0;
        float startY = 0;
        float endWidth = 0;
        float endHeight = 0;
        ImageData imageData = null;
    }

    public void setFullscreenImage(ImageView original, ImageData imageData, boolean edit) {
        GlideUrl url = new GlideUrl(
                APIInstance.BASE_URL + "images/" + imageData.id,
                new LazyHeaders.Builder()
                        .addHeader("Authorization", "Bearer " + UserManager.token)
                        .build());

        Glide.with(MainActivity.this)
                .load(url)
                .listener(new RequestListener<Drawable>() {
                    @Override
                    public boolean onLoadFailed(@Nullable GlideException e, Object model, Target<Drawable> target, boolean isFirstResource) {
                        Toast.makeText(MainActivity.this, "Failed", Toast.LENGTH_SHORT).show();
                        return false;
                    }

                    @Override
                    public boolean onResourceReady(Drawable resource, Object model, Target<Drawable> target, DataSource dataSource, boolean isFirstResource) {
                        float imageHeight = resource.getIntrinsicHeight();
                        float imageWidth = resource.getIntrinsicWidth();

                        int[] screenPos = new int[2];
                        original.getLocationOnScreen(screenPos);
                        float startX = screenPos[0];
                        float startY = screenPos[1];
                        float startWidth = original.getWidth();
                        float startHeight = original.getHeight();

                        float bigStartWidth = startWidth;
                        float bigStartHeight = startHeight;
                        if (imageWidth > imageHeight) {
                            bigStartWidth = startHeight * (imageWidth / imageHeight);
                        } else {
                            bigStartHeight = startWidth * (imageHeight / imageWidth);
                        }

                        if (edit) fsiUI_options.setVisibility(View.VISIBLE);
                        else fsiUI_options.setVisibility(View.GONE);


                        ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams) fullscreenImage.getLayoutParams();
                        params.width = (int) bigStartWidth;
                        params.height = (int) bigStartHeight;
                        fullscreenImage.setLayoutParams(params);

                        fullscreenImage.setTranslationX(startX - ((bigStartWidth - startWidth) / 2));
                        fullscreenImage.setTranslationY(startY - ((bigStartHeight - startHeight) / 2));

                        float endWidth = constraintLayout.getWidth();
                        float endHeight = constraintLayout.getHeight();

                        ValueAnimator widthAnimator = ValueAnimator.ofInt((int) bigStartWidth, (int) endWidth);
                        widthAnimator.addUpdateListener(animation -> {
                            params.width = (int) animation.getAnimatedValue();
                            fullscreenImage.setLayoutParams(params);
                        });

                        ValueAnimator heightAnimator = ValueAnimator.ofInt((int) bigStartHeight, (int) endHeight);
                        heightAnimator.addUpdateListener(animation -> {
                            params.height = (int) animation.getAnimatedValue();
                            fullscreenImage.setLayoutParams(params);
                        });

                        ValueAnimator maskAnimator = ValueAnimator.ofFloat(0, 1);
                        maskAnimator.addUpdateListener(animation ->
                            fullscreenImage.setMask(GoodMathUtils.lerp(startWidth, endWidth, animation.getAnimatedFraction()), GoodMathUtils.lerp(startHeight, endHeight, animation.getAnimatedFraction()))
                        );


                        AnimatorSet widthHeightSet = new AnimatorSet();
                        widthHeightSet.playTogether(widthAnimator, heightAnimator, maskAnimator);
                        widthHeightSet.setDuration(300);

                        ViewPropertyAnimator translateAnim = fullscreenImage.animate()
                                .translationX(0)
                                .translationY(0)
                                .setDuration(300);


                        fsiUI_background.setAlpha(0f);

                        ViewPropertyAnimator uiBackground = fsiUI_background.animate()
                                .alpha(1f)
                                .setDuration(300);

                        fsiUI_background.setVisibility(View.VISIBLE);

                        fullscreenImage.setMask(startWidth, startHeight);

                        fullscreenImage.setVisibility(View.VISIBLE);

                        widthHeightSet.start();
                        translateAnim.start();
                        uiBackground.start();

                        fsiProps.startWidth = startWidth;
                        fsiProps.startHeight = startHeight;
                        fsiProps.bigStartWidth = bigStartWidth;
                        fsiProps.bigStartHeight = bigStartHeight;
                        fsiProps.startX = startX;
                        fsiProps.startY = startY;
                        fsiProps.endWidth = endWidth;
                        fsiProps.endHeight = endHeight;
                        fsiProps.imageData = imageData;
                        return false;
                    }
                })
                .into(new SimpleTarget<Drawable>() {
                    @Override
                    public void onResourceReady(@NonNull Drawable resource, @Nullable Transition<? super Drawable> transition) {
                        fullscreenImage.setImageDrawable(resource);
                    }
                });
    }
    public void login() {
        navigateTo(R.id.loginFragment);
    }

    public void goToProfile(int id) {
        navigateTo(R.id.nav_profile, id);
    }

    public void goToProfile() {
        navigateTo(R.id.nav_profile);
    }

    public void goToHome() {
        navigateTo(R.id.nav_home);
    }

    public void goToSettings() {
        navigateTo(R.id.settingsFragment);
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