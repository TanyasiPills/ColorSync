package com.example.colorsync;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;

import java.util.List;

public class ScrollAdapter extends RecyclerView.Adapter<ScrollAdapter.ViewHolder>{
    private final List<Post> items;

    public ScrollAdapter(List<Post> items) {
        this.items = items;
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        public TextView username;
        public TextView description;
        public ImageView profilePicture;
        public ImageView image;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            username = itemView.findViewById(R.id.username);
            description = itemView.findViewById(R.id.description);
            profilePicture = itemView.findViewById(R.id.profile_picture);
            image = itemView.findViewById(R.id.image);
        }

        public void bind(Post post) {
            username.setText(post.getUser().getUsername());
            description.setText(post.getText());
            Glide.with(itemView)
                    .load("http://10.0.2.2:3000/users/" + post.getUserId() + "/pfp")
                    .into(profilePicture);
            Glide.with(itemView)
                    .load("http://10.0.2.2:3000/images/public/" + post.getImageId())
                    .into(image);
        }
    }

    @NonNull
    @Override
    public ScrollAdapter.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.post_layout, parent, false);
        return new ViewHolder(itemView);
    }

    @Override
    public void onBindViewHolder(@NonNull ScrollAdapter.ViewHolder holder, int position) {
        Post post = items.get(position);
        holder.bind(post);
    }

    @Override
    public int getItemCount() {
        return items.size();
    }

}
