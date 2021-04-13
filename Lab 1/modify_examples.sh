#!/bin/bash

print_dir()
{
	find "./testfolder" -print0 | while IFS= read -r -d '' f; do
		echo "$f"
	done
}

echo "---Setup of testing..."

mkdir "testfolder"
mkdir "testfolder/testinsidefolder"

touch "testfolder/test me.txt"
touch "testfolder/testmewithtext.txt"

echo "Created testfolder/test me.txt and testfolder/testmewithtext.txt"
echo "text inside testfolder/testmewithtext" > testfolder/testmewithtext.txt

touch "testfolder/testinsidefolder/test me.txt"
touch "testfolder/testinsidefolder/testmewithtext.txt"

echo "Created testfolder/testmewithtext/test me.txt and testfolder/testmewithtext/testmewithtext.txt"
echo "text inside testfolder/testinsidefolder/testmewithtext" > testfolder/testinsidefolder/testmewithtext.txt

echo -e "Setup done.\n"
echo "---Correct cases"

echo -e "\nTesting help (bash ./modify.sh -h)"
bash ./modify.sh -h

echo -e "\nTesting uppercasing files in ./testfolder non recursive (bash ./modify.sh -u ./testfolder)"
bash ./modify.sh -u ./testfolder
print_dir

echo -e "\nTesting lowercasing files in ./testfolder non recursive (bash ./modify.sh -l ./testfolder)"
bash ./modify.sh -l ./testfolder
print_dir

echo -e "\nTesting uppercasing files in ./testfolder recursive (bash ./modify.sh -r -u ./testfolder)"
bash ./modify.sh -r -u ./testfolder
print_dir

echo -e "\nTesting lowercasing files in ./testfolder recursive (bash ./modify.sh -r -u ./testfolder)"
bash ./modify.sh -r -l ./testfolder
print_dir

echo -e "\nTesting sed pattern in ./testfolder recursive (bash ./modify.sh -r -s "s/text/image/g" ./testfolder)"
echo "Previous context of testfolder/testmewithtext.txt"
cat testfolder/testmewithtext.txt
echo "Previous context of testfolder/testinsidefolder/testmewithtext.txt"
cat testfolder/testinsidefolder/testmewithtext.txt

bash ./modify.sh -r -s "s/text/image/g" ./testfolder

echo "Current context of testfolder/testmewithtext.txt"
cat testfolder/testmewithtext.txt
echo "Current context of testfolder/testinsidefolder/testmewithtext.txt"
cat testfolder/testinsidefolder/testmewithtext.txt

echo -e "\n---Incorrect cases"
echo -e "\nCalling modify without -u or -l or sed pattern (bash ./modify.sh ./testfolder)"
bash ./modify.sh ./testfolder

echo -e "\nCalling modify with non-existent file (bash ./modify.sh -u -r ./testfoldersss)"
bash ./modify.sh -u -r ./testfoldersss

echo -e "\nCalling modify with both -l and -u (bash ./modify.sh -l -u ./testfolder)"
bash ./modify.sh -l -u ./testfolder

echo -e "\nHaving 2 files with same name but in different casing ex: testfolder/testmewithtext.txt and testfolder/testmewithTEXT"
touch testfolder/testmewithTEXT.txt
bash ./modify.sh -r -l ./testfolder
