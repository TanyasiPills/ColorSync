package com.example.colorsync;

import android.annotation.SuppressLint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

public class TagAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private final ArrayList<String> items;
    private final boolean removable;

    public TagAdapter(ArrayList<String> tags, boolean removable) {
        this.items = tags;
        this.removable = removable;
    }

    public TagAdapter(ArrayList<String> tags) {
        this.items = tags;
        this.removable = false;
    }

    public void insertItem(String item, int position) {
        items.add(position, item);
        notifyItemInserted(position);
    }

    @SuppressLint("NotifyDataSetChanged")
    public void clear() {
        items.clear();
        notifyDataSetChanged();
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.tag_layout, parent, false);
        return new TagViewHolder(view, this, removable);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        String item = items.get(position);
        ((TagViewHolder) holder).bind(item);
    }

    @Override
    public int getItemCount() {
        return items.size();
    }

    public static class TagViewHolder extends RecyclerView.ViewHolder {
        private final TextView tag;

        public TagViewHolder(@NonNull View itemView, TagAdapter adapter, boolean removable) {
            super(itemView);
            tag = itemView.findViewById(R.id.tag);
            if (removable) {
                ImageButton remove = itemView.findViewById(R.id.remove);
                remove.setVisibility(View.VISIBLE);
                remove.setOnClickListener(view -> {
                    int position = getAdapterPosition();
                    adapter.items.remove(position);
                    adapter.notifyItemRemoved(position);
                });
            }
        }

        public void bind(String tag) {
            this.tag.setText(tag);
        }
    }
}
