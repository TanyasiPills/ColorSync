package com.example.colorsync;

import android.content.Context;

import androidx.datastore.core.DataStore;
import androidx.datastore.preferences.core.MutablePreferences;
import androidx.datastore.preferences.core.PreferenceDataStoreFactory;
import androidx.datastore.preferences.core.Preferences;
import androidx.datastore.preferences.core.PreferencesKeys;
import androidx.datastore.preferences.rxjava3.RxPreferenceDataStoreBuilder;
import androidx.datastore.rxjava3.RxDataStore;
import androidx.security.crypto.EncryptedFile;
import androidx.security.crypto.MasterKeys;

import java.io.File;
import java.io.IOException;
import java.security.GeneralSecurityException;
import java.util.Objects;

import io.reactivex.rxjava3.core.Flowable;
import io.reactivex.rxjava3.core.Single;

public class UserManager {
    private static final String FILE_NAME = "user_data";
    private static final Preferences.Key<String> TOKEN_KEY = PreferencesKeys.stringKey("user_token");
    private static RxDataStore<Preferences> dataStore;

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
}
