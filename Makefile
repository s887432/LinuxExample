CC=arm-linux-gnueabihf-gcc

lcd1602_test: lcd1602_test.c
	@$(CC) -o lcd1602_test lcd1602_test.c

show_ip: show_ip.c
	@$(CC) -o show_ip show_ip.c

get_ip: get_ip.c
	@$(CC) -o get_ip get_ip.c

clean:
	@rm -rf lcd1602_test get_ip show_ip

.PHONY: clean

