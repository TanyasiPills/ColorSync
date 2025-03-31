plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "com.example.colorsync"
    compileSdk = 35

    defaultConfig {
        applicationId = "com.example.colorsync"
        minSdk = 26
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
}

dependencies {
    implementation(libs.rxandroid)
    implementation(libs.security.crypto)
    implementation(libs.datastore.preferences)
    implementation(libs.datastore.preferences.rxjava3)
    implementation(libs.appcompat)
    implementation(libs.material)
    implementation(libs.activity)
    implementation(libs.swiperefreshlayout)
    implementation(libs.constraintlayout)
    implementation(libs.glide)
    implementation(libs.retrofit)
    implementation(libs.converter.gson)
    implementation(libs.navigation.fragment)
    implementation(libs.navigation.ui)
    annotationProcessor(libs.compiler)
    testImplementation(libs.junit)
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)
}