#!/bin/bash

let i=1

for pos_file in func_tests/pos_*.txt
do
    echo "Input file #${i}:"
    cat ${pos_file}
    echo ""
    ./app.exe config/steckerbrett.txt config/reflector.txt ${pos_file} encrypted.txt
    echo "Encrypted file:"
    cat encrypted.txt
    echo ""
    ./app.exe config/steckerbrett.txt config/reflector.txt encrypted.txt decrypted.txt
    echo "Decrypted file:"
    cat decrypted.txt
    echo ""
    i=$((i+1))
done

cd ..
