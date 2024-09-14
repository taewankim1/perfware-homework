set -e
make clean
rm -f ./listing/disasm_*
make
trials="listing_0037_single_register_mov listing_0038_many_register_mov listing_0039_more_movs listing_0043_immediate_movs listing_0044_register_movs listing_0046_add_sub_cmp listing_0048_ip_register listing_0051_memory_mov"
# listing_0041_add_sub_cmp_jnz"
# trial="listing_0038_many_register_mov"
for trial in $trials ; do
    echo
    echo "Test [$trial]"
    ./sim8086 ./listing/${trial} > ./listing/disasm_${trial}.asm
    nasm ./listing/disasm_${trial}.asm
    diff -q ./listing/${trial} ./listing/disasm_${trial}
    echo "Test [$trial] passed"
    echo
done