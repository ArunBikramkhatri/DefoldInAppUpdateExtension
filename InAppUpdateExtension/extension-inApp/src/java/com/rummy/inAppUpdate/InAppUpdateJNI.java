package com.rummy.inAppUpdate;

import android.app.Activity;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;

//import com.google.android.gms.tasks.Task;
import com.google.android.play.core.tasks.Task;
import com.google.android.play.core.appupdate.AppUpdateInfo;
import com.google.android.play.core.appupdate.AppUpdateManager;
import com.google.android.play.core.appupdate.AppUpdateManagerFactory;
import com.google.android.play.core.appupdate.AppUpdateOptions;
import com.google.android.play.core.install.model.AppUpdateType;
import com.google.android.play.core.install.model.UpdateAvailability;

public class InAppUpdateJNI {

  private Activity activity;
  private final String TAG = "InAppUpdateJNI";
  private String defoldUserAgent = "defold-x.y.z";

  public InAppUpdateJNI() {
  }

  public InAppUpdateJNI(Activity activity) {
    this.activity = activity;
    Log.d(TAG, "InAppUpdateJNI: constructor called");
    checkUpdate();
  }

  public void makeToast(String message) {
    int duration = Toast.LENGTH_SHORT;
    Log.d(TAG, "make toast");
    activity.runOnUiThread(new Runnable() {
      @Override
      public void run() {
        Toast.makeText(activity, message, duration).show();
      }
    });

  }

  void checkUpdate() {
    try {
      AppUpdateManager appUpdateManager = AppUpdateManagerFactory.create(activity.getApplicationContext());
      // Returns an intent object that you use to check for an update.
      Task<AppUpdateInfo> appUpdateInfoTask = appUpdateManager.getAppUpdateInfo();
      // Checks that the platform will allow the specified type of update.

      appUpdateInfoTask.addOnSuccessListener(appUpdateInfo -> {
        if (appUpdateInfo.updateAvailability() == UpdateAvailability.UPDATE_AVAILABLE
            // This example applies an immediate update. To apply a flexible update
            // instead, pass in AppUpdateType.FLEXIBLE
            && appUpdateInfo.isUpdateTypeAllowed(AppUpdateType.IMMEDIATE)) {
          // Request the update.
          Log.d(TAG, "Update: available");
          appUpdateManager.startUpdateFlow(appUpdateInfo,
              activity, AppUpdateOptions.newBuilder(AppUpdateType.FLEXIBLE).build());
        }
        if (appUpdateInfo.updateAvailability() == UpdateAvailability.UPDATE_NOT_AVAILABLE) {
          Log.d(TAG, "checkUpdate: not available");
        }

        if (appUpdateInfo.updateAvailability() == UpdateAvailability.UNKNOWN) {
          Log.d(TAG, "checkUpdate: unknown");
        }
      });
    } catch (Exception e) {
      e.printStackTrace();
      Log.d(TAG, "checkUpdate: " + e.getLocalizedMessage());
    }
  }

}
