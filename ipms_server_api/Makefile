#write by garfield  11/28/2013

EXENAME=ipmsd

vpath %.cpp ./
vpath %.cpp code_lib/src/ 

OBJS =	web_server.o \
        netmask.o \
        errors.o \
        mail.o \
        log.o \
        database.o \
        server.o
    
CC = g++ -O3 -DVMWARE_PORT=51108 -fPIC -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
#-O2 -g
CFLAGS =  -I./common/include   -I./code_lib/include  -DEXENAME='"$(EXENAME)"'  

$(EXENAME):$(OBJS)
	$(CC) $(OBJS) -o ./$(EXENAME) -lm -lpthread  -lssl    -lmysqlclient -I/usr/include/mysql/ -L/usr/lib64/mysql ./IPMS_config_so/config.so 
%.o:%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@ 
%.o:%.c
	gcc $(CFLAGS) -c $< -o $@ 

clean:
	-rm *.o ./$(EXENAME)

