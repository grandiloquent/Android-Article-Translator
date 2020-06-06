package euphoria.psycho.application;

import android.app.Application;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.IOException;

public class App extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        Thread.setDefaultUncaughtExceptionHandler(
                (thread, e) -> {
                    try {
                        Share.writeAllText(
                                new File(Environment.getExternalStorageDirectory(),
                                        "application.log"),
                                e.getMessage()
                        );

                        System.exit(1);
                    } catch (IOException ex) {
                        ex.printStackTrace();
                    }
                });
    }
}
