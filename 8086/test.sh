set -e
make clean
make
trial="listing_0038_many_register_mov"
echo
echo "Test [$trial]"
./sim8086 ${trial} > disasm_${trial}.asm
nasm disasm_${trial}.asm
diff -q ${trial} disasm_${trial}
echo "Test [$trial] passed"
echo