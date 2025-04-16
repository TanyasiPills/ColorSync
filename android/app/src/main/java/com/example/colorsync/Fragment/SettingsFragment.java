package com.example.colorsync.Fragment;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.ContentUris;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.cardview.widget.CardView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.provider.MediaStore;
import android.text.Editable;
import android.text.TextWatcher;
import android.transition.Fade;
import android.transition.Transition;
import android.transition.TransitionManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;

import com.bumptech.glide.Glide;
import com.example.colorsync.APIInstance;
import com.example.colorsync.APIService;
import com.example.colorsync.Adapter.ImageSelectionGrid;
import com.example.colorsync.DataType.User;
import com.example.colorsync.FileUtils;
import com.example.colorsync.GlideUtils;
import com.example.colorsync.MainActivity;
import com.example.colorsync.R;
import com.example.colorsync.UserManager;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicBoolean;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class SettingsFragment extends Fragment {
    private ConstraintLayout layout;
    private ImageButton back;
    private ImageButton logout;
    private ImageView profilePicture;
    private ImageButton profilePictureUndo;
    private EditText username;
    private ImageButton usernameUndo;
    private EditText description;
    private ImageButton descriptionUndo;
    private EditText email;
    private ImageButton emailUndo;
    private Button change;
    private boolean changed;
    private boolean upload;


    private ImageButton uploadCancel;
    private RecyclerView uploadImages;
    private CardView imagesContainer;
    private Cursor cursor;
    private List<Uri> uris;
    private boolean isLoadingUris;
    private ImageSelectionGrid adapter;

    private Uri selected;

    public SettingsFragment() {}

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @SuppressLint("CheckResult")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_settings, container, false);
        User user = UserManager.user;

        changed = false;
        uris = new ArrayList<>();

        back = view.findViewById(R.id.back);
        logout = view.findViewById(R.id.logout);
        profilePicture = view.findViewById(R.id.profilePicture);
        profilePictureUndo = view.findViewById(R.id.profilePictureUndo);
        username = view.findViewById(R.id.username);
        usernameUndo = view.findViewById(R.id.usernameUndo);
        description = view.findViewById(R.id.description);
        descriptionUndo = view.findViewById(R.id.descriptionUndo);
        email = view.findViewById(R.id.email);
        emailUndo = view.findViewById(R.id.emailUndo);
        change = view.findViewById(R.id.change);
        layout = view.findViewById(R.id.settingsLayout);

        uploadCancel = view.findViewById(R.id.uploadCancel);
        uploadImages = view.findViewById(R.id.uploadImages);
        imagesContainer = view.findViewById(R.id.imagesContainer);

        adapter = new ImageSelectionGrid(uris, null, this::selectionHandler, null, 50);
        uploadImages.setAdapter(adapter);
        uploadImages.setLayoutManager(new GridLayoutManager(requireContext(), 2));

        uploadImages.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrolled(@NonNull RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);

                if (isLoadingUris) return;
                GridLayoutManager layoutManager = (GridLayoutManager) recyclerView.getLayoutManager();

                int visibleItemCount = Objects.requireNonNull(layoutManager).getChildCount();
                int totalItemCount = layoutManager.getItemCount();
                int pastVisibleItems = layoutManager.findFirstVisibleItemPosition();

                if ((visibleItemCount + pastVisibleItems) >= totalItemCount - 10) {
                    loadMoreUris();
                }
            }
        });

        back.setOnClickListener(v -> {
            MainActivity.getInstance().goToProfile();
        });

        logout.setOnClickListener(v -> {
            UserManager.logout(getContext())
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribe(() -> {
                        MainActivity.getInstance().login();
                    }, throwable -> {
                        new AlertDialog.Builder(getContext())
                                .setTitle("Error logging out")
                                .setMessage(throwable.getMessage())
                                .setPositiveButton("OK", null)
                                .show();
                    });
        });

        username.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                if (charSequence.toString().isEmpty())
                    rotateIn(usernameUndo);
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void afterTextChanged(Editable editable) {
                if (!changed && !editable.toString().isEmpty()) {
                    setChanged();
                }
                else if (changed && editable.toString().isEmpty()) {
                    rotateOut(usernameUndo);
                    setChangedOff();
                }
            }
        });

        description.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                if (charSequence.toString().isEmpty())
                    rotateIn(descriptionUndo);
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void afterTextChanged(Editable editable) {
                if (!changed && !editable.toString().isEmpty()) {
                    setChanged();
                }
                else if (changed && editable.toString().isEmpty()) {
                    rotateOut(descriptionUndo);
                    setChangedOff();
                }
            }
        });

        email.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                if (charSequence.toString().isEmpty())
                    rotateIn(emailUndo);
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void afterTextChanged(Editable editable) {
                if (!changed && !editable.toString().isEmpty()) {
                    setChanged();
                }
                else if (changed && editable.toString().isEmpty()) {
                    rotateOut(emailUndo);
                    setChangedOff();
                }
            }
        });

        usernameUndo.setOnClickListener(v -> {
            username.setText("");
        });

        descriptionUndo.setOnClickListener(v -> {
            description.setText("");
        });

        emailUndo.setOnClickListener(v -> {
            email.setText("");
        });

        profilePictureUndo.setOnClickListener(view1 -> {
            rotateOut(profilePictureUndo);
            Glide.with(view)
                    .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
                    .signature(GlideUtils.getSignature())
                    .into(profilePicture);
            adapter.deselect();
            selected = null;
            setChangedOff();
        });

        profilePicture.setOnClickListener(v -> {
            upload();
        });

        uploadCancel.setOnClickListener(v -> {
            upload();
        });

        change.setOnClickListener(v -> {
            if (!changed && selected == null) return;
            APIService api = MainActivity.getApi();
            String token = UserManager.getBearer();
            String username = this.username.getText().toString();
            String email = this.email.getText().toString();
            String description = this.description.getText().toString();
            if (username.isEmpty()) username = null;
            if (email.isEmpty()) email = null;
            if (description.isEmpty()) description = null;
            AtomicBoolean finished = new AtomicBoolean(selected != null ^ changed);

            api.updateUser(token, new User(0, username, email, description)).enqueue(new Callback<User>() {
                @Override
                public void onResponse(Call<User> call, Response<User> response) {
                    if (response.isSuccessful() && response.body() != null) {
                        UserManager.user = response.body();
                        if (finished.getAndSet(true)) MainActivity.getInstance().goToProfile();
                    } else {
                        new AlertDialog.Builder(getContext())
                                .setTitle("Error updating userinfo")
                                .setMessage(response.message())
                                .setPositiveButton("OK", null)
                                .show();
                    }
                }

                @Override
                public void onFailure(Call<User> call, Throwable throwable) {
                    new AlertDialog.Builder(getContext())
                            .setTitle("Error updating userinfo")
                            .setMessage(throwable.getMessage())
                            .setPositiveButton("OK", null)
                            .show();
                }
            });
            if (selected != null) {
                File file;
                try {
                    file = FileUtils.getFileFromUri(getContext(), selected);
                } catch (IOException e) {
                    new android.app.AlertDialog.Builder(getContext())
                            .setTitle("Failed to open image")
                            .setMessage(e.getMessage())
                            .setPositiveButton("OK", null)
                            .show();
                    return;
                }
                RequestBody fileRequest = RequestBody.create(MediaType.parse("image/*"), file);
                MultipartBody.Part fileBody = MultipartBody.Part.createFormData("file", file.getName(), fileRequest);

                MainActivity.getApi().uploadPfp(UserManager.getBearer(), fileBody).enqueue(new Callback<Void>() {
                    @Override
                    public void onResponse(Call<Void> call, Response<Void> response) {
                        if (response.isSuccessful()) {
                            GlideUtils.changeSignature();
                            if (finished.getAndSet(true)) MainActivity.getInstance().goToProfile();
                        } else new AlertDialog.Builder(getContext())
                                .setTitle("Failed to change profile picture")
                                .setMessage(response.message())
                                .setPositiveButton("Ok", null)
                                .show();
                    }

                    @Override
                    public void onFailure(Call<Void> call, Throwable throwable) {
                        new AlertDialog.Builder(getContext())
                                .setTitle("Failed to change profile picture")
                                .setMessage(throwable.getMessage())
                                .setPositiveButton("Ok", null)
                                .show();
                    }
                });
            }
        });

        Glide.with(view)
                .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
                .signature(GlideUtils.getSignature())
                .into(profilePicture);

        username.setHint(user.getUsername());
        description.setHint(user.getProfile_description());
        email.setHint(user.getEmail());

        loadMoreUris();
        return view;
    }

    private Void selectionHandler(Uri uri) {
        selected = uri;
        upload();
        return null;
    }

    private void upload() {
        upload = !upload;

        if (upload) {
            imagesContainer.setVisibility(View.VISIBLE);
            Animation animation = AnimationUtils.loadAnimation(getContext(), R.anim.grow_in);
            imagesContainer.startAnimation(animation);
        }
        else {
            if (selected != null) {
                Glide.with(requireContext())
                        .load(selected)
                        .centerCrop()
                        .into(profilePicture);
                if (profilePictureUndo.getVisibility() != View.VISIBLE) rotateIn(profilePictureUndo);
                if (!changed) setChanged();
            }
            Animation animation = AnimationUtils.loadAnimation(getContext(), R.anim.grow_out);
            animation.setAnimationListener(new Animation.AnimationListener() {
                @Override
                public void onAnimationStart(Animation animation) {

                }

                @Override
                public void onAnimationEnd(Animation animation) {
                    imagesContainer.setVisibility(View.GONE);
                }

                @Override
                public void onAnimationRepeat(Animation animation) {

                }
            });
            imagesContainer.startAnimation(animation);
        }
    }

    public void loadMoreUris() {
        if (isLoadingUris) return;
        isLoadingUris = true;

        Uri collection;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            collection = MediaStore.Images.Media.getContentUri(MediaStore.VOLUME_EXTERNAL);
        } else {
            collection = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
        }
        String[] projection = {
                MediaStore.Images.Media._ID,
                MediaStore.Images.Media.DISPLAY_NAME
        };

        if (cursor == null) {
            cursor = MainActivity.getInstance().getContentResolver().query(
                    collection,
                    projection,
                    null, null,
                    MediaStore.Images.Media.DATE_ADDED + " DESC"
            );
        }
        if (cursor == null) return;

        int start = uris.size();
        int i = 0;
        for (; i < 10; i++) {
            if (!cursor.moveToNext()) break;
            int colIndex = cursor.getColumnIndex(MediaStore.Images.Media._ID);
            if (colIndex < 0) continue;
            long id = cursor.getLong(colIndex);
            Uri uri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id);
            uris.add(uri);
        }

        adapter.notifyItemRangeInserted(start, i);
        isLoadingUris = false;
    }

    private void setChangedOff() {
        if (changed && username.getText().toString().isEmpty() && description.getText().toString().isEmpty())
            setChanged();
    }

    private void setChanged() {
        changed = !changed;
        ConstraintSet off = new ConstraintSet();
        off.clone(layout);
        ConstraintSet on = new ConstraintSet();
        on.clone(layout);
        on.setVisibility(R.id.change, View.VISIBLE);
        off.setVisibility(R.id.change, View.GONE);

        Transition transition = new Fade();
        transition.setDuration(100);
        transition.setInterpolator(new AccelerateDecelerateInterpolator());

        transition.excludeTarget(R.id.profilePictureUndo, true);
        transition.excludeTarget(R.id.usernameUndo, true);
        transition.excludeTarget(R.id.descriptionUndo, true);
        transition.excludeTarget(R.id.uploadCardView, true);


        layout.post(() -> {
            TransitionManager.beginDelayedTransition(layout);
            if (changed) on.applyTo(layout);
            else off.applyTo(layout);
        });
    }

    private void rotateIn(View view) {
        Animation in = AnimationUtils.loadAnimation(getContext(), R.anim.rotate_in);
        view.setVisibility(View.VISIBLE);
        view.startAnimation(in);
    }

    private void rotateOut(View view) {
        Animation out = AnimationUtils.loadAnimation(getContext(), R.anim.rotate_out);
        out.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                view.setVisibility(View.GONE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });
        view.startAnimation(out);
    }
}