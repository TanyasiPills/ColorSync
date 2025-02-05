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

            Glide.with(itemView)
                    .load("https://m.media-amazon.com/images/S/pv-target-images/309f98728a1249ac33aeaeeaa618ca331175a6ee0b792dd273ff1f24badccace._SX1080_FMjpg_.jpg")
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
        Post currentItem = items.get(position);
        holder.username.setText(String.valueOf(items.indexOf(currentItem)));
    }

    @Override
    public int getItemCount() {
        return items.size();
    }

}
