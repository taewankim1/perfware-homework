set -e
make clean
make
trials="listing_0037_single_register_mov \
listing_0038_many_register_mov \
listing_0039_more_movs"
# listing_0041_add_sub_cmp_jnz"
# trial="listing_0038_many_register_mov"
for trial in $trials ; do
    echo
    echo "Test [$trial]"
    ./sim8086 ${trial} > disasm_${trial}.asm
    nasm disasm_${trial}.asm
    diff -q ${trial} disasm_${trial}
    echo "Test [$trial] passed"
    echo
done