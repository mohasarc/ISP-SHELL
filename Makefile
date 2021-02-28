

all: isp producer consumer

producer: ./src/producer.c
	gcc -Wall -g -o producer ./src/producer.c

consumer: ./src/consumer.c
	gcc -Wall -g -o consumer ./src/consumer.c

isp: ./src/isp.c
	gcc -Wall -g -o isp ./src/isp.c

clean:
	rm -fr isp producer consumer *dSYM
