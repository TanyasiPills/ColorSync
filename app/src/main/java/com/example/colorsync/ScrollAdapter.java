package com.example.colorsync;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.example.colorsync.DataTypes.Post;

import java.util.List;
import java.util.Locale;

public class ScrollAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private final List<Post> items;
    private static final int ADD_POST_TYPE = 0;
    private static final int POST_TYPE = 1;

    public ScrollAdapter(List<Post> items) {
        this.items = items;
    }

    public static class PostViewHolder extends RecyclerView.ViewHolder {
        private final TextView username;
        private final TextView description;
        private final ImageView profilePicture;
        private final ImageView image;
        private final RecyclerView commentsView;
        private final ImageView showComments;
        private final Context context;
        private final LinearLayout showCommentsContainer;
        private Post post;

        public PostViewHolder(@NonNull View itemView, Context context, ViewGroup parent) {
            super(itemView);
            this.context = context;
            username = itemView.findViewById(R.id.username);
            description = itemView.findViewById(R.id.description);
            profilePicture = itemView.findViewById(R.id.profile_picture);
            image = itemView.findViewById(R.id.image);
            commentsView = itemView.findViewById(R.id.commentsView);
            showComments = itemView.findViewById(R.id.showComments);
            showCommentsContainer = itemView.findViewById(R.id.showCommentsContainer);

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

            showCommentsContainer.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (commentsView.getAdapter() == null) {
                        commentsView.setLayoutManager(new LinearLayoutManager(context));
                        commentsView.setAdapter(new CommentAdapter(post.getComments()));
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
                }
            });
        }

        public void bind(Post post) {
            //TODO: add the add comment to the top
            this.post = post;
            username.setText(post.getUser().getUsername());
            description.setText(post.getText());

            commentsView.setAdapter(null);
            showComments.setRotation(0f);

            Glide.with(itemView)
                    .load(String.format(Locale.getDefault(), "%susers/%d/pfp", APIInstance.BASE_URL, post.getUser().getId()))
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
        return new PostViewHolder(itemView, itemView.getContext(), parent);
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
