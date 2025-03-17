package com.example.colorsync;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.example.colorsync.DataTypes.Comment;

import java.util.List;
import java.util.Locale;

public class CommentAdapter extends RecyclerView.Adapter<CommentAdapter.ViewHolder> {
    private final List<Comment> items;

    public CommentAdapter(List<Comment> items) {
        this.items = items;
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        public TextView username;
        public TextView description;
        public ImageView profilePicture;

        public ViewHolder(@NonNull View itemView) {
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

    @NonNull
    @Override
    public CommentAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.comment_layout, parent, false);
        return new CommentAdapter.ViewHolder(itemView);
    }


    @Override
    public void onBindViewHolder(@NonNull CommentAdapter.ViewHolder holder, int position) {
        Comment comment = items.get(position);
        holder.bind(comment);
    }

    @Override
    public int getItemCount() {
        return items.size();
    }
}
