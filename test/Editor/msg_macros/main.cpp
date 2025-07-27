#include <QCoreApplication>
#include <QtDebug>
#include <QVector>

#include "msg_preprocessor.h"

void testTokensList(const std::string &line)
{
    std::vector<std::string> tokens;
    QStringList qTokens;

    if(!msgMacroParseTokens(line, tokens))
    {
        qDebug() << "Line" << QString::fromStdString(line) << "is invalid";
    }

    for(auto &s : tokens)
        qTokens.append(QString::fromStdString(s));

    qDebug() << "Parsed tokens:" << qTokens;
}

static bool isUNum(const std::string &n)
{
    for(auto c : n)
    {
        if(!std::isdigit(c))
            return false;
    }

    return true;
}

void testRealCondition(const std::string &line)
{
    std::vector<std::string> tokens;

    if(!msgMacroParseTokens(line, tokens))
    {
        qDebug() << "Line" << QString::fromStdString(line) << "is invalid";
        return;
    }

    std::string cond = tokens[0];

    for(auto &c : cond)
        c = std::tolower(c);

    if(cond == "if" || cond == "elif")
    {
        if(tokens.size() < 5)
        {
            qWarning() << "#if or #elif command is incomplete";
            return;
        }

        std::string cmd = tokens[1];
        if(cmd == "player" || cmd == "state")
        {
            if(tokens[2] != "(")
            {
                qWarning() << "Invalid syntax: Should be a ( after command name!";
                return;
            }

            std::vector<int> numbers;
            bool wantComma = false;

            for(size_t i = 3; i < tokens.size(); ++i)
            {
                auto &s = tokens[i];

                if(wantComma)
                {
                    if(s != "," && s != ")")
                    {
                        qWarning() << "Invalid syntax: Should be a , or ) after number!";
                        return;
                    }

                    if(s == ")") // Arguments parsing done
                    {
                        if(i != tokens.size() - 1)
                        {
                            qWarning() << "Invalid syntax: No extra data allowed after )!";
                            return;
                        }
                        break;
                    }

                    wantComma = false;
                }
                else
                {
                    if(!isUNum(s))
                    {
                        qWarning() << "Invalid syntax: Should be a numeric value!" << QString::fromStdString(s);
                        return;
                    }

                    wantComma = true;
                    numbers.push_back(std::atoi(s.c_str()));
                }
            }

            QVector<int> qNumbers = QVector<int>(numbers.begin(), numbers.end());
            qDebug() << QString::fromStdString(cond) << QString::fromStdString(cmd) << qNumbers;
        }
    }
    else if(cond == "else")
    {
        if(tokens.size() > 1)
            qWarning() << "#else denies nore than one token!";
        else
            qDebug() << "Caught #else";
    }
    else if(cond == "endif")
    {
        if(tokens.size() > 1)
            qWarning() << "#endif denies nore than one token!";
        else
            qDebug() << "Caught #endif";
    }
    else
    {
        qWarning() << "Unknown macro!";
    }
}

void testFullString(const char *str, int player, int state)
{
    std::string out;
    msgMacroProcess(str, out, player, state);

    printf("Output:\n-------------------------\n%s\n------------------------------\n", out.c_str());
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    testTokensList("#if player(1, 3, 4)");
    testTokensList("#if player(1,    3,4)");
    testTokensList("#if state(1,  3,     4)");
    testTokensList("#if player(1,    3,            4)");
    testTokensList("#if player(3331,    34,     23333)");
    testTokensList("#if player(1,3,4)");
    testTokensList("#elif player(1, 4)");
    testTokensList("#else");
    testTokensList("#endif");
    testTokensList("#damn");
    testTokensList("#   heck");

    testRealCondition("#if player(1, 3, 4)");
    testRealCondition("#if player(1,    3,4)");
    testRealCondition("#if state(1,  3,     4)");
    testRealCondition("#if player(1,    3,            4)");
    testRealCondition("#if player(3331,    34,     23333)");
    testRealCondition("#if player(1,3,4)");
    testRealCondition("#elif player(1, 4)");
    testRealCondition("#else");
    testRealCondition("#endif");
    testRealCondition("#if player(1,    3,4)331");
    testRealCondition("#if player(1 rgh3,4)331");
    testRealCondition("#if player(,1 rgh3,4)331");
    testRealCondition("#if player=),1 rgh3,4)331");
    testRealCondition("#if player    =),1 rgh3,4)331");
    testRealCondition("#   if     player            (   1   ,     3   , 4  )   ");
    testRealCondition("#damn");
    testRealCondition("#   heck");

    const char * sample =
        "Hello!\n"
        "#if player(1,2)\n"
        "You are plumber?!\n"
        "#elif player(3)\n"
        "You are princess?!\n"
        "#elif player(4)\n"
        "What the heck yo uare here?! Go clean toilets!\n"
        "#else\n"
        "Welcome to our world, stranger!\n"
        "#endif\n"
        "I have nothing to tell you!";

    printf("Macro:\n-------------------------\n%s\n------------------------------\n", sample);
    fflush(stdout);

    testFullString(sample,  1, 2);
    testFullString(sample,  2, 2);
    testFullString(sample,  3, 2);
    testFullString(sample,  4, 2);
    testFullString(sample,  5, 2);

    return 0;
}
