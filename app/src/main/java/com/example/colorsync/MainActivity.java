package com.example.colorsync;

import android.animation.ArgbEvaluator;
import android.animation.ValueAnimator;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.transition.AutoTransition;
import android.transition.ChangeBounds;
import android.transition.Scene;
import android.transition.Slide;
import android.transition.Transition;
import android.transition.TransitionListenerAdapter;
import android.transition.TransitionManager;
import android.view.View;
import android.view.WindowInsetsController;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.cardview.widget.CardView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.example.colorsync.DataTypes.PostResponse;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;

public class MainActivity extends AppCompatActivity {
    private final ArrayList<Post> posts = new ArrayList<>();;
    private boolean isLoading;
    private int lastId;
    private ScrollAdapter adapter;
    private APIService api;

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

        isLoading = false;
        lastId = 0;
        RecyclerView recyclerView = findViewById(R.id.recycleView);
        recyclerView.setLayoutManager(new LinearLayoutManager(MainActivity.this));
        adapter = new ScrollAdapter(posts);
        recyclerView.setAdapter(adapter);
        api = APIInstance.getInstance().create(APIService.class);

        recyclerView.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrolled(@NonNull RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);

                if (dy > 0) {
                    LinearLayoutManager layoutManager = (LinearLayoutManager) recyclerView.getLayoutManager();
                    assert layoutManager != null;
                    int visibleItemCount = layoutManager.getChildCount();
                    int totalItemCount = layoutManager.getItemCount();
                    int pastVisibleItems = layoutManager.findFirstVisibleItemPosition();

                    int loadMoreThreshold = 5;
                    if ((visibleItemCount + pastVisibleItems) >= totalItemCount - loadMoreThreshold) {
                        loadMorePosts();
                    }
                }
            }
        });

        loadMorePosts();
    }

    private void loadMorePosts() {
        if (isLoading) return;
        isLoading = true;

        api.getAllPost(lastId).enqueue(new Callback<PostResponse>() {
            @Override
            public void onResponse(Call<PostResponse> call, Response<PostResponse> response) {
                if (response.isSuccessful() && response.body() != null) {
                    PostResponse data = response.body();
                    if (data.newLastId == null) return;
                    int start = posts.size();
                    posts.addAll(data.data);
                    lastId = data.newLastId;
                    adapter.notifyItemRangeInserted(start, data.data.size());
                    isLoading = false;
                } else {
                    new AlertDialog.Builder(MainActivity.this)
                            .setTitle("Error")
                            .setMessage("Failed to load posts")
                            .show();
                }
            }

            @Override
            public void onFailure(Call<PostResponse> call, Throwable throwable) {
                new AlertDialog.Builder(MainActivity.this)
                        .setTitle("Error")
                        .setMessage(throwable.getMessage())
                        .show();
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
        decorView.setSystemUiVisibility(uiOptions);
    }


    public void homeButtonOnClick(View view) {
        ConstraintLayout layout = findViewById(R.id.addPostLayout);
        CardView card = findViewById(R.id.addPost_card);

        Drawable background = layout.getBackground();

        ConstraintSet setOff = new ConstraintSet();
        setOff.clone(layout);
        setOff.clear(card.getId(), ConstraintSet.BOTTOM);
        setOff.clear(card.getId(), ConstraintSet.TOP);
        setOff.connect(card.getId(), ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.BOTTOM);

        ConstraintSet setOn = new ConstraintSet();
        setOn.clone(layout);
        setOn.connect(card.getId(), ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
        setOn.connect(card.getId(), ConstraintSet.BOTTOM, ConstraintSet.PARENT_ID, ConstraintSet.BOTTOM);


        if (layout.getVisibility() == View.GONE) {
            layout.setVisibility(View.VISIBLE);

            Transition transition = new ChangeBounds()
                    .setDuration(250)
                    .setInterpolator(new AccelerateDecelerateInterpolator());

            setOff.applyTo(layout);

            layout.post(() -> {
                TransitionManager.beginDelayedTransition(layout, transition);
                setOn.applyTo(layout);
                ValueAnimator alphaAnimator = ValueAnimator.ofInt(0, 256);
                alphaAnimator.setDuration(500);
                alphaAnimator.addUpdateListener(animation -> {
                    int alpha = (int) animation.getAnimatedValue();
                    background.setAlpha(alpha);
                });
                alphaAnimator.start();
            });
        } else {
            Transition transition = new ChangeBounds()
                    .setDuration(250)
                    .setInterpolator(new AccelerateDecelerateInterpolator())
                    .addListener(new Transition.TransitionListener() {
                @Override
                public void onTransitionStart(Transition transition) {

                }

                @Override
                public void onTransitionEnd(Transition transition) {
                    layout.setVisibility(View.GONE);
                }

                @Override
                public void onTransitionCancel(Transition transition) {

                }

                @Override
                public void onTransitionPause(Transition transition) {

                }

                @Override
                public void onTransitionResume(Transition transition) {

                }
            });

            setOn.applyTo(layout);
            layout.post(() -> {
                TransitionManager.beginDelayedTransition(layout, transition);
                setOff.applyTo(layout);
                ValueAnimator alphaAnimator = ValueAnimator.ofInt(Math.min(256, background.getAlpha()), 0);
                alphaAnimator.setDuration(500);
                alphaAnimator.addUpdateListener(animation -> {
                    int alpha = (int) animation.getAnimatedValue();
                    background.setAlpha(alpha);
                });
                alphaAnimator.start();
            });
        }
    }
}