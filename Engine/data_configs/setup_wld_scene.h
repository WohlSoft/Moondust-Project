#ifndef SETUP_WLD_SCENE_H
#define SETUP_WLD_SCENE_H

#include <QString>
class QSettings;

struct WorldAdditionalImage
{
    QString imgFile;
    bool animated;
    int frames;
    int framedelay;
    int x;
    int y;
};

struct WorldMapSetup
{
    void init(QSettings &engine_ini);
    QString backgroundImg;
    int viewport_x; //World map view port
    int viewport_y;
    int viewport_w;
    int viewport_h;
    enum titleAlign{
        align_left=0,
        align_center,
        align_right
    };

    int title_x; //Title of level
    int title_y;
    int title_w; //max width of title
    titleAlign title_align;

    bool points_en;
    int points_x;
    int points_y;

    bool health_en;
    int health_x;
    int health_y;

    bool star_en;
    int star_x;
    int star_y;

    bool coin_en;
    int coin_x;
    int coin_y;

    bool portrait_en;
    int portrait_x;
    int portrait_y;
    int portrait_frame_delay;
    QString portrait_animation;
    int portrait_direction;
    QList<WorldAdditionalImage > AdditionalImages;
};

#endif // SETUP_WLD_SCENE_H

