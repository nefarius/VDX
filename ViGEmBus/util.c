#include <ntifs.h>
#include <busenum.h>


VOID ReverseByteArray(PUCHAR Array, INT Length)
{
    PUCHAR s = (PUCHAR)ExAllocatePoolWithTag(NonPagedPool, sizeof(UCHAR) * Length, VIGEM_POOL_TAG);
    INT c, d;

    if (s == NULL)
        return;

    for (c = Length - 1, d = 0; c >= 0; c--, d++)
        *(s + d) = *(Array + c);

    for (c = 0; c < Length; c++)
        *(Array + c) = *(s + c);

    ExFreePoolWithTag(s, VIGEM_POOL_TAG);
}

VOID GenerateRandomMacAddress(PUCHAR Array, INT Length)
{
    UCHAR mac[6] = { 0 };
    ULONG seed;

    if (Length < 6)
        return;

    // Vendor "C0:13:37"
    mac[0] = 0xC0;
    mac[1] = 0x13;
    mac[2] = 0x37;

    // NIC (random)
    for (size_t i = 3; i < 6; i++)
    {
        seed = KeQueryPerformanceCounter(NULL).LowPart;
        mac[i] = RtlRandomEx(&seed) % 0xFF;
    }

    RtlCopyBytes(Array, mac, Length);
}
