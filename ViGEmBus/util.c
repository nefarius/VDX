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
    // Vendor "C0:13:37"
    UCHAR mac[6] = { 0xC0, 0x13, 0x37, 0x00, 0x00, 0x00 };
    ULONG seed;

    if (Length < 6)
        return;

    // NIC (random)
    for (size_t i = 3; i < 6; i++)
    {
        seed = KeQueryPerformanceCounter(NULL).LowPart;
        mac[i] = RtlRandomEx(&seed) % 0xFF;
    }

    RtlCopyBytes(Array, mac, Length);
}
