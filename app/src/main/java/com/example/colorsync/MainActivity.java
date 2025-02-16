package com.example.colorsync;

import android.os.Build;
import android.os.Bundle;
import android.transition.AutoTransition;
import android.transition.Transition;
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
    private ConstraintLayout constraintLayout;
    private View loadingBar;
    private ConstraintSet set1;
    private ConstraintSet set2;

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

        constraintLayout = findViewById(R.id.main);
        loadingBar = findViewById(R.id.loadingBar);

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
                    int lastVisibleItemPosition = layoutManager.findLastVisibleItemPosition();

                    int loadMoreThreshold = 5;
                    if ((visibleItemCount + pastVisibleItems) >= totalItemCount - loadMoreThreshold) {
                        loadMorePosts();
                    }

                    if (lastVisibleItemPosition == totalItemCount - 1) {
                        showLoadingBar();
                    }
                }
            }
        });

        set1 = new ConstraintSet();
        set1.clone(constraintLayout);
        set2 = new ConstraintSet();
        set2.clone(constraintLayout);

        set2.applyTo(constraintLayout);


        showLoadingBar();
        loadMorePosts();
    }

    private void showLoadingBar() {
        Toast.makeText(this, "load more posts", Toast.LENGTH_SHORT).show();
        loadingBar.setVisibility(View.VISIBLE);

        Transition transition = getTransition();
        set2.applyTo(constraintLayout);

        set1.applyTo(constraintLayout);
        TransitionManager.beginDelayedTransition(constraintLayout, transition);
        set2.applyTo(constraintLayout);
    }

    @NonNull
    private Transition getTransition() {
        set1.connect(loadingBar.getId(), ConstraintSet.START, R.id.navbar, ConstraintSet.START, 0);
        set1.connect(loadingBar.getId(), ConstraintSet.BOTTOM, R.id.navbar, ConstraintSet.TOP, 0);
        set1.clear(loadingBar.getId(), ConstraintSet.END);
        set1.constrainWidth(loadingBar.getId(), 300);
        set1.constrainHeight(loadingBar.getId(), 5);

        set2.connect(loadingBar.getId(), ConstraintSet.END, R.id.navbar, ConstraintSet.END, 0);
        set2.connect(loadingBar.getId(), ConstraintSet.BOTTOM, R.id.navbar, ConstraintSet.TOP, 0);
        set2.clear(loadingBar.getId(), ConstraintSet.START);
        set2.constrainWidth(loadingBar.getId(), 300);
        set2.constrainHeight(loadingBar.getId(), 5);

        Transition transition = new AutoTransition();
        transition.setDuration(1000);
        transition.setInterpolator(new AccelerateDecelerateInterpolator());

        transition.addListener(new Transition.TransitionListener() {
            @Override
            public void onTransitionStart(Transition transition) {

            }

            @Override
            public void onTransitionEnd(Transition transition) {
                ConstraintSet temp = set1;
                set1 = set2;
                set2 = temp;
                TransitionManager.beginDelayedTransition(constraintLayout, transition);
                set2.applyTo(constraintLayout);
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
        return transition;
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
}