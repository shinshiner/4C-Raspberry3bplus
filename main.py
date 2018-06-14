import os

if __name__ == '__main__':
	flag = True
	while flag:
		os.system('sh record.sh')
		os.system('sh run.sh')
		os.system('sh pcm2wav.sh ./samples/bin/res')
		flag = raw_input('继续请按Y，退出请按N')
		if flag == 'Y' or flag == 'y':
			flag = True
		elif flag == 'N' or flag == 'n':
			flag = False
	print('谢谢使用')