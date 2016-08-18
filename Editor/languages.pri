android:{
    LANGUAGES_TARGET=/assets/languages
} else {
    LANGUAGES_TARGET=$$DESTDIR/languages
}

mkpath($$LANGUAGES_TARGET)
unix:  system(find $$shell_path($$PWD/languages) -name *.ts | xargs $$shell_path($$LRELEASE_EXECUTABLE))
win32: system(for /r $$shell_path($$PWD/languages) %B in (*.ts) do $$shell_path($$LRELEASE_EXECUTABLE) %B)
system($$QMAKE_COPY $$shell_path($$PWD/languages/*.qm)  \"$$shell_path($$LANGUAGES_TARGET)\")
system($$QMAKE_COPY $$shell_path($$PWD/languages/*.png) \"$$shell_path($$LANGUAGES_TARGET)\")

TRANSLATIONS += \
    $$PWD/languages/editor_en.ts \
    $$PWD/languages/editor_ru.ts \
    $$PWD/languages/editor_uk.ts \
    $$PWD/languages/editor_de.ts \
    $$PWD/languages/editor_pl.ts \
    $$PWD/languages/editor_es.ts \
    $$PWD/languages/editor_nl.ts \
    $$PWD/languages/editor_it.ts \
    $$PWD/languages/editor_fr.ts \
    $$PWD/languages/editor_pt.ts \
    $$PWD/languages/editor_ja.ts \
    $$PWD/languages/editor_zh.ts \
    $$PWD/languages/editor_bg.ts \
    $$PWD/languages/editor_id.ts \
    $$PWD/languages/editor_sv.ts

