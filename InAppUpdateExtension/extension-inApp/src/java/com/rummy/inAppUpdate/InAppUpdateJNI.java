package com.rummy.inAppUpdate;

import android.app.Activity;
import android.content.DialogInterface;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.play.core.appupdate.AppUpdateInfo;
import com.google.android.play.core.appupdate.AppUpdateManager;
import com.google.android.play.core.appupdate.AppUpdateManagerFactory;
import com.google.android.play.core.appupdate.AppUpdateOptions;
import com.google.android.play.core.install.InstallStateUpdatedListener;
import com.google.android.play.core.install.model.AppUpdateType;
import com.google.android.play.core.install.model.InstallStatus;
import com.google.android.play.core.install.model.UpdateAvailability;
import com.google.android.play.core.tasks.OnFailureListener;
import com.google.android.play.core.tasks.Task;

import java.lang.ref.WeakReference;

public class InAppUpdateJNI {

    private static final String TAG = "InAppUpdateJNI";
    private WeakReference<Activity> mActivityWeakReference;
    private AppUpdateManager appUpdateManager;

    public InAppUpdateJNI(Activity activity) {
        mActivityWeakReference = new WeakReference<>(activity);
        Log.d(TAG, "InAppUpdateJNI: constructor called");
        checkUpdate();
    }

    private Activity getWeakActivity() {
        return mActivityWeakReference.get();
    }

    public void makeToast(String message) {
        int duration = Toast.LENGTH_SHORT;
        Activity activity = getWeakActivity();
        if (activity != null) {
            activity.runOnUiThread(() -> Toast.makeText(activity, message, duration).show());
        }
    }

    private void checkUpdate() {
        try {
            Activity activity = getWeakActivity();
            if (activity == null) {
                return;
            }
            appUpdateManager = AppUpdateManagerFactory.create(activity.getApplicationContext());

            setInstallListener();

            Task<AppUpdateInfo> appUpdateInfoTask = appUpdateManager.getAppUpdateInfo();
            appUpdateInfoTask.addOnSuccessListener(appUpdateInfo -> {
                if (appUpdateInfo.updateAvailability() == UpdateAvailability.UPDATE_AVAILABLE
                        && appUpdateInfo.isUpdateTypeAllowed(AppUpdateType.IMMEDIATE)) {
                    Log.d(TAG, "Update: available");
                    appUpdateManager.startUpdateFlow(appUpdateInfo,
                            activity, AppUpdateOptions.newBuilder(AppUpdateType.FLEXIBLE).build());
                } else if (appUpdateInfo.updateAvailability() == UpdateAvailability.UPDATE_NOT_AVAILABLE) {
                    Log.d(TAG, "checkUpdate: not available");
                } else if (appUpdateInfo.updateAvailability() == UpdateAvailability.UNKNOWN) {
                    Log.d(TAG, "checkUpdate: unknown");
                }
            }).addOnFailureListener(e -> Log.d(TAG, "checkUpdate: " + e.getLocalizedMessage()));
        } catch (Exception e) {
            e.printStackTrace();
            Log.d(TAG, "checkUpdate: " + e.getLocalizedMessage());
        }
    }

    private void setInstallListener() {
        InstallStateUpdatedListener listener = installState -> {
            Activity activity = getWeakActivity();
            if (activity == null) {
                return;
            }
            switch (installState.installStatus()) {
                case InstallStatus.DOWNLOADED:
                    Log.d(TAG, "An update has been downloaded");
                    popupSnackbarForCompleteUpdate();
                    break;
                case InstallStatus.DOWNLOADING:
                    Log.d(TAG, "installState : downloading");
                    break;
                case InstallStatus.FAILED:
                    Log.d(TAG, "installState : downloading failed");
                    break;
                case InstallStatus.CANCELED:
                    Log.d(TAG, "installState : downloading canceled");
                    break;
                default:
                    Log.d(TAG, "installState : unknown");
                    break;
            }
        };
        appUpdateManager.registerListener(listener);
    }

    private void popupSnackbarForCompleteUpdate() {
        Activity activity = getWeakActivity();
        if (activity != null) {
            activity.runOnUiThread(() -> new MaterialAlertDialogBuilder(activity)
                    .setMessage("An update has just been downloaded. Install update?")
                    .setPositiveButton("Install", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            appUpdateManager.completeUpdate();
                        }
                    })
                    .show());
        }
    }
}
