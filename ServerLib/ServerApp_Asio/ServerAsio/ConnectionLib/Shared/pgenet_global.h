#ifndef PGENET_GLOBAL_H
#define PGENET_GLOBAL_H


class PGENET_Global
{
public:
    static const short Port = 24444;
    static const long long NetworkVersion = 1;
};

///
/// \brief The PGEPacketsV2 enum
///
enum class PacketID : int {
    PGENET_UNDEFINED,
    PGENET_PacketUserAuth,
    PGENET_PacketMessage
};

#endif // PGENET_GLOBAL_H
