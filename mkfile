FOLDERS=\
	libwidget	\
	cmd

all:V:

%:V: $FOLDERS
	for(f in $FOLDERS) @{
		cd $f; mk $stem
	}
