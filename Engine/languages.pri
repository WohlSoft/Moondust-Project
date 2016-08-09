
android:{
    LANGUAGES_TARGET=/assets/languages
} else {
    LANGUAGES_TARGET=$$DESTDIR/languages
}

mkpath($$LANGUAGES_TARGET)
unix:  system(find $$shell_path($$PWD/languages) -name *.ts | xargs $$shell_path($$LRELEASE_EXECUTABLE) -idbased)
win32: system(for /r $$shell_path($$PWD/languages) %B in (*.ts) do $$shell_path($$LRELEASE_EXECUTABLE) -idbased %B)
system($$QMAKE_COPY $$shell_path($$PWD/languages/*.qm)  \"$$shell_path($$LANGUAGES_TARGET)\")

TRANSLATIONS += \
    $$PWD/languages/engine_en.ts \
    $$PWD/languages/engine_ru.ts \
    $$PWD/languages/engine_de.ts \
    $$PWD/languages/engine_es.ts \
    $$PWD/languages/engine_it.ts \
    $$PWD/languages/engine_pl.ts \
    $$PWD/languages/engine_pt.ts
