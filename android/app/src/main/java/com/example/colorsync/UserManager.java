package com.example.colorsync;

import android.content.Context;

import androidx.datastore.preferences.core.MutablePreferences;
import androidx.datastore.preferences.core.Preferences;
import androidx.datastore.preferences.core.PreferencesKeys;
import androidx.datastore.preferences.rxjava3.RxPreferenceDataStoreBuilder;
import androidx.datastore.rxjava3.RxDataStore;

import com.example.colorsync.DataType.User;

import java.util.Objects;

import io.reactivex.rxjava3.core.Completable;
import io.reactivex.rxjava3.core.Flowable;
import io.reactivex.rxjava3.core.Single;

public class UserManager {
    private static final String FILE_NAME = "user_data";
    private static final Preferences.Key<String> TOKEN_KEY = PreferencesKeys.stringKey("user_token");
    private static RxDataStore<Preferences> dataStore;
    public static String token;
    public static User user;

    public static String getBearer() {
        return "Bearer " + token;
    }

    private static RxDataStore<Preferences> getInstance(Context context) {
        if (dataStore == null) {
            synchronized (UserManager.class) {
                if (dataStore == null) {
                    dataStore = new RxPreferenceDataStoreBuilder(
                            context.getApplicationContext(),
                            FILE_NAME
                    ).build();
                }
            }
        }
        return dataStore;
    }

    public static Single<Preferences> saveToken(Context context, String token) {
        return getInstance(context).updateDataAsync(preferences -> {
            MutablePreferences mutablePreferences = preferences.toMutablePreferences();
            mutablePreferences.set(TOKEN_KEY, token);
            return Single.just(mutablePreferences);
        });
    }

    public static Flowable<String> loadToken(Context context) {
        return getInstance(context).data()
                .map(preferences -> preferences.get(TOKEN_KEY))
                .filter(Objects::nonNull);
    }

    public static Completable logout(Context context) {
        return getInstance(context).updateDataAsync(preferences -> {
            MutablePreferences mutablePreferences = preferences.toMutablePreferences();
            mutablePreferences.remove(TOKEN_KEY); // Remove token from storage
            return Single.just(mutablePreferences);
        }).ignoreElement()
                .doOnComplete(() -> {
                    token = null;
                    user = null;
                });
    }
}
