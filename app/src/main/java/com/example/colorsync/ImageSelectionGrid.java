package com.example.colorsync;

import android.net.Uri;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.bumptech.glide.Glide;
import com.example.colorsync.DataTypes.ImageData;

import java.util.List;
import java.util.function.Function;

public class ImageSelectionGrid extends  RecyclerView.Adapter<ImageSelectionGrid.ImageViewHolder> {
    private List<Uri> imageUriLIst;
    private List<ImageData> imageDataList;
    private boolean uri;
    private Function<Uri, Void> uriClickHandler;
    private Function<ImageData, Void> dataClickHandler;
    public long selected;

    public ImageSelectionGrid(List<Uri> imageUriLIst, List<ImageData> imageDataList, Function<Uri, Void> uriClickHandler, Function<ImageData, Void> dataClickHandler ) {
        this.imageUriLIst = imageUriLIst;
        this.imageDataList = imageDataList;
        this.uriClickHandler = uriClickHandler;
        this.dataClickHandler = dataClickHandler;
        uri = true;
        selected = -1;
    }

    public void changeToUri() {
        uri = true;
        selected = -1;
        notifyDataSetChanged();
    }

    public void changeToData() {
        uri = false;
        selected = -1;
        notifyDataSetChanged();
    }

    public void selectionHandler(int position) {
        selected = position;
        if (uri) uriClickHandler.apply(imageUriLIst.get(position));
        else dataClickHandler.apply(imageDataList.get(position));
        notifyDataSetChanged();
    }

    public void deselect() {
        selected = -1;
        notifyDataSetChanged();
    }

    public static class ImageViewHolder extends RecyclerView.ViewHolder {
        public ConstraintLayout imageLayout;
        public ImageView imageView;
        public Uri uri;
        public ImageData imageData;

        public ImageViewHolder(View itemView) {
            super(itemView);
            imageView = itemView.findViewById(R.id.image);
            imageLayout = itemView.findViewById(R.id.imageLayout);
            ViewGroup.LayoutParams layoutParams = imageView.getLayoutParams();
            int size = (int) ((imageView.getContext().getResources().getDisplayMetrics().widthPixels / 2.0) -
                    TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 50, imageView.getContext().getResources().getDisplayMetrics()));
            layoutParams.width = size;
            layoutParams.height = size;
            imageView.setLayoutParams(layoutParams);
        }

        public void bind(Uri uri, int position, ImageSelectionGrid grid) {
            this.uri = uri;
            this.imageData = null;

            Glide.with(imageView.getContext())
                    .load(uri)
                    .centerCrop()
                    .into(imageView);

            if (grid.selected == position) {
                imageLayout.setBackgroundColor(itemView.getResources().getColor(R.color.bgHighlight));
            } else {
                imageLayout.setBackgroundColor(0);
            }
            imageView.setOnClickListener(view -> {
                grid.selectionHandler(position);
            });
        }

        public void bind(ImageData imageData, int position, ImageSelectionGrid grid) {
            this.imageData = imageData;
            this.uri = null;

            Glide.with(imageView.getContext())
                    .load(APIInstance.BASE_URL + "images/" + imageData.id)
                    .centerCrop()
                    .into(imageView);

            if (grid.selected == position) {
                imageLayout.setBackgroundColor(itemView.getResources().getColor(R.color.bgHighlight));
            } else {
                imageLayout.setBackgroundColor(0);
            }
            imageView.setOnClickListener(view -> {
                grid.selectionHandler(position);
            });
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
        if (uri) {
            Uri item = imageUriLIst.get(position);
            holder.bind(item, position, this);
        } else {
            ImageData item = imageDataList.get(position);
            holder.bind(item, position, this);
        }
    }

    @Override
    public int getItemCount() {
        if (uri) return imageUriLIst.size();
        else return imageDataList.size();
    }
}
