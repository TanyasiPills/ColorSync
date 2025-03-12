package com.example.colorsync;

import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.bumptech.glide.load.DataSource;
import com.bumptech.glide.load.engine.GlideException;
import com.bumptech.glide.load.model.GlideUrl;
import com.bumptech.glide.load.model.LazyHeaders;
import com.bumptech.glide.request.RequestListener;
import com.bumptech.glide.request.target.Target;
import com.example.colorsync.DataTypes.ImageData;

import java.util.List;

public class ImageSelectionGrid extends  RecyclerView.Adapter<ImageSelectionGrid.ImageViewHolder> {
    private List<Uri> items;
    public ImageSelectionGrid(List<Uri> items) {
        this.items = items;
    }

    public static class ImageViewHolder extends RecyclerView.ViewHolder {
        public ImageView imageView;
        public Uri item;

        public ImageViewHolder(View itemView) {
            super(itemView);
            imageView = itemView.findViewById(R.id.image);
            ViewGroup.LayoutParams layoutParams = imageView.getLayoutParams();
            int size = (int) ((imageView.getContext().getResources().getDisplayMetrics().widthPixels / 2.0) -
                    TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 30, imageView.getContext().getResources().getDisplayMetrics()));

            layoutParams.width = size;
            layoutParams.height = size;
            imageView.setLayoutParams(layoutParams);
        }

        public void bind(Uri imageData) {
            this.item = imageData;

            Glide.with(imageView.getContext())
                    .load(item)
                    .centerCrop()
                    .into(imageView);
        }


    }


    @NonNull
    @Override
    public ImageViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.image_item, parent, false);
        return new ImageViewHolder(itemView);
    }

    @Override
    public void onBindViewHolder(@NonNull ImageViewHolder holder, int position) {
        Uri item = items.get(position);
        holder.bind(item);
    }

    @Override
    public int getItemCount() {
        return items.size();
    }
}
