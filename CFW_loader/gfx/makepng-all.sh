for f in menu?.bin nand*.bin arm*.bin options.bin
do
	convert -size 240x320 -depth 8 bgr\:$f -size 320x240 -rotate 270 ${f%.*}.png
done

for f in *TOP.bin
do
convert -size 240x400 -depth 8 bgr\:$f -size 400x240 -rotate 270 ${f%.*}.png
done
