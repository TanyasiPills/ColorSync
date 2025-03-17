package com.example.colorsync;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.provider.OpenableColumns;
import android.util.Log;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Paths;

public class FileUtils {
    public static File getFileFromUri(Context context, Uri uri) throws IOException {
        Log.println(Log.ASSERT, "asd", uri.toString());
        File file = new File(context.getCacheDir(), getFileName(context, uri));

        InputStream inputStream = context.getContentResolver().openInputStream(uri);
        if (inputStream != null) {
            Files.copy(inputStream, Paths.get(file.getPath()));
            inputStream.close();
        }

        return file;
    }

    private static String getFileName(Context context, Uri uri) {
        String result = null;
        if (uri.getScheme().equals("content")) {
            try (android.database.Cursor cursor = context.getContentResolver().query(uri, null, null, null, null)) {
                if (cursor != null && cursor.moveToFirst()) {
                    int index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                    if (index == -1) return null;
                    result = cursor.getString(index);
                }
            }
        }
        if (result == null) {
            result = uri.getPath();
            int cut = result.lastIndexOf('/');
            if (cut != -1) {
                result = result.substring(cut + 1);
            }
        }
        return result;
    }
}
