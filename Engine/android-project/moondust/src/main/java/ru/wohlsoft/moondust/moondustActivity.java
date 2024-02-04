package ru.wohlsoft.moondust;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.widget.LinearLayout;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import org.libsdl.app.SDLActivity;

enum ControllerKeys
{
    key_BEGIN(0),
    key_start(0),
    key_left(1),
    key_right(2),
    key_up(3),
    key_down(4),
    key_run(5),
    key_jump(6),
    key_altrun(7),
    key_altjump(8),
    key_drop(9),
    key_END(10);

    private final int value;

    ControllerKeys(final int newValue)
    {
        value = newValue;
    }

    public int getValue()
    {
        return value;
    }
}

public class moondustActivity extends SDLActivity
{
    final String LOG_TAG = "Moondust";
    public static final int READWRITE_PERMISSION_FOR_GAME = 1;
    private final Bundle m_savedInstanceState = null;

    protected String[] getLibraries()
    {
        return new String[] {
//          "SDL2",
            "pge_engine"
        };
    }

    private String detectLanguage()
    {
        String lang = Locale.getDefault().toString();
        String[] langD = lang.split("_");
        if(langD.length >= 1)
            return langD[0];
        return "";
    }

    protected String[] getArguments()
    {
        List<String> args = new ArrayList<>();

        // Detect current language of the system
        String lang = detectLanguage();
        if(lang.length() >= 1)
            args.add("--lang=" + lang);

        String[] argsOut = new String[args.size()];
        args.toArray(argsOut);

        return argsOut;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setSdCardPath(Environment.getExternalStorageDirectory().getAbsolutePath());

        if(!hasManageAppFS())
            return;

        LinearLayout ll = new LinearLayout(this);
        ll.setBackground(getResources().getDrawable(R.mipmap.buttons));
        addContentView(ll, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.MATCH_PARENT));
    }

    public boolean hasManageAppFS()
    {
        if(Build.VERSION.SDK_INT >= 30)
        {
            if(Environment.isExternalStorageManager())
                return true;

            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            String pName = getPackageName();
            Uri uri = Uri.fromParts("package", pName, null);
            intent.setData(uri);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);

            return false;
        }

        return true;
    }

    public static native void setSdCardPath(String path);

    public static native void setKeyPos(int cmd, float left, float top, float right, float bottom);
    public static native void setCanvasSize(float width, float height);

}
