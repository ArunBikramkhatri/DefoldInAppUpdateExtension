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

  private static final String TEST_ID = "ca-app-pub-3940256099942544/6300978111";
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

  void checkUpdate() {
    try {
      Log.d(TAG, "checkUpdate: 0");
      AppUpdateManager appUpdateManager = AppUpdateManagerFactory.create(activity.getApplicationContext());
      Log.d(TAG, "checkUpdate: 1");
      // Returns an intent object that you use to check for an update.
      Task<AppUpdateInfo> appUpdateInfoTask = appUpdateManager.getAppUpdateInfo();
      // Checks that the platform will allow the specified type of update.

      Log.d(TAG, "checkUpdate: 2");
      appUpdateInfoTask.addOnSuccessListener(appUpdateInfo -> {
        if (appUpdateInfo.updateAvailability() == UpdateAvailability.UPDATE_AVAILABLE
            // This example applies an immediate update. To apply a flexible update
            // instead, pass in AppUpdateType.FLEXIBLE
            && appUpdateInfo.isUpdateTypeAllowed(AppUpdateType.IMMEDIATE)) {
          // Request the update.
          Log.d(TAG, "checkUpdate: available");
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
      Log.d(TAG, "checkUpdate: 3");
    } catch (Exception e) {
      e.printStackTrace();
      Log.d(TAG, "checkUpdate: " + e.getLocalizedMessage());
      throw new RuntimeException(e);
    }
  }

}
