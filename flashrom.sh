
FILE=$1
CHIP=GD25B128B/GD25Q128B
TEMPFILE=__temp.bin

if [ ! $FILE ]; then
	echo "Please input file name"
	echo "./flashrom.sh file"
	exit
fi

if [ -f ${FILE} ];then
	echo file ${FILE} exist!
    else
	echo file ${FILE} not exist!
	exit
fi

dd if=/dev/zero of=$TEMPFILE bs=1M count=16
dd if=$FILE of=$TEMPFILE conv=notrunc
flashrom -p serprog:dev=/dev/ttyACM0,spispeed=31250000 -c $CHIP --layout rom.layout --image bootloader -w $TEMPFILE

echo "remove temp file: $TEMPFILE"
rm $TEMPFILE
