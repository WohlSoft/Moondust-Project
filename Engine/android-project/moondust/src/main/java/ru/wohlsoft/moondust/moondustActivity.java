package ru.wohlsoft.moondust;

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
}
