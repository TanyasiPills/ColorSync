package com.example.colorsync;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;

import androidx.appcompat.widget.AppCompatImageView;

public class MaskedImageView extends AppCompatImageView {
    private float maskWidth;
    private float maskHeight;

    public MaskedImageView(Context context) {
        super(context);
    }

    public MaskedImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public MaskedImageView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public void setMask(float maskWidth, float maskHeight) {
        this.maskWidth = maskWidth;
        this.maskHeight = maskHeight;
        invalidate();
    }

    @Override
    public void draw(Canvas canvas) {
        if (getDrawable() == null || maskWidth <= 0 || maskHeight <= 0) {
            super.draw(canvas);
            return;
        }

        Drawable drawable = getDrawable();
        int imageWidth = drawable.getIntrinsicWidth();
        int imageHeight = drawable.getIntrinsicHeight();

        int viewWidth = getWidth();
        int viewHeight = getHeight();

        float scaleX = (float) viewWidth / imageWidth;
        float scaleY = (float) viewHeight / imageHeight;
        float scale = Math.min(scaleX, scaleY);

        int scaledWidth = (int) (imageWidth * scale);
        int scaledHeight = (int) (imageHeight * scale);

        int offsetX = (viewWidth - scaledWidth) / 2;
        int offsetY = (viewHeight - scaledHeight) / 2;

        float maskOffsetX = (viewWidth - maskWidth) / 2;
        float maskOffsetY = (viewHeight - maskHeight) / 2;
        canvas.clipRect(new RectF(maskOffsetX, maskOffsetY, maskOffsetX + maskWidth, maskOffsetY + maskHeight));

        drawable.setBounds(offsetX, offsetY, offsetX + scaledWidth,offsetY + scaledHeight);

        drawable.draw(canvas);
    }
}
