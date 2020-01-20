#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDialog>
#include <QTranslator>

namespace Ui
{
    class MaintainerMain;
}

class MaintainerMain : public QDialog
{
    Q_OBJECT

public:
    explicit MaintainerMain(QWidget *parent = 0);
    ~MaintainerMain();

private slots:
    void on_quitOut_clicked();
    void on_music_converter_batch_clicked();
    void on_episode_converter_clicked();
    void on_gifs2png_converter_clicked();
    void on_png2gifs_converter_clicked();
    void on_pathcase_fixer_clicked();

private:
    Ui::MaintainerMain *ui;

// //////////////////Miltilanguage///////////////////////////
private:
    /*!
     * \brief Load language by two-letter code (en, ru, de, it, es, etc.)
     * \param rLanguage Two-letter language name (en, ru, de, it, es, etc.)
     */
    void loadLanguage(const QString& rLanguage);
    /*!
     * \brief Init settings of the translator and initialize default language
     */
    void setDefLang();
    /*!
     * \brief Switch translator module
     * \param translator Translator module
     * \param filename Path to the translation file
     * \return true if successfully loaded, false on any error
     */
    bool switchTranslator(QTranslator &translator, const QString &filename);
    /*!
     * \brief Syncronize state of the languages menu
     */
    //! contains the translations for this application
    QTranslator     m_translator;
    //! contains the translations for qt
    QTranslator     m_translatorQt;
    //! contains the currently loaded language
    QString         m_currLang;
    //! Path of language files. This is always fixed to /languages
    QString         m_langPath;
signals:
    //! Is editor language was switched
    void languageSwitched();
// ///////////////////////////////////////////////////////////
};

#endif // MAIN_WINDOW_H
