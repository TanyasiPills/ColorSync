package com.example.colorsync.Fragment;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.ContentUris;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.provider.MediaStore;
import android.transition.ChangeBounds;
import android.transition.Transition;
import android.transition.TransitionManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.example.colorsync.APIInstance;
import com.example.colorsync.Adapter.ImageGridAdapter;
import com.example.colorsync.Adapter.ImageSelectionGrid;
import com.example.colorsync.DataType.IdType;
import com.example.colorsync.DataType.ImageData;
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

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ProfileFragment extends Fragment {
    private User user;
    private View view;
    private ImageView profilePicture;
    private TextView username;
    private ImageButton settings;
    private TextView description;
    private RecyclerView imagesContainer;
    private List<ImageData> items;
    private ImageGridAdapter adapter;
    private boolean loaded;
    private boolean own;

    private ConstraintLayout mainConstraint;
    private ImageButton uploadOpenButton;
    private RecyclerView uploadImages;
    private ConstraintLayout uploadConstraint;
    private boolean isUploadOpen;
    private boolean isLoadingUris;
    private Cursor cursor;
    private List<Uri> uris;
    private ImageSelectionGrid uploadAdapter;
    private CheckBox uploadCheckbox;
    private Button uploadButton;
    private ImageButton uploadClose;
    private TextView uploadText;
    public TextView uploadPublicText;
    ConstraintSet closed;
    ConstraintSet closedMain;
    ConstraintSet open;
    ConstraintSet openMain;
    private boolean isPfp;

    private Uri selected;


    public ProfileFragment() {}

    @Override
    public void onCreate(Bundle savedInstanceState) {super.onCreate(savedInstanceState); }

    @SuppressLint("CheckResult")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        loaded = false;

        Bundle arguments = getArguments();
        int id = 0;
        if (arguments != null) {
            id = arguments.getInt("id", 0);
        }
        if (id < 1) {
            id = UserManager.user.getId();
        }
        own = id == UserManager.user.getId();

        MainActivity.getApi().getUserById(id).enqueue(new Callback<User>() {
            @Override
            public void onResponse(@NonNull Call<User> call, @NonNull Response<User> response) {
                if (response.isSuccessful() && response.body() != null) {
                    user = response.body();
                    load();
                } else {
                    new AlertDialog.Builder(MainActivity.getInstance())
                            .setTitle("Failed to load user")
                            .setPositiveButton("Ok", (dialog, which) -> MainActivity.getInstance().goToHome())
                            .show();
                }
            }

            @Override
            public void onFailure(@NonNull Call<User> call, @NonNull Throwable throwable) {
                new AlertDialog.Builder(MainActivity.getInstance())
                        .setTitle("Failed to load user")
                        .setMessage(throwable.getMessage())
                        .setPositiveButton("Ok", (dialog, which) -> MainActivity.getInstance().goToHome())
                        .show();
            }
        });


        view = inflater.inflate(R.layout.fragment_profile, container, false);
        imagesContainer = view.findViewById(R.id.imagesContainer);
        username = view.findViewById(R.id.username);
        profilePicture = view.findViewById(R.id.profilePicture);
        description = view.findViewById(R.id.description);
        uploadOpenButton = view.findViewById(R.id.uploadOpenButton);
        uploadImages = view.findViewById(R.id.uploadImages);
        uploadConstraint = view.findViewById(R.id.uploadConstraint);
        mainConstraint = view.findViewById(R.id.profileConstraint);
        uploadCheckbox = view.findViewById(R.id.uploadCheckbox);
        uploadButton = view.findViewById(R.id.uploadButton);
        uploadClose = view.findViewById(R.id.uploadCancel);
        uploadText = view.findViewById(R.id.uploadText);
        uploadPublicText = view.findViewById(R.id.uploadPublicText);
        settings = view.findViewById(R.id.settings);

        isUploadOpen = false;
        isLoadingUris = false;
        isPfp = false;
        items = new ArrayList<>();
        uris = new ArrayList<>();

        adapter = new ImageGridAdapter(items, 40);
        imagesContainer.setAdapter(adapter);
        imagesContainer.setLayoutManager(new GridLayoutManager(requireContext(), 2));

        uploadAdapter = new ImageSelectionGrid(uris, null, this::uriClickHandler, null, 50);
        uploadImages.setAdapter(uploadAdapter);
        uploadImages.setLayoutManager(new GridLayoutManager(requireContext(), 2));

        settings.setOnClickListener(v -> {
            MainActivity.getInstance().goToSettings();
        });

        uploadOpenButton.setOnClickListener(v -> {
            uploadOpen(false);
        });

        uploadClose.setOnClickListener(v -> {
            uploadOpen(false);
        });

        uploadButton.setOnClickListener(v -> {
            upload();
        });

        profilePicture.setOnClickListener(v -> {
            uploadOpen(true);
        });

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

        if (!own) {
            uploadOpenButton.setVisibility(View.GONE);
            settings.setVisibility(View.GONE);
        }


        closed = new ConstraintSet();
        closed.clone(uploadConstraint);
        open = new ConstraintSet();
        open.clone(uploadConstraint);

        open.setVisibility(R.id.uploadText, View.VISIBLE);
        open.setVisibility(R.id.uploadImages, View.VISIBLE);
        open.setVisibility(R.id.uploadPublicText, View.VISIBLE);
        open.setVisibility(R.id.uploadCheckbox, View.VISIBLE);
        open.setVisibility(R.id.uploadCancel, View.VISIBLE);
        open.setVisibility(R.id.uploadOpenButton, View.GONE);
        open.setVisibility(R.id.uploadButton, View.VISIBLE);

        closed.setVisibility(R.id.uploadText, View.GONE);
        closed.setVisibility(R.id.uploadImages, View.GONE);
        closed.setVisibility(R.id.uploadPublicText, View.GONE);
        closed.setVisibility(R.id.uploadCheckbox, View.GONE);
        closed.setVisibility(R.id.uploadCancel, View.GONE);
        closed.setVisibility(R.id.uploadOpenButton, View.VISIBLE);
        closed.setVisibility(R.id.uploadButton, View.GONE);

        closedMain = new ConstraintSet();
        closedMain.clone(mainConstraint);
        openMain = new ConstraintSet();
        openMain.clone(mainConstraint);

        openMain.connect(R.id.uploadCardView, ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
        openMain.connect(R.id.uploadCardView, ConstraintSet.START, ConstraintSet.PARENT_ID, ConstraintSet.START);

        closedMain.clear(R.id.uploadCardView, ConstraintSet.TOP);
        closedMain.clear(R.id.uploadCardView, ConstraintSet.START);

        load();
        return view;
    }

    private Void uriClickHandler(Uri uri) {
        selected = uri;
        return null;
    }

    private void upload() {
        if (selected == null) {
            Toast.makeText(requireContext(), "No image selected", Toast.LENGTH_SHORT).show();
            return;
        }
        File file;
        try {
            file = FileUtils.getFileFromUri(getContext(), selected);
        } catch (IOException e) {
            new android.app.AlertDialog.Builder(getContext())
                    .setTitle("Failed to open image")
                    .setMessage(e.getMessage())
                    .setPositiveButton("OK", null)
                    .show();
            throw new RuntimeException(e);
        }
        RequestBody fileRequest = RequestBody.create(MediaType.parse("image/*"), file);
        MultipartBody.Part fileBody = MultipartBody.Part.createFormData("file", file.getName(), fileRequest);

        if (isPfp) {
            MainActivity.getApi().uploadPfp(UserManager.getBearer(), fileBody).enqueue(new Callback<Void>() {
                @Override
                public void onResponse(Call<Void> call, Response<Void> response) {
                    if (response.isSuccessful()) {
                        uploadOpen(false);
                        Glide.with(view)
                                .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
                                .signature(GlideUtils.changeSignature())
                                .into(profilePicture);
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
        } else {
            String visibility;
            if (uploadCheckbox.isChecked()) visibility = "public";
            else visibility = "private";
            RequestBody visibilityRequest = RequestBody.create(MediaType.parse("text/plain"), visibility);

            MainActivity.getApi().uploadImage(UserManager.getBearer(), fileBody, visibilityRequest).enqueue(new Callback<IdType>() {
                @Override
                public void onResponse(Call<IdType> call, Response<IdType> response) {
                    if (response.isSuccessful() && response.body() != null) {
                        Toast.makeText(requireContext(), "Image uploaded", Toast.LENGTH_SHORT).show();
                        loadImages();
                        uploadOpen(false);
                    } else new AlertDialog.Builder(requireContext())
                            .setTitle("Failed to upload image")
                            .setMessage(response.message())
                            .setPositiveButton("OK", null)
                            .show();
                }

                @Override
                public void onFailure(Call<IdType> call, Throwable throwable) {
                    new AlertDialog.Builder(requireContext())
                            .setTitle("Failed to upload image")
                            .setMessage(throwable.getMessage())
                            .setPositiveButton("OK", null)
                            .show();
                }
            });
        }
    }

    private void load() {
        if (!loaded) {
            loaded = true;
            return;
        }
        username.setText(user.getUsername());
        Glide.with(view)
                .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
                .signature(GlideUtils.getSignature())
                .into(profilePicture);

        if (user.getProfile_description() != null) description.setText(user.getProfile_description());
        else description.setVisibility(View.GONE);

        loadImages();
    }

    private void loadImages() {
        MainActivity.getApi().getUserImages(UserManager.getBearer(), user.getId()).enqueue(new Callback<List<ImageData>>() {
            @SuppressLint("NotifyDataSetChanged")
            @Override
            public void onResponse(@NonNull Call<List<ImageData>> call, @NonNull Response<List<ImageData>> response) {
                if (response.isSuccessful() && response.body() != null) {
                    items.clear();
                    items.addAll(response.body());
                    adapter.notifyDataSetChanged();
                    if (items.isEmpty()) view.findViewById(R.id.noImagesFound).setVisibility(View.VISIBLE);
                    else view.findViewById(R.id.noImagesFound).setVisibility(View.GONE);
                } else if (response.code() == 404) {
                    view.findViewById(R.id.noImagesFound).setVisibility(View.VISIBLE);
                }
                else {
                    new AlertDialog.Builder(requireContext())
                            .setTitle("Error loading images")
                            .setMessage(response.message())
                            .setPositiveButton("OK", null)
                            .show();
                }
            }

            @Override
            public void onFailure(@NonNull Call<List<ImageData>> call, @NonNull Throwable throwable) {
                new AlertDialog.Builder(requireContext())
                        .setTitle("Error loading images")
                        .setMessage(throwable.getMessage())
                        .setPositiveButton("OK", null)
                        .show();
            }
        });
    }

    private void uploadOpen(boolean pfp) {
        isUploadOpen = !isUploadOpen;
        if (isUploadOpen && uris.isEmpty()) loadMoreUris();

        if (pfp && !isPfp) {
            open.setVisibility(R.id.uploadPublicText, View.GONE);
            open.setVisibility(R.id.uploadCheckbox, View.GONE);
            uploadText.setText(R.string.changePfpText);
            uploadButton.setText(R.string.changeText);
            isPfp = true;
        } else if (isPfp) {
            open.setVisibility(R.id.uploadPublicText, View.VISIBLE);
            open.setVisibility(R.id.uploadCheckbox, View.VISIBLE);
            uploadText.setText(R.string.uploadText);
            uploadButton.setText(R.string.uploadButtonText);
            isPfp = false;
        }

        Transition transition = new ChangeBounds()
                .setDuration(250)
                .setInterpolator(new AccelerateDecelerateInterpolator());

        transition.excludeTarget(R.id.uploadConstraint, true);

        if (isUploadOpen) {
            closed.applyTo(uploadConstraint);
            closedMain.applyTo(mainConstraint);
        } else {
            open.applyTo(uploadConstraint);
            openMain.applyTo(mainConstraint);
        }

        uploadConstraint.post(() -> {
            TransitionManager.beginDelayedTransition(uploadConstraint, transition);
            TransitionManager.beginDelayedTransition(mainConstraint, transition);

            if (isUploadOpen) {
                open.applyTo(uploadConstraint);
                openMain.applyTo(mainConstraint);
            } else {
                closed.applyTo(uploadConstraint);
                closedMain.applyTo(mainConstraint);
                uploadAdapter.deselect();
                uploadImages.scrollToPosition(0);
                uploadCheckbox.setChecked(false);
            }
        });
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

        uploadAdapter.notifyItemRangeInserted(start, i);
        isLoadingUris = false;
    }
}