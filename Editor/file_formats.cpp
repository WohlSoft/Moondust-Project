#include "mainwindow.h"
#include "lvl_filedata.h"


//NPC file Read
bool MainWindow::ReadNpcTXTFile(QFile &inf)
{
    int str_count=0;        //Line Counter
    int i;                  //counters
    int file_format;        //File format number
    QString line;           //Current Line data
    QTextStream in(&inf);   //Read File

    //Read NPC.TXT File config

    /*
    Comming soon
    */

return false;
}

//World file Read
bool MainWindow::ReadWorldFile(QFile &inf)
{
    int str_count=0;        //Line Counter
    int i;                  //counters
    int file_format;        //File format number
    QString line;           //Current Line data
    QTextStream in(&inf);   //Read File

    //Read NPC.TXT File config

    /*
    Comming soon
    */

return false;
}

//Level File Read
bool MainWindow::ReadLevelFile(QFile &inf)
{
    QTextStream in(&inf);   //Read File
    int str_count=0;        //Line Counter
    int i;                  //counters
    int file_format;        //File format number
    QString line;           //Current Line data

                //Mark as first line

    QRegExp isint("\\d+");     //Check "Is Numeric"
    QRegExp boolwords("^(#TRUE#|#FALSE#)$");
    QRegExp issint("^[\\-0]?\\d*$");     //Check "Is signed Numeric"
    QRegExp issfloat("^[\\-0]?[\\d*]?[\\.]?\\d*$");     //Check "Is signed Float Numeric"

    QRegExp qstr("^\"(?:[^\"\\\\]|\\\\.)*\"$");


    ///////////////////////////////////////Begin file///////////////////////////////////////
    str_count++;line = in.readLine();   //Read first Line
    if(!isint.exactMatch(line)) //File format number
        goto badfile;

    else file_format=line.toInt();

    str_count++;line = in.readLine();   //Read second Line
    if(!isint.exactMatch(line)) //File format number
        goto badfile;
    //Else set stars

    str_count++;line = in.readLine();   //Read second Line
    //Add to QString - level title

    ////////////SECTION Data//////////
    for(i=0;i<21;i++)
    {
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //left
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //top
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //bottom
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //right
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //music
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //bgcolor
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //IsLevelWarp
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //OffScreen
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //BackGround id
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //NoTurnBack
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //UnderWater
            goto badfile;
        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //Custom Music
            goto badfile;
    }


    //Player's point config

    str_count++;line = in.readLine();
    if(!issint.exactMatch(line)) //1 Player x
        goto badfile;
    str_count++;line = in.readLine();
    if(!issint.exactMatch(line)) //1 Player y
        goto badfile;
    str_count++;line = in.readLine();
    if(!isint.exactMatch(line)) //1 Player w
        goto badfile;
    str_count++;line = in.readLine();
    if(!isint.exactMatch(line)) //1 Player h
        goto badfile;

    str_count++;line = in.readLine();
    if(!issint.exactMatch(line)) //1 Player x
        goto badfile;
    str_count++;line = in.readLine();
    if(!issint.exactMatch(line)) //1 Player y
        goto badfile;
    str_count++;line = in.readLine();
    if(!isint.exactMatch(line)) //1 Player w
        goto badfile;
    str_count++;line = in.readLine();
    if(!isint.exactMatch(line)) //1 Player h
        goto badfile;

    ////////////Block Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        if(!issint.exactMatch(line)) //Block x
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Block y
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Block h
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Block w
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Block id
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Containing NPC id
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Invisible
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Slippery
            goto badfile;
        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //layer
            goto badfile;
        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //event destroy
            goto badfile;
        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //event hit
            goto badfile;
        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //event no more objects in layer
            goto badfile;
        str_count++;line = in.readLine();
    }


    ////////////BGO Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        if(!issint.exactMatch(line)) //BGO x
            goto badfile;

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //BGO y
            goto badfile;

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //BGO id
            goto badfile;

        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //layer
            goto badfile;

        str_count++;line = in.readLine();
    }


    ////////////NPC Data//////////
     str_count++;line = in.readLine();
     while(line!="\"next\"")
     {
         if(!issint.exactMatch(line)) //NPC x
             goto badfile;

         str_count++;line = in.readLine();
         if(!issint.exactMatch(line)) //NPC y
             goto badfile;

         str_count++;line = in.readLine();
         if(!issint.exactMatch(line)) //NPC direction
             goto badfile;


         str_count++;line = in.readLine();
         if(!isint.exactMatch(line)) //NPC id
             goto badfile;

         switch(line.toInt())
         {
         case 283:/*Buble*/ case 91: /*burried*/ case 284: /*SMW Lakitu*/
         case 96: /*egg*/   /*parakoopa*/
         case 76: case 121: case 122:case 123:case 124: case 161:case 176:case 177:
         /*paragoomba*/ case 243: case 244:
         /*Cheep-Cheep*/ case 28: case 229: case 230: case 232: case 233: case 234: case 236:
         /*WarpSelection*/ case 288: case 289: /*firebar*/ case 260:

             str_count++;line = in.readLine();
             if(!isint.exactMatch(line)) //NPC id
                 goto badfile;

             break;
         default: break;
         }

         str_count++;line = in.readLine();
         if(!boolwords.exactMatch(line)) //Generator enabled
             goto badfile;

         if(line=="#TRUE#")
         {

             str_count++;line = in.readLine();
             if(!isint.exactMatch(line)) //Generator direction
                 goto badfile;
             str_count++;line = in.readLine();
             if(!isint.exactMatch(line)) //Generator type [1] Warp, [2] Projective
                 goto badfile;
             str_count++;line = in.readLine();
             if(!isint.exactMatch(line)) //Generator period ( sec*10 ) [1-600]
                 goto badfile;
         }

         str_count++;line = in.readLine();
         if(!qstr.exactMatch(line)) //Message
             goto badfile;

         str_count++;line = in.readLine();
         if(!boolwords.exactMatch(line)) //Friedly NPC
             goto badfile;
         str_count++;line = in.readLine();
         if(!boolwords.exactMatch(line)) //Don't move NPC
             goto badfile;
         str_count++;line = in.readLine();
         if(!boolwords.exactMatch(line)) //LegacyBoss
             goto badfile;

         str_count++;line = in.readLine();
         if(!qstr.exactMatch(line)) //Layer
             goto badfile;

         str_count++;line = in.readLine();
         if(!qstr.exactMatch(line)) //Activate event
             goto badfile;
         str_count++;line = in.readLine();
         if(!qstr.exactMatch(line)) //Death event
             goto badfile;
         str_count++;line = in.readLine();
         if(!qstr.exactMatch(line)) //Talk event
             goto badfile;
         str_count++;line = in.readLine();
         if(!qstr.exactMatch(line)) //No more object in layer event
             goto badfile;
         str_count++;line = in.readLine();
         if(!qstr.exactMatch(line)) //Layer name to attach
             goto badfile;

         str_count++;line = in.readLine();
    }


    ////////////Warp and Doors Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        if(!issint.exactMatch(line)) //Entrance x
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Entrance y
            goto badfile;

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Exit x
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Exit y
            goto badfile;

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Entrance direction: [3] down, [1] up, [2] left, [4] right
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Exit direction: [1] down [3] up [4] left [2] right
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Door type: [1] pipe, [2] door, [0] instant
            goto badfile;

        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //Warp to level
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Normal entrance or Warp to other door
            goto badfile;

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Level Entrance (can not enter)
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Level Exit (End of level)
            goto badfile;

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //WarpTo X
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //WarpTo y
            goto badfile;

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Need a stars
            goto badfile;

        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //Layer
            goto badfile;

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //<unused>, always FALSE
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //No Yoshi
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Allow NPC
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Locked
            goto badfile;

    str_count++;line = in.readLine();
    }

    ////////////Water/QuickSand Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        if(!issint.exactMatch(line)) //Water x
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Water y
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Water w
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Water h
            goto badfile;

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Unused
            goto badfile;

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Quicksand
            goto badfile;

        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //Layer
            goto badfile;

    str_count++;line = in.readLine();
    }

    ////////////Layers Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        if(!qstr.exactMatch(line)) //Layer name
            goto badfile;

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //hidden layer
            goto badfile;

    str_count++;line = in.readLine();
    }

    ////////////Events Data//////////
    str_count++;line = in.readLine();
    while((line!="")&&(!line.isNull()))
    {
        if(!qstr.exactMatch(line)) //Event name
            goto badfile;

        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //Event message
            goto badfile;

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //PlaySound
            goto badfile;

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //EndGame
            goto badfile;

        for(i=0; i<21; i++)
        {
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Hide layer
                goto badfile;
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Show layer
                goto badfile;
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Toggle layer
                goto badfile;
        }

        for(i=0; i<21; i++)
        {
            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //Set Music
                goto badfile;
            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //Set Background
                goto badfile;
            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //Set Position to: LEFT
                goto badfile;
            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //Set Position to: TOP
                goto badfile;
            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //Set Position to: BOTTOM
                goto badfile;
            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //Set Position to: RIGHT
                goto badfile;
        }
        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //Trigger
            goto badfile;
        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Start trigger event after x [sec*10]. Etc. 153,2 sec
            goto badfile;

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //No Smoke
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Hold ALT-JUMP player control
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //ALT-RUN
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //DOWN
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //DROP
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //JUMP
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //LEFT
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //RIGHT
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //RUN
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //START
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //UP
            goto badfile;
        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Auto start
            goto badfile;

        str_count++;line = in.readLine();
        if(!qstr.exactMatch(line)) //Layer for movement
            goto badfile;
        str_count++;line = in.readLine();
        if(!issfloat.exactMatch(line)) //Layer moving speed – horizontal
            goto badfile;
        str_count++;line = in.readLine();
        if(!issfloat.exactMatch(line)) //Layer moving speed – vertical
            goto badfile;
        str_count++;line = in.readLine();
        if(!issfloat.exactMatch(line)) //Move screen horizontal speed
            goto badfile;
        str_count++;line = in.readLine();
        if(!issfloat.exactMatch(line)) //Move screen vertical speed
            goto badfile;
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Scroll section x, (in file value is x-1)
            goto badfile;

    str_count++;line = in.readLine();
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////

    return false;

    badfile:    //If file format not corrects
    BadFileMsg(this, inf.fileName(), str_count, line);
    return true;
}

//Errror for file parsing
void BadFileMsg(MainWindow *window, QString fileName_DATA, int str_count, QString line)
{
    QMessageBox::warning(window, MainWindow::tr("Bad File"),
    MainWindow::tr(QString("Bad file format\nFile: %1\n"
               "Line Number: %3\n"         //Print Line With error
               "Line Data: %2").arg(fileName_DATA.toStdString().c_str(),line)  //Print Bad data string
               .arg(str_count)
               .toStdString().c_str()),
        QMessageBox::Ok);
}
