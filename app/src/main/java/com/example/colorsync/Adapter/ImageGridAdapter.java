package com.example.colorsync.Adapter;

import android.graphics.drawable.Drawable;
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
import com.example.colorsync.APIInstance;
import com.example.colorsync.DataType.ImageData;
import com.example.colorsync.R;
import com.example.colorsync.UserManager;

import java.util.List;

public class ImageGridAdapter extends  RecyclerView.Adapter<ImageGridAdapter.ImageViewHolder> {
    private List<ImageData> items;
    private int padding;
    public ImageGridAdapter(List<ImageData> items, int padding) {
        this.items = items;
        this.padding = padding;
    }

    public static class ImageViewHolder extends RecyclerView.ViewHolder {
        public ImageView imageView;
        public ImageData item;

        public ImageViewHolder(View itemView, int padding) {
            super(itemView);
            imageView = itemView.findViewById(R.id.image);
            setImageSize(padding);
        }

        public void setImageSize(int padding) {
            ViewGroup.LayoutParams layoutParams = imageView.getLayoutParams();
            int size = (int) ((imageView.getContext().getResources().getDisplayMetrics().widthPixels / 2.0) -
                    TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, padding, imageView.getContext().getResources().getDisplayMetrics()));
            layoutParams.width = size;
            layoutParams.height = size;
            imageView.setLayoutParams(layoutParams);
        }

        public void bind(ImageData imageData) {
            this.item = imageData;

            GlideUrl url = new GlideUrl(
                    APIInstance.BASE_URL + "images/" + item.id,
                    new LazyHeaders.Builder()
                            .addHeader("Authorization", "Bearer " + UserManager.token)
                            .build());

            Glide.with(imageView.getContext())
                    .load(url)
                    .listener(new RequestListener<Drawable>() {
                        @Override
                        public boolean onLoadFailed(@Nullable GlideException e, Object model, Target<Drawable> target, boolean isFirstResource) {
                            if (e != null) Toast.makeText(imageView.getContext(), e.getMessage(), Toast.LENGTH_SHORT).show();
                            return false;
                        }

                        @Override
                        public boolean onResourceReady(Drawable resource, Object model, Target<Drawable> target, DataSource dataSource, boolean isFirstResource) {
                            imageView.requestLayout();
                            return false;
                        }
                    })
                    .into(imageView);
        }


    }


    @NonNull
    @Override
    public ImageViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.image_item, parent, false);
        return new ImageViewHolder(itemView, padding);
    }

    @Override
    public void onBindViewHolder(@NonNull ImageViewHolder holder, int position) {
        ImageData item = items.get(position);
        holder.bind(item);
    }

    @Override
    public int getItemCount() {
        return items.size();
    }
}
