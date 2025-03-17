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
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Paths;

public class  FileUtils {
    public static File getFileFromUri(Context context, Uri uri) throws IOException {
        Log.println(Log.ASSERT, "asd", uri.toString());
        String name = getFileName(context, uri);
        if (name == null) throw new IOException("File not found");

        File file = new File(context.getCacheDir(), name);

        if (file.exists()) {
            file.delete();
        }

        boolean fileCreated = file.createNewFile();
        if (!fileCreated) {
            throw new IOException("Failed to create file in cache directory");
        }

        try (InputStream inputStream = context.getContentResolver().openInputStream(uri);
             OutputStream outputStream = new FileOutputStream(file)) {

            if (inputStream == null) {
                throw new IOException("Failed to open input stream from URI");
            }

            byte[] buffer = new byte[1024];
            int length;
            while ((length = inputStream.read(buffer)) > 0) {
                outputStream.write(buffer, 0, length);
            }
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
