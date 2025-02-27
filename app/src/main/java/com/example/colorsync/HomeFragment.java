package com.example.colorsync;

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.cardview.widget.CardView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.transition.ChangeBounds;
import android.transition.Transition;
import android.transition.TransitionManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateDecelerateInterpolator;

import com.example.colorsync.DataTypes.PostResponse;

import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class HomeFragment extends Fragment {
    private boolean isLoading;
    private int offset;
    private ScrollAdapter adapter;
    private List<Post> posts;
    private RecyclerView recyclerView;
    private Context context;
    private View view;

    public HomeFragment() {
        isLoading = false;
        offset = 0;
        posts = new ArrayList<>();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_home, container, false);
        context = view.getContext();

        RecyclerView recyclerView = view.findViewById(R.id.recycleView);
        recyclerView.setLayoutManager(new LinearLayoutManager(context));
        adapter = new ScrollAdapter(posts);
        recyclerView.setAdapter(adapter);

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

        return view;
    }

    public void addPost() {
        ConstraintLayout layout = view.findViewById(R.id.addPostLayout);
        CardView card = view.findViewById(R.id.addPost_card);

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

    private void loadMorePosts() {
        if (isLoading) return;
        isLoading = true;

        MainActivity.getApi().getAllPost(offset).enqueue(new Callback<PostResponse>() {
            @Override
            public void onResponse(Call<PostResponse> call, Response<PostResponse> response) {
                if (response.isSuccessful() && response.body() != null) {
                    PostResponse data = response.body();
                    if (data.offset == null) return;
                    int start = posts.size();
                    posts.addAll(data.data);
                    offset = data.offset;
                    adapter.notifyItemRangeInserted(start, data.data.size());
                    isLoading = false;
                } else {
                    new AlertDialog.Builder(context)
                            .setTitle("Error")
                            .setMessage("Failed to load posts")
                            .show();
                }
            }

            @Override
            public void onFailure(Call<PostResponse> call, Throwable throwable) {
                new AlertDialog.Builder(context)
                        .setTitle("Error")
                        .setMessage(throwable.getMessage())
                        .show();
            }
        });
    }
}