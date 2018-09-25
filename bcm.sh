#Downloads the bcm2835 library for the Raspberry Pi

#ANSI COLORS
RED='\033[0;31m'
NC='\033[0m'
GREEN='\033[0;32m'
#BEGIN
echo 'Getting necessary files...'
wget -q http://www.airspayce.com/mikem/bcm2835/bcm2835-1.57.tar.gz
if [[ -f wget-log ]]; then
	rm wget-log
fi
if [ -f bcm2835-1.57.tar.gz ]; then
	echo ${GREEN}'Done downloading the files. Now Installing' ${NC} 	
	tar zxvf bcm2835-1.57.tar.gz
	rm bcm2835-1.57.tar.gz
	
	cd bcm2835-1.57
	./configure
	make
	sudo make check
	sudo make install
	echo ${GREEN}'Installation completed!!'${NC}
	cd ..
	rm -rf bcm2835-1.50
else
	echo ${RED}'Problem downloading the file. Check you internet connection and try again!'${NC}
fi 