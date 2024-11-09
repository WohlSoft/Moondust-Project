#pragma once

#include <QByteArray>

struct Mix_Chunk;

namespace XTConvert
{

namespace ExportWAV
{

void export_wav(QByteArray& out, const Mix_Chunk* chunk);

} // namespace ExportWAV

} // namespace XTConvert
