#include <stdint.h>


#define FbinOp(S, ASM) uint64_t S(uint64_t a, uint64_t b) \
{                       \
    uint64_t res = 0;   \
asm volatile (          \
    "llihf %%r0,801\n"  \
    "lg %%r2, %[a]\n"   \
    "lg %%r3, %[b]\n"   \
    ASM                 \
    "stg %%r0, %[res] " \
    : [res] "=m" (res)  \
    : [a] "m" (a)       \
    , [b] "m" (b)       \
    : "r0", "r2", "r3"  \
);                      \
    return res;         \
}


/* AND WITH COMPLEMENT */
FbinOp(_ncrk,  ".insn rrf, 0xB9F50000, %%r0, %%r3, %%r2, 0\n")
FbinOp(_ncgrk, ".insn rrf, 0xB9E50000, %%r0, %%r3, %%r2, 0\n")

/* NAND */
FbinOp(_nnrk,  ".insn rrf, 0xB9740000, %%r0, %%r3, %%r2, 0\n")
FbinOp(_nngrk, ".insn rrf, 0xB9640000, %%r0, %%r3, %%r2, 0\n")

/* NOT XOR */
FbinOp(_nxrk,  ".insn rrf, 0xB9770000, %%r0, %%r3, %%r2, 0\n")
FbinOp(_nxgrk, ".insn rrf, 0xB9670000, %%r0, %%r3, %%r2, 0\n")

/* NOR */
FbinOp(_nork,  ".insn rrf, 0xB9760000, %%r0, %%r3, %%r2, 0\n")
FbinOp(_nogrk, ".insn rrf, 0xB9660000, %%r0, %%r3, %%r2, 0\n")

/* OR WITH COMPLEMENT */
FbinOp(_ocrk,  ".insn rrf, 0xB9750000, %%r0, %%r3, %%r2, 0\n")
FbinOp(_ocgrk, ".insn rrf, 0xB9650000, %%r0, %%r3, %%r2, 0\n")


int main(int argc, char *argv[])
{
    if (_ncrk(0xFF88, 0xAA11)  != 0x0000032100000011ull ||
        _nnrk(0xFF88, 0xAA11)  != 0x00000321FFFF55FFull ||
        _nork(0xFF88, 0xAA11)  != 0x00000321FFFF0066ull ||
        _nxrk(0xFF88, 0xAA11)  != 0x00000321FFFFAA66ull ||
        _ocrk(0xFF88, 0xAA11)  != 0x00000321FFFFAA77ull ||
        _ncgrk(0xFF88, 0xAA11) != 0x0000000000000011ull ||
        _nngrk(0xFF88, 0xAA11) != 0xFFFFFFFFFFFF55FFull ||
        _nogrk(0xFF88, 0xAA11) != 0xFFFFFFFFFFFF0066ull ||
        _nxgrk(0xFF88, 0xAA11) != 0xFFFFFFFFFFFFAA66ull ||
        _ocgrk(0xFF88, 0xAA11) != 0xFFFFFFFFFFFFAA77ull)
    {
        return 1;
    }

    return 0;
}
