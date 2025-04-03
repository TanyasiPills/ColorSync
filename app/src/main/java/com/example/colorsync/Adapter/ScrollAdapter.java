package com.example.colorsync.Adapter;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.example.colorsync.APIInstance;
import com.example.colorsync.GlideUtils;
import com.example.colorsync.DataType.Post;
import com.example.colorsync.MainActivity;
import com.example.colorsync.R;
import com.example.colorsync.UserManager;

import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ScrollAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private final List<Post> items;
    private final List<Integer> likes;

    public ScrollAdapter(List<Post> items, List<Integer> likes) {
        this.items = items;
        this.likes = likes;
    }

    public void like(int postId) {
        if (likes.contains(postId)) likes.remove(Integer.valueOf(postId));
        else likes.add(postId);
    }

    public static class PostViewHolder extends RecyclerView.ViewHolder {
        private final TextView username;
        private final TextView description;
        private final ImageView profilePicture;
        private final ImageView image;
        private final RecyclerView commentsView;
        private final ImageView showComments;
        private final LinearLayout showCommentsContainer;
        private final TextView likeText;
        private final ImageButton likeButton;
        private Post post;
        private final TextView date;
        private final RecyclerView tags;
        public PostViewHolder(@NonNull View itemView, Context context, ViewGroup parent, ScrollAdapter adapter) {
            super(itemView);
            username = itemView.findViewById(R.id.username);
            description = itemView.findViewById(R.id.description);
            profilePicture = itemView.findViewById(R.id.profile_picture);
            image = itemView.findViewById(R.id.image);
            commentsView = itemView.findViewById(R.id.commentsView);
            showComments = itemView.findViewById(R.id.showComments);
            showCommentsContainer = itemView.findViewById(R.id.showCommentsContainer);
            likeText = itemView.findViewById(R.id.likes);
            likeButton = itemView.findViewById(R.id.likeButton);
            date = itemView.findViewById(R.id.date);
            tags = itemView.findViewById(R.id.tags);
            tags.setLayoutManager(new LinearLayoutManager(context, LinearLayoutManager.HORIZONTAL, false));

            commentsView.addOnItemTouchListener(new RecyclerView.OnItemTouchListener() {
                @Override
                public boolean onInterceptTouchEvent(@NonNull RecyclerView rv, @NonNull MotionEvent e) {
                    if (e.getAction() == MotionEvent.ACTION_MOVE) {
                        if (rv.canScrollVertically(-1)) {
                            rv.getParent().requestDisallowInterceptTouchEvent(true);
                        } else if (rv.canScrollVertically(1)) {
                            rv.getParent().requestDisallowInterceptTouchEvent(true);
                        }
                    } else if (e.getAction() == MotionEvent.ACTION_UP || e.getAction() == MotionEvent.ACTION_CANCEL) {
                        rv.getParent().requestDisallowInterceptTouchEvent(false);
                    }
                    return false;
                }

                @Override
                public void onTouchEvent(@NonNull RecyclerView rv, @NonNull MotionEvent e) {

                }

                @Override
                public void onRequestDisallowInterceptTouchEvent(boolean disallowIntercept) {

                }
            });

            likeButton.setOnClickListener(v -> {
                if (post.isLiked()) {
                    likeText.setText(String.valueOf(post.getLikes() - 1));
                    likeButton.setImageResource(R.drawable.heart_empty);
                } else {
                    likeText.setText(String.valueOf(post.getLikes() + 1));
                    likeButton.setImageResource(R.drawable.heart);
                }
                MainActivity.getApi().likePost(UserManager.getBearer(), post.getId()).enqueue(new Callback<Void>() {
                    @Override
                    public void onResponse(@NonNull Call<Void> call, @NonNull Response<Void> response) {
                        if (response.isSuccessful()) {
                            if (post.isLiked()) {
                                post.setLiked(false);
                                post.setLikes(post.getLikes() - 1);
                            } else {
                                post.setLiked(true);
                                post.setLikes(post.getLikes() + 1);
                            }
                            adapter.like(post.getId());
                        } else {
                            if (post.isLiked()) likeButton.setImageResource(R.drawable.heart);
                            else likeButton.setImageResource(R.drawable.heart_empty);
                            likeText.setText(String.valueOf(post.getLikes()));
                            new AlertDialog.Builder(context)
                                    .setTitle("Error liking post")
                                    .setMessage(response.message())
                                    .setPositiveButton("Ok", null)
                                    .show();
                        }
                    }

                    @Override
                    public void onFailure(@NonNull Call<Void> call, @NonNull Throwable throwable) {
                        if (post.isLiked()) likeButton.setImageResource(R.drawable.heart);
                        else likeButton.setImageResource(R.drawable.heart_empty);
                        likeText.setText(String.valueOf(post.getLikes()));
                        new AlertDialog.Builder(context)
                                .setTitle("Error liking post")
                                .setMessage(throwable.getMessage())
                                .setPositiveButton("Ok", null)
                                .show();
                    }
                });
            });

            showCommentsContainer.setOnClickListener(v -> {
                if (commentsView.getAdapter() == null) {
                    commentsView.setLayoutManager(new LinearLayoutManager(context));
                    commentsView.setAdapter(new CommentAdapter(post.getComments(), post.getId()));
                }
                if (commentsView.getVisibility() == View.GONE) {

                    commentsView.measure(
                            View.MeasureSpec.makeMeasureSpec(((View) commentsView.getParent()).getWidth(), View.MeasureSpec.AT_MOST),
                            View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED)
                    );

                    int targetHeight = commentsView.getMeasuredHeight();
                    int maxHeight  = (int)(context.getResources().getDisplayMetrics().density * 300);
                    targetHeight = Math.min(targetHeight, maxHeight);

                    commentsView.getLayoutParams().height = 0;
                    commentsView.requestLayout();

                    commentsView.setVisibility(View.VISIBLE);

                    ValueAnimator heightAnimator = ValueAnimator.ofInt(0, targetHeight);
                    heightAnimator.addUpdateListener(animation -> {
                        commentsView.getLayoutParams().height = (int) animation.getAnimatedValue();
                        commentsView.requestLayout();
                    });

                    ObjectAnimator rotationAnimator = ObjectAnimator.ofFloat(showComments, "rotation", 0f, 90f);
                    AnimatorSet animatorSet = new AnimatorSet();
                    animatorSet.playTogether(heightAnimator, rotationAnimator);
                    animatorSet.setDuration(500);
                    animatorSet.start();
                } else {
                    int initialHeight = commentsView.getMeasuredHeight();
                    ValueAnimator heightAnimator = ValueAnimator.ofInt(initialHeight, 0);
                    heightAnimator.addUpdateListener(animation -> {
                        commentsView.getLayoutParams().height = (int) animation.getAnimatedValue();
                        commentsView.requestLayout();
                    });
                    ObjectAnimator rotationAnimator = ObjectAnimator.ofFloat(showComments, "rotation", 90f, 0f);
                    AnimatorSet animatorSet = new AnimatorSet();
                    animatorSet.playTogether(heightAnimator, rotationAnimator);
                    animatorSet.setDuration(500);

                    animatorSet.addListener(new Animator.AnimatorListener() {
                        @Override
                        public void onAnimationStart(@NonNull Animator animation) {
                        }

                        @Override
                        public void onAnimationEnd(@NonNull Animator animation) {
                            commentsView.setVisibility(View.GONE);
                        }

                        @Override
                        public void onAnimationCancel(@NonNull Animator animation) {
                        }

                        @Override
                        public void onAnimationRepeat(@NonNull Animator animation) {
                        }
                    });
                    animatorSet.start();
                }
            });
        }

        public void bind(Post post) {
            this.post = post;
            username.setText(post.getUser().getUsername());
            description.setText(post.getText());

            if (post.getTags().isEmpty()) {
                tags.setVisibility(View.GONE);
            } else {
                tags.setVisibility(View.VISIBLE);
                tags.setAdapter(new TagAdapter(new ArrayList<>(post.getTags())));
            }

            commentsView.setAdapter(null);
            showComments.setRotation(0f);

            likeText.setText(String.valueOf(post.getLikes()));
            LocalDateTime localDate = post.getDate().toInstant().atZone(ZoneId.systemDefault()).toLocalDateTime();
            DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy.MM.dd HH:mm", Locale.getDefault());

            date.setText(localDate.format(formatter));

            if (post.isLiked()) likeButton.setImageResource(R.drawable.heart);
            else likeButton.setImageResource(R.drawable.heart_empty);

            Glide.with(itemView)
                    .load(String.format(Locale.getDefault(), "%susers/%d/pfp", APIInstance.BASE_URL, post.getUser().getId()))
                    .signature(GlideUtils.getSignature())
                    .into(profilePicture);
            if (post.getImageId() != null)
                Glide.with(itemView)
                    .load(String.format(Locale.getDefault(), "%simages/%d", APIInstance.BASE_URL, post.getImageId()))
                    .into(image);
            else image.setImageResource(0);
        }
    }


    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.post_layout, parent, false);
        return new PostViewHolder(itemView, itemView.getContext(), parent, this);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        Post post = items.get(position);
        ((PostViewHolder)holder).bind(post);
    }

    @Override
    public int getItemCount() {
        return items.size();
    }
}
