BLE Eddystone demo
===========================
This is a BLE advertising demo with virtual HCI interface. Send Reset/ADV_PARAM/ADV_DATA/ADV_ENABLE HCI command for BLE advertising.

esp-idf used
	commit 1e0710f1b24429a316c9c34732aa17bd3f189421
	Date:   Fri May 12 18:23:20 2017 +0800

eclipse
	include include.xml to C and C++ Paths

INSTALL
	make menuconfig
	(make clean)
	make -j8 all
	make flash
	make monitor



