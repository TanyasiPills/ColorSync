package com.example.colorsync;

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
import android.transition.ChangeBounds;
import android.transition.Transition;
import android.transition.TransitionManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.example.colorsync.DataTypes.ImageData;
import com.example.colorsync.DataTypes.User;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ProfileFragment extends Fragment {
    private User user;
    private View view;
    private ImageView profilePicture;
    private TextView username;
    private TextView description;
    private RecyclerView imagesContainer;
    private List<ImageData> items;
    private ImageGridAdapter adapter;
    private boolean loaded = false;

    private ConstraintLayout mainConstraint;
    private ImageButton uploadButton;
    private RecyclerView uploadImages;
    private ConstraintLayout uploadConstraint;
    private boolean isUploadOpen;
    private boolean isLoadingUris;
    private Cursor cursor;
    private List<Uri> uris;
    private ImageSelectionGrid uploadAdapter;

    private Uri selected;


    public ProfileFragment() {
        this(0);
    }

    public ProfileFragment(int id) {
        if (id < 1) {
            id = UserManager.user.getId();
        }
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
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {super.onCreate(savedInstanceState); }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_profile, container, false);
        imagesContainer = view.findViewById(R.id.imagesContainer);
        username = view.findViewById(R.id.username);
        profilePicture = view.findViewById(R.id.profilePicture);
        description = view.findViewById(R.id.description);
        uploadButton = view.findViewById(R.id.uploadButton);
        uploadImages = view.findViewById(R.id.uploadImages);
        uploadConstraint = view.findViewById(R.id.uploadConstraint);
        mainConstraint = view.findViewById(R.id.profileConstraint);

        isUploadOpen = false;
        isLoadingUris = false;
        items = new ArrayList<>();
        uris = new ArrayList<>();

        adapter = new ImageGridAdapter(items);
        imagesContainer.setAdapter(adapter);
        imagesContainer.setLayoutManager(new GridLayoutManager(requireContext(), 2));

        uploadAdapter = new ImageSelectionGrid(uris, null, this::uriClickHandler, null);
        uploadImages.setAdapter(uploadAdapter);
        uploadImages.setLayoutManager(new GridLayoutManager(requireContext(), 2));

        uploadButton.setOnClickListener(v -> {
            if (isUploadOpen) upload();
            else uploadOpen();
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

        if (loaded) load();
        else loaded = true;

        return view;
    }

    private Void uriClickHandler(Uri uri) {
        selected = uri;
        return null;
    }

    private void upload() {

    }

    private void load() {
        if (!loaded) {
            loaded = true;
            return;
        }
        username.setText(user.getUsername());
        Glide.with(view)
                .load(APIInstance.BASE_URL + "users/" + user.getId() + "/pfp")
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
                    if (items.isEmpty()) view.findViewById(R.id.imagesCardView).setVisibility(View.GONE);
                } else if (response.code() == 404) {
                    //TODO: no profile images
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

    private void uploadOpen() {
        isUploadOpen = !isUploadOpen;
        if (isUploadOpen && uris.isEmpty()) loadMoreUris();
        ConstraintSet closed = new ConstraintSet();
        closed.clone(uploadConstraint);
        ConstraintSet open = new ConstraintSet();
        open.clone(uploadConstraint);

        open.setVisibility(R.id.uploadText, View.VISIBLE);
        open.setVisibility(R.id.uploadImages, View.VISIBLE);

        closed.setVisibility(R.id.uploadText, View.GONE);
        closed.setVisibility(R.id.uploadImages, View.GONE);

        ConstraintSet closedMain = new ConstraintSet();
        closedMain.clone(mainConstraint);
        ConstraintSet openMain = new ConstraintSet();
        openMain.clone(mainConstraint);

        openMain.connect(R.id.uploadCardView, ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
        openMain.connect(R.id.uploadCardView, ConstraintSet.START, ConstraintSet.PARENT_ID, ConstraintSet.START);

        closedMain.clear(R.id.uploadCardView, ConstraintSet.TOP);
        closedMain.clear(R.id.uploadCardView, ConstraintSet.START);


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
            }
        });
    }

    public void loadMoreUris() {
        if (isLoadingUris) return;
        isLoadingUris = true;

        Toast.makeText(requireContext(), "Loading uris...", Toast.LENGTH_SHORT).show();

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