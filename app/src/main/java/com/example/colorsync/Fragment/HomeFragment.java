package com.example.colorsync.Fragment;

import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.SearchView;
import androidx.cardview.widget.CardView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import android.provider.MediaStore;
import android.transition.ChangeBounds;
import android.transition.Transition;
import android.transition.TransitionManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.bumptech.glide.load.DataSource;
import com.bumptech.glide.load.engine.GlideException;
import com.bumptech.glide.request.RequestListener;
import com.bumptech.glide.request.target.Target;
import com.example.colorsync.APIInstance;
import com.example.colorsync.Adapter.ImageSelectionGrid;
import com.example.colorsync.Adapter.ScrollAdapter;
import com.example.colorsync.Adapter.TagAdapter;
import com.example.colorsync.DataType.ImageData;
import com.example.colorsync.DataType.Post;
import com.example.colorsync.DataType.PostCreate;
import com.example.colorsync.DataType.PostResponse;
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

import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HomeFragment extends Fragment {
    private boolean isLoading;
    private int offset;
    private ScrollAdapter adapter;
    private final List<Post> posts;
    private final List<Integer> likes;
    private RecyclerView recyclerView;
    private Context context;
    private View view;
    private SwipeRefreshLayout swipeLayout;

    private ImageSelectionGrid post_adapter;
    private final List<ImageData> post_userImages;
    private final List<Uri> post_uris;
    private RecyclerView post_images;
    private ImageButton post_send;
    private ImageButton post_upload;
    private ImageButton post_add;
    private EditText post_description;
    private ImageView post_preview;
    private ConstraintLayout post_previewContainer;
    private ImageButton post_previewCancel;
    private RecyclerView post_tags;
    private TagAdapter post_tagsAdapter;
    private final List<String> post_tagsList;
    private EditText post_tagInput;
    private Uri selectedImage;
    private Integer selectedImageId;
    private Cursor cursor;
    private boolean isLoadingUris;
    private boolean upload;
    private SearchView search;
    private String queryText;


    public HomeFragment() {
        isLoading = false;
        isLoadingUris = false;
        offset = 0;
        posts = new ArrayList<>();
        likes = new ArrayList<>();
        post_uris = new ArrayList<>();
        post_userImages = new ArrayList<>();
        post_tagsList = new ArrayList<>();
        selectedImage = null;
        selectedImageId = null;
        upload = true;
        queryText = "";
    }


    @Override
    public void onStart() {
        super.onStart();
        if (MainActivity.getInstance().getOpenAddPost()) addPost();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityResultLauncher<String[]> requestPermissions = registerForActivityResult(
                new ActivityResultContracts.RequestMultiplePermissions(),
                o -> {}
        );

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            requestPermissions.launch(new String[]{
                    android.Manifest.permission.READ_MEDIA_IMAGES,
                    android.Manifest.permission.READ_MEDIA_VIDEO,
                    android.Manifest.permission.READ_MEDIA_VISUAL_USER_SELECTED
            });
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            requestPermissions.launch(new String[]{
                    android.Manifest.permission.READ_MEDIA_IMAGES,
                    android.Manifest.permission.READ_MEDIA_VIDEO
            });
        } else {
            requestPermissions.launch(new String[]{
                    android.Manifest.permission.READ_EXTERNAL_STORAGE
            });
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (cursor != null && !cursor.isClosed()) cursor.close();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_home, container, false);
        context = view.getContext();

        swipeLayout = view.findViewById(R.id.swipeLayout);
        swipeLayout.setOnRefreshListener(this::reloadPosts);
        search = view.findViewById(R.id.search);

        recyclerView = view.findViewById(R.id.recycleView);
        recyclerView.setLayoutManager(new LinearLayoutManager(context));
        adapter = new ScrollAdapter(posts, likes);
        recyclerView.setAdapter(adapter);

        post_images = view.findViewById(R.id.post_images);
        post_images.setVisibility(View.GONE);
        post_adapter = new ImageSelectionGrid(post_uris, post_userImages, this::uriClickHandler, this::dataClickHandler, 50);
        post_images.setAdapter(post_adapter);
        post_images.setLayoutManager(new GridLayoutManager(requireContext(), 2));

        post_tags = view.findViewById(R.id.post_tags);
        post_tagsAdapter = new TagAdapter((ArrayList<String>)post_tagsList, true);
        post_tags.setAdapter(post_tagsAdapter);
        post_tags.setLayoutManager(new LinearLayoutManager(requireContext(), LinearLayoutManager.HORIZONTAL, false));

        recyclerView.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrolled(@NonNull RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);

                if (dy > 0) {
                    LinearLayoutManager layoutManager = (LinearLayoutManager) recyclerView.getLayoutManager();
                    assert layoutManager != null;
                    int visibleItemCount = layoutManager.getChildCount();
                    int totalItemCount = layoutManager.getItemCount();
                    int pastVisibleItems = layoutManager.findFirstVisibleItemPosition();

                    int loadMoreThreshold = 5;
                    if ((visibleItemCount + pastVisibleItems) >= totalItemCount - loadMoreThreshold) {
                        loadMorePosts();
                    }
                }
            }
        });

        post_images.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrolled(@NonNull RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);

                if (upload) {
                    GridLayoutManager layoutManager = (GridLayoutManager) recyclerView.getLayoutManager();

                    int visibleItemCount = Objects.requireNonNull(layoutManager).getChildCount();
                    int totalItemCount = layoutManager.getItemCount();
                    int pastVisibleItems = layoutManager.findFirstVisibleItemPosition();

                    if ((visibleItemCount + pastVisibleItems) >= totalItemCount - 10) {
                        loadMoreUris();
                    }
                }
            }
        });

        EditText searchEditText = search.findViewById(androidx.appcompat.R.id.search_src_text);
        searchEditText.setTextColor(getResources().getColor(R.color.text));
        searchEditText.setBackgroundResource(R.drawable.rounded_background);

        search.setOnQueryTextListener(new SearchView.OnQueryTextListener() {
            @Override
            public boolean onQueryTextSubmit(String query) {
                if (!queryText.equals(query)) {
                    queryText = query;
                    reloadPosts();
                }
                return false;
            }

            @Override
            public boolean onQueryTextChange(String newText) {
                return false;
            }
        });

        search.setOnQueryTextFocusChangeListener((view, hasFocus) -> {
            if (!hasFocus && !queryText.isEmpty() && search.getQuery().toString().isEmpty()) {
                queryText = "";
                reloadPosts();
            }
        });

        if (view.findViewById(R.id.addPostLayout).getVisibility() == View.VISIBLE) addPost();

        post_send = view.findViewById(R.id.post_send);
        post_description = view.findViewById(R.id.post_description);
        post_upload = view.findViewById(R.id.post_upload);
        post_add = view.findViewById(R.id.post_add);
        post_preview = view.findViewById(R.id.post_preview);
        post_previewContainer = view.findViewById(R.id.post_previewContainer);
        post_previewCancel = view.findViewById(R.id.post_previewCancel);
        post_tagInput = view.findViewById(R.id.post_tagInput);

        post_tagInput.setOnEditorActionListener((textView, i, keyEvent) -> {
            String tag = post_tagInput.getText().toString();
            if (tag.isEmpty() || post_tagsList.contains(tag)) {
                Animation shake = AnimationUtils.loadAnimation(context, R.anim.shake);
                post_tagInput.startAnimation(shake);
                return true;
            }
            post_tagsAdapter.insertItem(tag, 0);
            post_tagInput.setText("");
            post_tags.scrollToPosition(0);
            return true;
        });

        post_upload.setOnClickListener(v -> {
            post_previewContainer.setVisibility(View.GONE);
            if (upload && post_images.getVisibility() == View.VISIBLE) {
                post_images.setVisibility(View.GONE);
                if (selectedImage != null) post_previewContainer.setVisibility(View.VISIBLE);
            }
            else  {
                if (!upload) {
                    upload = true;
                    selectedImageId = null;
                    post_adapter.changeToUri();
                }
                post_images.setVisibility(View.VISIBLE);
                if (post_uris.isEmpty()) loadMoreUris();
            }
        });

        post_add.setOnClickListener(v -> {
            post_previewContainer.setVisibility(View.GONE);
            if (!upload && post_images.getVisibility() == View.VISIBLE) {
                post_images.setVisibility(View.GONE);
                if (selectedImageId != null) post_previewContainer.setVisibility(View.VISIBLE);
            } else {
                if (upload) {
                    upload = false;
                    selectedImage = null;
                    post_adapter.changeToData();
                }
                post_images.setVisibility(View.VISIBLE);
                if (post_userImages.isEmpty()) {
                    MainActivity.getApi().getUserImages(UserManager.getBearer()).enqueue(new Callback<List<ImageData>>() {
                        @SuppressLint("NotifyDataSetChanged")
                        @Override
                        public void onResponse(@NonNull Call<List<ImageData>> call, @NonNull Response<List<ImageData>> response) {
                            if (response.isSuccessful() && response.body() != null) {
                                post_userImages.clear();
                                post_userImages.addAll(response.body());
                                post_adapter.notifyDataSetChanged();
                            } else if (response.code() != 404) {
                                new AlertDialog.Builder(context)
                                        .setTitle("Failed to load images")
                                        .setMessage(response.message())
                                        .show();
                            }
                        }

                        @Override
                        public void onFailure(@NonNull Call<List<ImageData>> call, @NonNull Throwable throwable) {
                            new AlertDialog.Builder(context)
                                    .setTitle("Failed to load images")
                                    .setMessage(throwable.getMessage())
                                    .show();
                        }
                    });
                }
            }
        });

        post_send.setOnClickListener(v -> {
            String text = post_description.getText().toString();
            if (text.isEmpty()) {
                Animation shake = AnimationUtils.loadAnimation(context, R.anim.shake);
                post_description.startAnimation(shake);
                return;
            }
            File file = null;
            if (upload && selectedImage != null && selectedImage.getPath() != null) {
                try {
                    file = FileUtils.getFileFromUri(context, selectedImage);
                } catch (IOException e) {
                    new android.app.AlertDialog.Builder(context)
                            .setTitle("Failed to upload image")
                            .setPositiveButton("OK", null)
                            .show();
                    throw new RuntimeException(e);
                }
            }
            if (file != null) {
                RequestBody fileRequest = RequestBody.create(MediaType.parse("image/*"), file);
                MultipartBody.Part fileBody = MultipartBody.Part.createFormData("file", file.getName(), fileRequest);
                RequestBody textRequest = RequestBody.create(MediaType.parse("text/plain"), text);
                List<RequestBody> tagsBody = new ArrayList<>();

                for (String tag : post_tagsList) {
                    tagsBody.add(RequestBody.create(MediaType.parse("text/plain"), tag));
                }
                MainActivity.getApi().createPostWithFile(UserManager.getBearer(), fileBody, textRequest, tagsBody).enqueue(new Callback<Post>() {
                    @Override
                    public void onResponse(@NonNull Call<Post> call, @NonNull Response<Post> response) {
                        if (response.isSuccessful() && response.body() != null) {
                            postCreated(response.body());
                        }
                        else new AlertDialog.Builder(context)
                                .setTitle("Error creating post")
                                .setMessage(response.message())
                                .setPositiveButton("Ok", null)
                                .show();
                    }

                    @Override
                    public void onFailure(@NonNull Call<Post> call, @NonNull Throwable throwable) {
                        new AlertDialog.Builder(context)
                                .setTitle("Error creating post")
                                .setMessage(throwable.getMessage())
                                .setPositiveButton("Ok", null)
                                .show();
                    }
                });
            } else {
                MainActivity.getApi().createPost(UserManager.getBearer(), new PostCreate(text, selectedImageId, post_tagsList)).enqueue(new Callback<Post>() {
                    @Override
                    public void onResponse(@NonNull Call<Post> call, @NonNull Response<Post> response) {
                        if (response.isSuccessful() && response.body() != null) {
                            postCreated(response.body());
                        } else if (response.code() == 409) {
                            new AlertDialog.Builder(context)
                                    .setTitle("Warning")
                                    .setMessage("You are trying to post a private image, do you want to make it public?")
                                    .setNegativeButton("No", null)
                                    .setPositiveButton("Yes", (dialogInterface, i) -> {
                                        PostCreate post = new PostCreate(text, selectedImageId, post_tagsList).forcePost();
                                        Toast.makeText(context, "post: " + post.forcePost, Toast.LENGTH_SHORT).show();
                                        MainActivity.getApi().createPost(UserManager.getBearer(), post).enqueue(new Callback<Post>() {
                                            @Override
                                            public void onResponse(@NonNull Call<Post> call, @NonNull Response<Post> response) {
                                                if (response.isSuccessful() && response.body() != null) {
                                                    postCreated(response.body());
                                                } else new AlertDialog.Builder(context)
                                                        .setTitle("Error creating post")
                                                        .setMessage(response.message())
                                                        .setPositiveButton("Ok", null)
                                                        .show();
                                            }

                                            @Override
                                            public void onFailure(@NonNull Call<Post> call, @NonNull Throwable throwable) {
                                                new AlertDialog.Builder(context)
                                                        .setTitle("Error creating post")
                                                        .setMessage(throwable.getMessage())
                                                        .setPositiveButton("Ok", null)
                                                        .show();
                                            }
                                        });
                                    })
                                    .show();
                        } else new AlertDialog.Builder(context)
                                .setTitle("Error creating post")
                                .setMessage(response.message())
                                .setPositiveButton("Ok", null)
                                .show();
                    }

                    @Override
                    public void onFailure(@NonNull Call<Post> call, @NonNull Throwable throwable) {
                        new AlertDialog.Builder(context)
                                .setTitle("Error creating post")
                                .setMessage(throwable.getMessage())
                                .setPositiveButton("Ok", null)
                                .show();
                    }
                });
            }
        });

        post_previewCancel.setOnClickListener(v -> {
            post_previewContainer.setVisibility(View.GONE);
            selectedImage = null;
            selectedImageId = null;
            post_adapter.deselect();
        });

        loadMorePosts();
        getLikes();

        return view;
    }

    private void getLikes() {
        MainActivity.getApi().getLikes(UserManager.getBearer()).enqueue(new Callback<List<Integer>>() {
            @Override
            public void onResponse(@NonNull Call<List<Integer>> call, @NonNull Response<List<Integer>> response) {
                if (response.isSuccessful() && response.body() != null) {
                    likes.addAll(response.body());
                    if (!posts.isEmpty()) likePosts(posts);
                }
            }

            @Override
            public void onFailure(@NonNull Call<List<Integer>> call, @NonNull Throwable throwable) {

            }
        });
    }

    private void likePosts(List<Post> posts) {
        if (likes.isEmpty()) return;
        for (Post post : posts) {
            if (likes.contains(post.getId())) post.setLiked(true);
        }
    }

    private void postCreated(Post post) {
        MainActivity.getInstance().hideKeyboard();
        selectedImage = null;
        post_description.setText("");
        post_previewContainer.setVisibility(View.GONE);
        posts.add(0, post);
        post_tagsAdapter.clear();
        adapter.notifyItemInserted(0);
        offset++;
        addPost();

        LinearLayoutManager layoutManager = (LinearLayoutManager) recyclerView.getLayoutManager();
        if (layoutManager != null) {
            int firstVisibleItemPosition = layoutManager.findFirstVisibleItemPosition();
            if (firstVisibleItemPosition < 2) recyclerView.scrollToPosition(0);
        }
    }
    public Void uriClickHandler(Uri uri) {
        post_previewContainer.setVisibility(View.VISIBLE);
        selectedImage = uri;
        selectedImageId = null;
        Glide.with(context)
                .load(uri)
                .into(post_preview);
        post_images.setVisibility(View.GONE);
        return null;
    }

    public Void dataClickHandler(ImageData data) {
        selectedImageId = data.id;
        selectedImage = null;
        Glide.with(context)
                .load(APIInstance.BASE_URL + "images/" + data.id)
                .listener(new RequestListener<Drawable>() {
                    @Override
                    public boolean onLoadFailed(@Nullable GlideException e, Object model, Target<Drawable> target, boolean isFirstResource) {
                        new android.app.AlertDialog.Builder(context)
                                .setTitle("Failed to load image")
                                .setPositiveButton("OK", null)
                                .show();
                        return false;
                    }

                    @Override
                    public boolean onResourceReady(Drawable resource, Object model, Target<Drawable> target, DataSource dataSource, boolean isFirstResource) {
                        post_previewContainer.setVisibility(View.VISIBLE);
                        post_images.setVisibility(View.GONE);
                        return false;
                    }
                })
                .into(post_preview);
        return null;
    }

    public void loadMoreUris() {
        post_images.setVisibility(View.VISIBLE);
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

        int start = post_uris.size();
        int i = 0;
        for (; i < 10; i++) {
            if (!cursor.moveToNext()) break;
            int colIndex = cursor.getColumnIndex(MediaStore.Images.Media._ID);
            if (colIndex < 0) continue;
            long id = cursor.getLong(colIndex);
            Uri uri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id);
            post_uris.add(uri);
        }

        post_adapter.notifyItemRangeInserted(start, i);
        isLoadingUris = false;
    }

    public void addPost() {
        ConstraintLayout layout = view.findViewById(R.id.addPostLayout);
        CardView card = view.findViewById(R.id.addPost_card);

        Drawable background = layout.getBackground();

        ConstraintSet setOff = new ConstraintSet();
        setOff.clone(layout);
        setOff.clear(card.getId(), ConstraintSet.BOTTOM);
        setOff.clear(card.getId(), ConstraintSet.TOP);
        setOff.connect(card.getId(), ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.BOTTOM);

        ConstraintSet setOn = new ConstraintSet();
        setOn.clone(layout);
        setOn.connect(card.getId(), ConstraintSet.TOP, ConstraintSet.PARENT_ID, ConstraintSet.TOP);
        setOn.connect(card.getId(), ConstraintSet.BOTTOM, ConstraintSet.PARENT_ID, ConstraintSet.BOTTOM);


        if (layout.getVisibility() == View.GONE) {
            layout.setVisibility(View.VISIBLE);

            Transition transition = new ChangeBounds()
                    .setDuration(250)
                    .setInterpolator(new AccelerateDecelerateInterpolator());

            setOff.applyTo(layout);

            layout.post(() -> {
                TransitionManager.beginDelayedTransition(layout, transition);
                setOn.applyTo(layout);
                ValueAnimator alphaAnimator = ValueAnimator.ofInt(0, 256);
                alphaAnimator.setDuration(500);
                alphaAnimator.addUpdateListener(animation -> {
                    int alpha = (int) animation.getAnimatedValue();
                    background.setAlpha(alpha);
                });
                alphaAnimator.start();
            });
        } else {
            Transition transition = new ChangeBounds()
                    .setDuration(250)
                    .setInterpolator(new AccelerateDecelerateInterpolator())
                    .addListener(new Transition.TransitionListener() {
                        @Override
                        public void onTransitionStart(Transition transition) {

                        }

                        @Override
                        public void onTransitionEnd(Transition transition) {
                            layout.setVisibility(View.GONE);
                            if (cursor != null && !cursor.isClosed()) cursor.close();
                            post_uris.clear();
                            post_userImages.clear();
                            selectedImage = null;
                            selectedImageId = null;
                            post_description.setText("");
                            post_previewContainer.setVisibility(View.GONE);
                            post_images.setVisibility(View.GONE);
                        }

                        @Override
                        public void onTransitionCancel(Transition transition) {

                        }

                        @Override
                        public void onTransitionPause(Transition transition) {

                        }

                        @Override
                        public void onTransitionResume(Transition transition) {

                        }
                    });

            setOn.applyTo(layout);
            layout.post(() -> {
                TransitionManager.beginDelayedTransition(layout, transition);
                setOff.applyTo(layout);
                ValueAnimator alphaAnimator = ValueAnimator.ofInt(Math.min(256, background.getAlpha()), 0);
                alphaAnimator.setDuration(500);
                alphaAnimator.addUpdateListener(animation -> {
                    int alpha = (int) animation.getAnimatedValue();
                    background.setAlpha(alpha);
                });
                alphaAnimator.start();
            });
        }
    }

    private void reloadPosts() {
        int size = posts.size();
        posts.clear();
        adapter.notifyItemRangeRemoved(0, size);
        likes.clear();
        offset = 0;
        GlideUtils.changeSignature();
        loadMorePosts();
        getLikes();
    }

    private void loadMorePosts() {
        if (isLoading) return;
        isLoading = true;
        
        String text = "";
        List<String> tags = new ArrayList<>();

        if (!queryText.isEmpty()) {
            Pattern pattern = Pattern.compile("#\\w+");
            Matcher matcher = pattern.matcher(queryText);

            while (matcher.find()) {
                tags.add(matcher.group().substring(1));
            }

            text = queryText.replaceAll("#\\w+\\s?", "");
            text = text.replaceAll("\\s+", " ").trim();
        }

        MainActivity.getApi().searchPosts(UserManager.getBearer(), text, tags, offset, false).enqueue(new Callback<PostResponse>() {
            @Override
            public void onResponse(@NonNull Call<PostResponse> call, @NonNull Response<PostResponse> response) {
                if (response.isSuccessful() && response.body() != null) {
                    PostResponse data = response.body();
                    if (data.offset == null) {
                        swipeLayout.setRefreshing(false);
                        isLoading = false;
                        return;
                    }
                    int start = posts.size();
                    posts.addAll(data.data);
                    offset = data.offset;
                    likePosts(data.data);
                    adapter.notifyItemRangeInserted(start, data.data.size());
                    isLoading = false;
                    swipeLayout.setRefreshing(false);
                } else {
                    new AlertDialog.Builder(context)
                            .setTitle("Error")
                            .setMessage("Failed to load posts")
                            .show();
                }
            }

            @Override
            public void onFailure(@NonNull Call<PostResponse> call, @NonNull Throwable throwable) {
                new AlertDialog.Builder(context)
                        .setTitle("Error")
                        .setMessage(throwable.getMessage())
                        .show();
                throwable.getStackTrace();
            }
        });
    }
}