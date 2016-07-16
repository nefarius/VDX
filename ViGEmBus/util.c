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

VOID GenerateRandomMacAddress(PMAC_ADDRESS Address)
{
    // Vendor "C0:13:37"
    Address->Vendor0 = 0xC0;
    Address->Vendor1 = 0x13;
    Address->Vendor2 = 0x37;

    ULONG seed = KeQueryPerformanceCounter(NULL).LowPart;
    
    Address->Nic0 = RtlRandomEx(&seed) % 0xFF;
    Address->Nic1 = RtlRandomEx(&seed) % 0xFF;
    Address->Nic2 = RtlRandomEx(&seed) % 0xFF;
}
