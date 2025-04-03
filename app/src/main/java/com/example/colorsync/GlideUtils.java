package com.example.colorsync;

import com.bumptech.glide.signature.ObjectKey;

public class GlideUtils {
    private static ObjectKey signature;
    public static ObjectKey getSignature() {
        if (signature == null) return changeSignature();
        return signature;
    }

    public static ObjectKey changeSignature() {
        signature = new ObjectKey(System.currentTimeMillis());
        return signature;
    }
}
