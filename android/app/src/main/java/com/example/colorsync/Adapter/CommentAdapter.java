package com.example.colorsync.Adapter;

import android.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.example.colorsync.APIInstance;
import com.example.colorsync.DataType.Comment;
import com.example.colorsync.DataType.CreateComment;
import com.example.colorsync.GlideUtils;
import com.example.colorsync.MainActivity;
import com.example.colorsync.R;
import com.example.colorsync.UserManager;

import java.util.List;
import java.util.Locale;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class CommentAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private final List<Comment> items;
    private final int postId;
    private static final int VIEW_TYPE_ADD_COMMENT = 0;
    private static final int VIEW_TYPE_COMMENT = 1;

    public CommentAdapter(List<Comment> items,  int postId) {
        this.items = items;
        this.postId = postId;
    }

    private void addComment(Comment comment) {
        items.add(comment);
        notifyItemInserted(items.size());
    }


    public static class CommentViewHolder extends RecyclerView.ViewHolder {
        public TextView username;
        public TextView description;
        public ImageView profilePicture;

        public CommentViewHolder(@NonNull View itemView) {
            super(itemView);
            username = itemView.findViewById(R.id.username);
            description = itemView.findViewById(R.id.description);
            profilePicture = itemView.findViewById(R.id.profile_picture);
        }

        public void bind(Comment comment) {
            username.setText(comment.getUser().getUsername());
            description.setText(comment.getText());
            Glide.with(itemView)
                    .load(String.format(Locale.getDefault(), "%susers/%d/pfp", APIInstance.BASE_URL, comment.getUser().getId()))
                    .into(profilePicture);
        }
    }

    public static class AddCommentViewHolder extends RecyclerView.ViewHolder {
        public TextView username;
        public EditText description;
        public ImageView profilePicture;
        public ImageButton createComment;
        public CommentAdapter adapter;

        public AddCommentViewHolder(@NonNull View itemView, CommentAdapter adapter) {
            super(itemView);
            username = itemView.findViewById(R.id.username);
            description = itemView.findViewById(R.id.description);
            profilePicture = itemView.findViewById(R.id.profile_picture);
            createComment = itemView.findViewById(R.id.createComment);
            this.adapter = adapter;

            createComment.setOnClickListener(view -> {
                String text = description.getText().toString();
                if (text.isEmpty()) {
                    Animation shake = AnimationUtils.loadAnimation(itemView.getContext(), R.anim.shake);
                    description.startAnimation(shake);
                    return;
                }
                MainActivity.getInstance().hideKeyboard();
                MainActivity.getApi().createComment(UserManager.getBearer(), new CreateComment(adapter.postId, text)).enqueue(new Callback<Comment>() {
                    @Override
                    public void onResponse(@NonNull Call<Comment> call, @NonNull Response<Comment> response) {
                        if (response.isSuccessful() && response.body() != null) {
                            adapter.addComment(response.body());
                            description.setText("");
                        } else {
                            new AlertDialog.Builder(itemView.getContext())
                                    .setTitle("Error creating comment")
                                    .setMessage(response.message())
                                    .setPositiveButton("OK", null)
                                    .show();
                        }
                    }

                    @Override
                    public void onFailure(@NonNull Call<Comment> call, @NonNull Throwable throwable) {
                        new AlertDialog.Builder(itemView.getContext())
                                .setTitle("Error creating comment")
                                .setMessage(throwable.getMessage())
                                .setPositiveButton("OK", null)
                                .show();
                    }
                });
            });
        }

        public void bind() {
            username.setText(UserManager.user.getUsername());
            Glide.with(itemView)
                    .load(String.format(Locale.getDefault(), "%susers/%d/pfp", APIInstance.BASE_URL, UserManager.user.getId()))
                    .signature(GlideUtils.getSignature())
                    .into(profilePicture);
        }
    }

    @Override
    public int getItemViewType(int position) {
        if (position == 0) {
            return VIEW_TYPE_ADD_COMMENT;
        } else {
            return VIEW_TYPE_COMMENT;
        }
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        if (viewType == VIEW_TYPE_ADD_COMMENT) {
            View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.add_comment_layout, parent, false);
            return new CommentAdapter.AddCommentViewHolder(itemView, this);
        } else {
            View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.comment_layout, parent, false);
            return new CommentAdapter.CommentViewHolder(itemView);
        }
    }


    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        if (holder instanceof CommentViewHolder) {
            Comment comment = items.get(position - 1);
            ((CommentViewHolder) holder).bind(comment);
        } else if (holder instanceof AddCommentViewHolder) {
            ((AddCommentViewHolder) holder).bind();
        }
    }

    @Override
    public int getItemCount() {
        return items.size() + 1;
    }
}
