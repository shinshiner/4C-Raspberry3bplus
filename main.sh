# cnt = 1
# while [ $cnt -1e 1 ]; do
# 	echo $cnt
# 	sh record.sh;
# 	sh run.sh;
# 	sh pcm2wav.sh res
# 	cnt = $((cnt + 1))
# done
# echo "finished"

sh record.sh;
sh run.sh;
sh pcm2wav.sh ./samples/bin/res