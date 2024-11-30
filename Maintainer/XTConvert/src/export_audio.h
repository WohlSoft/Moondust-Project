#pragma once

#include <QByteArray>

struct Mix_Chunk;

namespace XTConvert
{

namespace ExportAudio
{

void export_wav(QByteArray& out, const Mix_Chunk* chunk);

void export_qoa(QByteArray& out, const Mix_Chunk* chunk);

} // namespace ExportAudio

} // namespace XTConvert
