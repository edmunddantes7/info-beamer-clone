make
if [ $? -eq 0 ]; then
	./info-beamer
else
	echo "Project build failed!"
fi
