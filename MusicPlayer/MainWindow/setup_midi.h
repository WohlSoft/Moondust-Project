#ifndef SETUP_MIDI_H
#define SETUP_MIDI_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class SetupMidi;
}

class SetupMidi : public QDialog
{
    Q_OBJECT

public:
    explicit SetupMidi(QWidget *parent = nullptr);
    ~SetupMidi();

    void loadSetup();
    void saveSetup();

    QString getRawMidiArgs();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_opnEmulator_currentIndexChanged(int index);
    void on_opnVolumeModel_currentIndexChanged(int index);
    void on_opn_bank_browse_clicked();
    void on_opn_bank_editingFinished();

    void on_adl_bankId_currentIndexChanged(int index);
    void on_adl_bank_browse_clicked();
    void on_adl_bank_editingFinished();

    void on_adlEmulator_currentIndexChanged(int index);
    void on_adlVolumeModel_currentIndexChanged(int index);

    void on_adl_tremolo_clicked();
    void on_adl_vibrato_clicked();
    void on_adl_adlibMode_clicked();
    void on_adl_scalableModulation_clicked();

    void updateAutoArgs();

    void on_mididevice_currentIndexChanged(int index);

    void on_resetDefaultADLMIDI_clicked();

    void on_midiRawArgs_editingFinished();

    void on_opn_use_custom_clicked(bool checked);

    void on_adl_use_custom_clicked(bool checked);

signals:
    void songRestartNeeded();

private:
    Ui::SetupMidi *ui;
    void restartForAdl();
    void restartForOpn();
    bool m_setupLock = false;
};

#endif // SETUP_MIDI_H
