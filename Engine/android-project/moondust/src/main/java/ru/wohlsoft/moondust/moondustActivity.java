package ru.wohlsoft.moondust;

import android.os.Bundle;
import android.widget.LinearLayout;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import org.libsdl.app.SDLActivity;

public class moondustActivity extends SDLActivity
{
    protected String[] getLibraries()
    {
        return new String[] {
            "SDL2",
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
        LinearLayout ll = new LinearLayout(this);
        ll.setBackground(getResources().getDrawable(R.mipmap.buttons));
        addContentView(ll, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.MATCH_PARENT));
    }
}
