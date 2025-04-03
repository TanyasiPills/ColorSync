package com.example.colorsync;

import android.annotation.SuppressLint;
import android.content.Context;
import android.util.AttributeSet;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.SearchView;

public class SearchViewWithEmpty extends SearchView {
    @SuppressLint("RestrictedApi")
    SearchView.SearchAutoComplete autoComplete;

    public SearchViewWithEmpty(@NonNull Context context) {
        super(context);
    }

    public SearchViewWithEmpty(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    public SearchViewWithEmpty(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override public void setOnQueryTextListener(OnQueryTextListener listener) {
        super.setOnQueryTextListener(listener);
        autoComplete = this.findViewById(androidx.appcompat.R.id.search_src_text);
        autoComplete.setOnEditorActionListener((textView, i, keyEvent) -> {
            if (listener != null) {
                listener.onQueryTextSubmit(getQuery().toString());
            }
            MainActivity.getInstance().hideKeyboard();
            return true;
        });
    }
}
